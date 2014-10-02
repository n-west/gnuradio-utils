
from os import listdir, chdir, getcwd
from os.path import isfile, join
from copy import deepcopy
import re
import argparse
import subprocess

def main():
    desc = 'Create an html table showing ISA coverage for VOLK'
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument('--volk_root', type=str, default=None, required=True,
                            help='The volk root (/opt/code/gnuradio/volk)')
    parser.add_argument('--namespace', type=str, default='volk',
                            help='The volk namespace')
    parser.add_argument('--isa_list', type=str, nargs='*', default=['generic', 'sse', 'avx', 'neon'],
                            help='The isas to count.')
    parser.add_argument('--output_file', type=str, default="cov_table.html",
                            help='The filename to write the table to')

    args = parser.parse_args()

    cwd = getcwd()
    chdir(args.volk_root)
    p = subprocess.Popen(["git", "describe"], stdout=subprocess.PIPE)
    version_string = re.sub('git.*$', '', p.communicate()[0])
    version_string = re.sub('[a-zA-Z\s]', '', version_string)
    version_parts = version_string.split('.')

    chdir(cwd)

    kernel_dir = "{0}/kernels/{1}".format(args.volk_root, args.namespace)

    kernel_files = get_kernel_list(kernel_dir)

    kernel_counts = count_kernels(kernel_dir, kernel_files, args.namespace, args.isa_list)

    # send in the isa list as a tuple so the order never changes
    write_table(args.output_file, version_parts, kernel_counts, tuple(args.isa_list))

def get_kernel_list(kernel_dir):
    '''
    Given the volk directory and namespace return a list of kernels
    '''
    kernel_files = [ f for f in listdir(kernel_dir) if isfile(join(kernel_dir,f)) ]
    return kernel_files

def count_kernels(kernel_dir, kernel_files, ns, isa_list):
    '''
    Given a list of kernel files count the ISA implementations
    and return them in a dict
    '''

    kernel_counts = {}
    for kernel_file in kernel_files:
        kernel_name = kernel_file.replace(ns+'_', '').replace('.h', '')
        whole_path = "{0}/{1}".format(kernel_dir, kernel_file)
        kernel_counts[kernel_name] = count_isas(whole_path, kernel_name, isa_list, ns)
    return kernel_counts

def count_isas(file_path, kernel_name, isa_list, ns):
    '''
    Count ISAs from a specific file
    '''
    f_handle = open(file_path, 'r')
    f_contents = f_handle.read()
    counts = {}
    for isa in isa_list:
        counts[isa] = {'u':0, 'a':0}
        pattern = "{0}_{1}_(\w*)({2})\w*\(".format(ns, kernel_name, isa)
        matches = re.findall(pattern, f_contents)
        for match in matches:
            if match[0].startswith('a'):
                counts[isa]['a'] += 1
            else:
                counts[isa]['u'] += 1
    return counts

def write_table(output_filename, gr_version, results, isa_list):
    '''
    Write results dict to html table
    '''

    f = open(output_filename, "w")
    # Write the header garbage
    f.write('<html>\n')
    f.write('<style>\n')
    f.write('table {\n')
    f.write('background: #f5f5f5;\n')
    f.write('border-collapse: separate;\n')
    f.write('box-shadow: inset 0 1px 0 #fff;\n')
    f.write('font-size: 14px;\n')
    f.write('line-height: 24px;\n')
    f.write('margin: 20px ;\n')
    f.write('text-align: center;\n')
    f.write('}\n')
    f.write('</style>\n')
    f.write('VOLK ISA coverage for each kernel in GNU Radio v' + '.'.join(gr_version) + '\n\n\n')

    f.write('<table border="1">\n')
    f.write(' <tr>\n')
    f.write('   <td></td>')
    for isa in isa_list:
        f.write(' <td colspan="2">{0}</td>'.format(isa))
    f.write('</tr>\n    <tr>\n')
    f.write('   <td>Kernel</td>' + ' <td width="20px">u</td><td width="20px">a</td>'*isa_list.__len__() + '</td>')

    # Write the actual results
    for kernel in sorted(results.keys()):
        row_string = '  <tr>\n'
        row_string += '    <td align="left">{0}</td>\n'.format(kernel)
        for isa in isa_list:
            row_string += format_cell(results[kernel][isa])
        row_string += '</tr>\n'
        f.write(row_string)

    # close out the table
    f.write('</table>\n</html>\n')
    f.close()

def format_cell(results):
    '''
    Format a cell in the table.
    Basically, print the number of implementations and
    color the background red or green
    '''
    good_color = '#78FA7A'
    bad_color = '#FF878B'
    if results['u'] > 0:
        u_color = good_color
    else:
        u_color = bad_color
    if results['a'] > 0:
        a_color = good_color
    else:
        a_color = bad_color
    td = '    <td bgcolor="{1}">{0}</td><td bgcolor="{3}">{2}</td>\n'.format(results['u'], u_color, results['a'], a_color)
    return td



if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt as e:
        raise(e)
