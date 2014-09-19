#!/usr/bin/python

import json
import re
import copy
import argparse
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import numpy as np

def read_results(input_file, kernels_regexp_list):
    '''
    Read JSON output of volk_profile returning  dict of results with filename
    being the top level keys. Value is a list of dicts where each dict
    contains results of a specific kernel
    '''
    res = {}
    for file_name in input_file:
        # read every input file
        with open(file_name, 'r') as f:
            # read all results in this file
            tmp_res = json.load(f)['volk_tests']
            tmp_res_keep = {}
            for kernel in tmp_res:
                # filter through kernels we care about
                for regexp in kernels_regexp_list:
                    if re.search(regexp, kernel['name']):
                        tmp_res_keep[kernel['name']] = kernel
            res[file_name] = tmp_res_keep
    return res

def normalize_a_file(results, test_case, protokernel_normalizer, file_normalizer):
    '''
    This is currently a do-nothing
    '''
    normalized_results = {}
    for kernel in results[test_case].keys():
        #normalized_results[kernel] = {'results':{}}
        normalized_results[kernel] = copy.deepcopy(results[test_case][kernel])
        for protokernel in results[test_case][kernel]['results'].keys():
            this_val = results[test_case][kernel]['results'][protokernel]['time']
            try:
                normalize_val = results[file_normalizer][kernel]['results'][protokernel_normalizer]['time']
            except KeyError as e:
                print "kernel {0} has no '{1}' key to normalize by in {2}".format(kernel, protokernel_normalizer, test_case)
                raise e
            normalized_results[kernel]['results'][protokernel]['time'] = this_val / normalize_val
            normalized_results[kernel]['results'][protokernel]['units'] = 'speedup'
    return normalized_results

def normalize_results(results, protokernel_normalizer, file_normalizer):
    '''
    Normalize all results by given normalizers. There's no error
    checking, so this will likely fail hard if given bogus names
    '''
    normalized_results = {}
    if protokernel_normalizer is not None and file_normalizer is not None:
        # normalize all kernels by the named protokernel in the given file
        # e.g. usage:  comparing speedup of different compiler flags on same hw
        for test_case in results.keys():
            normalized_results[test_case] = normalize_a_file(results, test_case, protokernel_normalizer, file_normalizer)
    elif protokernel_normalizer is not None:
        # normalize all kernels in each file by the name protokernel. If
        # multiple files have been given then each file is normalized
        # independtly
        # e.g. usage: aggregating results of different hw
        for test_case in results.keys():
            normalized_results[test_case] = normalize_a_file(results, test_case, protokernel_normalizer, test_case)
    elif file_normalizer is not None:
        # normalize all protokernels by the same protokernels of the named file
        # e.g. usage: very similar to first case
        # todo...
        pass
    else:
        # do nothing
        normalized_results = results
    return normalized_results

def plot_results(results):
    '''
    Actually plot the results now
    '''

    # Set up the figure window
    # we create a subgraph for each kernel, so we need to count nkernels first
    # and create a gridspec of nkernels x 1
    fig = plt.figure(figsize=[6,10])

    width = 0.12
    colors = ['orange', 'red', 'blue', 'green', 'purple', 'yellow', 'black', ]
    sub_graphs = set()
    for test_case in results.keys():
        for kernel in results[test_case].keys():
            sub_graphs.add(kernel)
    nkernels = len(sub_graphs)
    axarray = gridspec.GridSpec(nkernels, 1, hspace=1.25)

    # Next, for each kernel create lists of rectangles and labels
    ii = 0
    ax0 = plt.subplot(axarray[ii])
    for kernel in sub_graphs:
        protokernel_times = []
        protokernel_names = []
        for test_case in results.keys():
            protokernels = results[test_case][kernel]['results'].keys()
            for pk in protokernels:
                protokernel_times.append(results[test_case][kernel]['results'][pk]['time'])
                protokernel_names.append(test_case + ":" + pk)
        num_protokernels = protokernel_names.__len__()
        # create the subplot for this kernel
        ax = plt.subplot(axarray[ii], sharex=ax0)
        plt.sca(ax) # hilight the new axis
        # actually plot the data
        rects = ax.barh( np.arange(num_protokernels)/20.0, protokernel_times, height=0.05, color=colors)
        # write the labels on the y-axis
        plt.yticks((np.arange(num_protokernels)+0.25)/20.0, protokernel_names, rotation=0)
        plt.title(kernel)
        plt.ylim([0, num_protokernels/20.0])
        ii += 1

    ax0.set_xlim(0, 1.1)
    plt.show()


    print rects


def parse_options():
    parser = argparse.ArgumentParser(description="Plot the results of volk profiling")
    parser.add_argument('--kernels', dest='kernels', type=str, nargs='+', default=['.*'],
                        help='The VOLK kernels to plot using a space seperated list of regexps (--list-kernels for a list of options)')
    parser.add_argument("--file", dest="infile", type=str, nargs="+",
                            help="The input file(s) to process")
    parser.add_argument("--protokernel_normalizer", dest="protokernel_normalizer", type=str,
                            help="The protokernel name to normalize by (usually generic)")
    parser.add_argument("--file_normalizer", dest="file_normalizer", type=str,
                            help="The file name to normalize by (usually generic)")
    return parser.parse_args()

if __name__ == '__main__':
    options = parse_options()
    res = read_results(options.infile, options.kernels)
    res = normalize_results(res, options.protokernel_normalizer, options.file_normalizer)
    print res
    plot_results(res)

