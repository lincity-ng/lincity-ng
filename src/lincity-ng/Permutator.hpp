#ifndef __LC_Permutator_h__
#define __LC_Permutator_h__

#include "ErrorInterface.hpp"

#include <iostream>
#include <assert.h>
#include <vector>

// Permutator uses a linear feedback shift register to iterate
// through a permutation of a range of values in an efficient
// and deterministic way.
// http://en.wikipedia.org/wiki/Linear_feedback_shift_register

class Permutator {
public:
    Permutator(unsigned int range, unsigned int seed);
    ~Permutator();
    unsigned int getIndex(unsigned int i) const {
        assert(i < range);
        return permutation[i];
    }
    void shuffle(); //take the next power of the permutation
    static void test();
protected:
    unsigned int range;
    //unsigned int *permutation, *p;
    std::vector<unsigned int> permutation, p; 
};

/* Use case:

doSimulation() {
    int size;
    Permutator permutator(size, 10);
    for (int i = 0; i < size; i++) {
        permutator.getIndex(i);
    }
}
*/

#endif

