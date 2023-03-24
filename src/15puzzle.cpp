#include "15puzzle.h"

namespace puzzle {
permut_type permut_create(std::initializer_list<unsigned> p) {
    permut_type ret_p = 0;
    for (auto e : p) {
        ret_p <<= P_ELEM_OFFESET;
        ret_p |= static_cast<permut_type>(e);
    }
    return ret_p;
}

std::array<unsigned, PUZZLE_SIZE * PUZZLE_SIZE> permut_to_array(permut_type p) {
    std::array<unsigned, PUZZLE_SIZE* PUZZLE_SIZE> ret_arr = {};
    for (auto it = ret_arr.end() - 1; it >= ret_arr.begin(); --it) {
        *it = p & P_ELEM_MASK;
        p >>= P_ELEM_OFFESET;
    }
    return ret_arr;
}
}  // namespace puzzle