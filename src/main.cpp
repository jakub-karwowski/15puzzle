#include <iostream>

#include "15puzzle_solver.h"

int main() {
    //{3, 1, 2, 4, 0, 5, 8, 6, 7}
    //
    puzzle::permut_type p = puzzle::permut_create<>({1, 0, 2, 3, 8, 5, 6, 7, 4});
    // std::cout << puzzle::permut_to_string(p) << "\n\n";
    puzzle::find_solution<3, decltype(puzzle::manhattan_dist<3>)>(p, puzzle::manhattan_dist<3>);

    return 0;
}