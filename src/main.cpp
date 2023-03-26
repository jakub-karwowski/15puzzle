#include <iostream>

#include "15puzzle.h"
#include "15puzzle_solver.h"

int main() {
    puzzle::permut_type p = puzzle::permut_create<>({1, 0, 2, 3, 8, 5, 6, 7, 4});
    std::cout << puzzle::permut_to_string(p) << "\n\n";
    puzzle::permut_neighbors_itr<> itr(p);
    puzzle::puzzle_queue queue;
    for (uint32_t i = 8; auto e : itr) {
        std::cout << puzzle::permut_to_string(e) << "\n\n";
    }
    return 0;
}