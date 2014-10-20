
#include <iostream>
#include <stdint.h>

uint64_t byteswap_lut_index(uint8_t* chars)
{
    unsigned int ii;
    uint64_t index = 0;
    for(ii=0; ii < 8; ++ii) {
        index += ((uint64_t)(*(chars+ii))) << (ii*8);
    }
    return index;
}
int main()
{

    uint8_t pattern01_chars[8] = {24, 16, 8, 0, 25, 17, 9, 1};
    uint64_t pattern01 = byteswap_lut_index(pattern01_chars);

    uint8_t pattern23_chars[8] = {26,18,10,2,27,19,11,3};
    uint64_t pattern23 = byteswap_lut_index(pattern23_chars);

    uint8_t pattern45_chars[8] = {28,20,12,4,29,21,13,5};
    uint64_t pattern45 = byteswap_lut_index(pattern45_chars);

    uint8_t pattern67_chars[8] = {30,22,14,6,31,23,15,7};
    uint64_t pattern67 = byteswap_lut_index(pattern67_chars);

    std::cout << "int_lookup01 = vcreate_u8(" << pattern01 << ");" << std::endl;
    std::cout << "int_lookup23 = vcreate_u8(" << pattern23 << ");" << std::endl;
    std::cout << "int_lookup45 = vcreate_u8(" << pattern45 << ");" << std::endl;
    std::cout << "int_lookup67 = vcreate_u8(" << pattern67 << ");" << std::endl;

}
