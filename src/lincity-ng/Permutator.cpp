#include "Permutator.hpp"

Permutator::Permutator(unsigned int range, unsigned int seed) {
    if (range <= 0) DO_ERROR("range must be positive");
    if (seed == 0) DO_ERROR("seed must not be zero");

    this->range = range;

    // determine indexRange as a power of 2 not less than the area
    unsigned int indexRange = 1;
    unsigned int bits = 0;
    while (indexRange <= range)
    {
        indexRange <<= 1;
        bits++;
    }
/*
    unsigned int irreduciblePolynomials[] = {
        0x0, 0x1, 0x3, 0x6,
        0xC, 0x14, 0x30, 0x60,
        0x95, 0x110, 0x240, 0x500,
        0xE08, 0x1860, 0x3802, 0x6000,
        0xB400, 0x12000, 0x20400, 0x50C00,
        0x90000
    };
*/
    static unsigned int irreduciblePolynomials[] = {
        0x0, 0x1, 0x3, 0x6,//4
        0xC, 0x17, 0x30, 0x47,//8
        0x95, 0x110, 0x240, 0x40B,//12
        0xE08, 0x100D, 0x2015, 0x400B,//16
        0xB400, 0x10007, 0x20013, 0x40013,//20
        0x90000, 0x140000, 0x420000, 0xE10000,//24
        0x1200000, 0x2000023, 0x4000013, 0x9000000//28
    };
    if (bits >= sizeof(irreduciblePolynomials) / sizeof(irreduciblePolynomials[0])) {
        DO_ERROR("range exceeded the supported irreducible polynomials");
        assert (false);
    }
    unsigned int irreduciblePolynomial = irreduciblePolynomials[bits];
    unsigned int mappedI = seed;
    //p = new unsigned int[range];
    p.resize(range);
    for (unsigned int i = 0; i < range; i++) {
        do {
            mappedI = (mappedI >> 1) ^ (-(mappedI & 1) & irreduciblePolynomial);
        } while (mappedI > range);
        //p[i] = mappedI - 1;
        p[i] = mappedI - 1;
    }
/*    //has been tested up to 9000000
    // double check the permutaton p
    std::vector <unsigned int> histogram;
    histogram.resize(range);
    for (unsigned int i = 0; i < range; i++) {
        histogram[i] = 0;
    }
    for (unsigned int i = 0; i < range; i++) {
        histogram[p[i]]++;
    }
    for (unsigned int i = 0; i < range; i++) {
        if (histogram[i]!=1)
            std::cout << "irred. polynomial bit "<< bits <<"   index position" << i << std::endl;
        assert (histogram[i] == 1);
    }
    histogram.clear();
*/
    // compute the composition of the above permutaton p.p
    //permutation = new unsigned int[range];
    permutation.resize(range);
    for (unsigned int i = 0; i < range; i++) {
        //permutation[i] = p[p[i]];
        permutation[i] = p[p[i]];
    }
}
Permutator::~Permutator() {
    //delete[] permutation;
    //delete[] p;
    permutation.clear();
    p.clear();
}
void Permutator::shuffle() {
    // compute the composition of the above permutaton p.permutation
    //unsigned int *nextPermutation = new unsigned int[range];
    std::vector <unsigned int> nextPermutation;
    nextPermutation.resize(range);
    for (unsigned int i = 0; i < range; i++) {
        //nextPermutation[i] = p[permutation[i]];
        nextPermutation[i] = p[permutation[i]];
    }
    for (unsigned int i = 0; i < range; i++) {
        //permutation[i] = nextPermutation[i];
        permutation[i] = nextPermutation[i];
    }
    //delete[] nextPermutation;
    nextPermutation.clear();
}

void Permutator::test() {
    unsigned int range = 100;
    Permutator permutator(range, 1);
    //unsigned int *histogram = new unsigned int[range];
    std::vector <unsigned int> histogram;
    histogram.resize(range);
    for (unsigned int i = 0; i < range; i++) {
        //histogram[i] = 0;
        histogram[i] = 0;
    }
    for (unsigned int i = 0; i < range; i++) {
        //histogram[permutator.getIndex(i)]++;
        histogram[permutator.getIndex(i)]++;
    }
    for (unsigned int i = 0; i < range; i++) {
        //assert (histogram[i] == 1);
        assert (histogram[i] == 1);
    }
    //delete[] histogram;
    histogram.clear();
}

