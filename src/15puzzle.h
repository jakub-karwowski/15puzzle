#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>

#define PUZZLE_SIZE 3
#define P_ELEM_OFFESET 4
#define P_ELEM_MASK 0xf

namespace puzzle {
using permut_type = uint64_t;

permut_type permut_create(std::initializer_list<unsigned> p);

std::array<unsigned, PUZZLE_SIZE * PUZZLE_SIZE> permut_to_array(permut_type p);

}  // namespace puzzle
