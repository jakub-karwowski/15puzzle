#include <iostream>

#include "15puzzle.h"

int main() {
    puzzle::permut_type p = puzzle::permut_create({1, 0, 2, 3, 4, 5, 6, 7, 8});
    auto arr = puzzle::permut_to_array(p);
    for (auto e : arr) {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    return 0;
}