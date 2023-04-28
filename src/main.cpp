#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>

#include "15puzzle_solver.h"

#define PUZZLE_SIZE 4

enum print_mode {
    basic = 0,
    w_initial = 1,
    w_steps = (w_initial << 1),
};

void solution_print(std::ostream& stream, const puzzle::solution& sol, int mode = print_mode::basic) {
    stream << sol.processed << ' ' << sol.touched << ' ' << sol.steps.size() << "\n";
    if (mode & print_mode::w_initial) {
        puzzle::permut_write<PUZZLE_SIZE>(stream, sol.steps.back()) << "...\n";
    }
    if (mode & print_mode::w_steps) {
        auto it = sol.steps.crbegin();
        while (it != sol.steps.crend()) {
            puzzle::permut_write<PUZZLE_SIZE>(stream, *it) << "\n";
            ++it;
        }
    }
}

puzzle::permut_type get_random_permut(std::mt19937& src_of_randomnes) {
    constexpr auto create_initial_permut = []() -> auto{
        if constexpr (PUZZLE_SIZE == 3) {
            return std::array{0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U};
        } else {
            return std::array{0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U};
        }
    };
    constexpr auto initial_permut = create_initial_permut();
    std::array temp_partial_arr{initial_permut};
    do {
        std::shuffle(temp_partial_arr.begin(), temp_partial_arr.end(), src_of_randomnes);
    } while (!puzzle::parity_check<PUZZLE_SIZE * PUZZLE_SIZE - 1>(temp_partial_arr));
    return puzzle::permut_create_from_partial<PUZZLE_SIZE>(temp_partial_arr);
}

int main() {
    std::random_device random_device;
    std::mt19937 src_of_randomnes(random_device());
    auto permut = get_random_permut(src_of_randomnes);
    puzzle::permut_write<PUZZLE_SIZE>(std::cout, permut) << '\n';
    // auto sol = puzzle::find_solution_manhattan<PUZZLE_SIZE>(permut);
    auto additional = puzzle::linear_conflict<PUZZLE_SIZE>;
    auto sol = puzzle::find_solution_manhattan_wadditional<PUZZLE_SIZE, decltype(additional)>(permut, additional);
    if (sol.has_value()) {
        solution_print(std::cout, *sol, print_mode::basic | print_mode::w_steps);
    } else {
        std::cout << "no solution\n";
    }
    return 0;
}
