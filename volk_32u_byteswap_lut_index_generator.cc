
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

    /* These patterns are a 64-bit byteswap */
    //uint8_t pattern01_chars[8] = {25, 17, 9, 1, 24, 16, 8, 0};
    //uint8_t pattern23_chars[8] = {27,19,11,3,26,18,10,2};
    //uint8_t pattern45_chars[8] = {29,21,13,5,28,20,12,4};
    //uint8_t pattern67_chars[8] = {31,23,15,7,30,22,14,6};

    /* These patterns are a 16-bit byteswap */
    uint8_t pattern01_chars[8] = {8,0,24,16,9,1,25,17};
    uint8_t pattern23_chars[8] = {10,2,26,18,11,3,27,19};
    uint8_t pattern45_chars[8] = {12,4,28,20,13,5,29,21};
    uint8_t pattern67_chars[8] = {14,6,30,22,15,7,31,23};

    uint64_t pattern01 = byteswap_lut_index(pattern01_chars);
    uint64_t pattern23 = byteswap_lut_index(pattern23_chars);
    uint64_t pattern45 = byteswap_lut_index(pattern45_chars);
    uint64_t pattern67 = byteswap_lut_index(pattern67_chars);

    std::cout << "int_lookup01 = vcreate_u8(" << pattern01 << ");" << std::endl;
    std::cout << "int_lookup23 = vcreate_u8(" << pattern23 << ");" << std::endl;
    std::cout << "int_lookup45 = vcreate_u8(" << pattern45 << ");" << std::endl;
    std::cout << "int_lookup67 = vcreate_u8(" << pattern67 << ");" << std::endl;

}
