#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <queue>
#include <string>

#include "15puzzle_solver.h"

#define PUZZLE_SIZE 3

namespace puzzle {
using permut_type = uint64_t;

template <uint32_t psize = PUZZLE_SIZE>
constexpr permut_type permut_create(std::initializer_list<uint32_t> p) {
    constexpr auto offset = std::bit_width(psize * psize - 1);
    permut_type ret_p = 0;
    for (auto e : p) {
        ret_p <<= offset;
        ret_p |= static_cast<permut_type>(e);
    }
    return ret_p;
}

template <uint32_t psize = PUZZLE_SIZE>
std::array<uint32_t, psize * psize> permut_to_array(permut_type p) {
    constexpr int offset = std::bit_width(psize * psize - 1);
    constexpr uint32_t mask = ~(~0U << offset);
    std::array<uint32_t, psize* psize> ret_arr = {};
    for (auto it = ret_arr.end() - 1; it >= ret_arr.begin(); --it) {
        *it = p & mask;
        p >>= offset;
    }
    return ret_arr;
}

template <uint32_t psize = PUZZLE_SIZE>
std::string permut_to_string(permut_type permut) {
    std::string result;
    result.reserve(psize * psize * 2);
    auto arr = puzzle::permut_to_array<psize>(permut);
    for (unsigned counter = 0; auto e : arr) {
        if (counter % psize == 0 && counter > 0) {
            result += '\n';
        }
        result += std::to_string(e /*(e + 1) % (psize * psize)*/);
        result += ' ';
        ++counter;
    }
    return result;
}

template <uint32_t psize = PUZZLE_SIZE>
uint32_t manhattan_dist(permut_type a) {
    constexpr int offset = std::bit_width(psize * psize - 1);
    constexpr uint32_t mask = ~(~0U << offset);
    constexpr auto diff = [](uint32_t a, uint32_t b) -> uint32_t {
        return a > b ? a - b : b - a;
    };
    uint32_t dist = 0;
    for (uint32_t pozz = psize * psize - 1; pozz != static_cast<uint32_t>(-1); --pozz) {
        auto temp = a & mask;
        auto pozz_row = pozz / psize;
        auto pozz_col = pozz % psize;
        auto temp_row = temp / psize;
        auto temp_col = temp % psize;
        dist += diff(pozz_row, temp_row);
        dist += diff(pozz_col, temp_col);
        a >>= offset;
    }
    return dist;
}

template <uint32_t psize = PUZZLE_SIZE>
constexpr int find_empty(permut_type permut) {
    constexpr int offset = std::bit_width(psize * psize - 1);
    constexpr permut_type mask = ~(~0U << offset);
    constexpr permut_type empty = psize * psize - 1;
    int empty_offset = 0;
    while ((permut & mask) != empty) {
        empty_offset += offset;
        permut >>= offset;
    }
    return empty_offset;
}

// namespace internal {

template <uint32_t psize = PUZZLE_SIZE>
class permut_neighbors_itr {
    permut_type neighbors[4];
    unsigned len = 0;

public:
    explicit permut_neighbors_itr(permut_type permut) {
        constexpr int offset = std::bit_width(psize * psize - 1);
        constexpr permut_type mask = ~(~0U << offset);
        constexpr int max_offset = (psize * psize - 1) * offset;
        int empty_offset = find_empty<psize>(permut);
        permut_type clean_mask = mask << empty_offset;
        permut_type empty = permut & clean_mask;
        permut_type clean_permut = permut & ~clean_mask;
        if (int curr_offset = empty_offset + psize * offset; curr_offset <= max_offset) {
            permut_type curr_mask = mask << curr_offset;
            neighbors[len] = ((clean_permut & curr_mask) >> (curr_offset - empty_offset)) | (clean_permut & ~curr_mask);
            neighbors[len] |= (empty << (curr_offset - empty_offset));
            ++len;
        }
        if (int curr_offset = empty_offset - offset; curr_offset > 0) {
            permut_type curr_mask = mask << curr_offset;
            neighbors[len] = ((clean_permut & curr_mask) << offset) | (clean_permut & ~curr_mask);
            neighbors[len] |= (empty >> offset);
            ++len;
        }
        if (int curr_offset = empty_offset - psize * offset; curr_offset > 0) {
            permut_type curr_mask = mask << curr_offset;
            neighbors[len] = ((clean_permut & curr_mask) << (empty_offset - curr_offset)) | (clean_permut & ~curr_mask);
            neighbors[len] |= (empty >> (empty_offset - curr_offset));
            ++len;
        }
        if (int curr_offset = empty_offset + offset; curr_offset <= max_offset) {
            permut_type curr_mask = mask << curr_offset;
            neighbors[len] = ((clean_permut & curr_mask) >> offset) | (clean_permut & ~curr_mask);
            neighbors[len] |= (empty << offset);
            ++len;
        }
    }
    permut_type* begin() {
        return &neighbors[0];
    }
    permut_type* end() {
        return &neighbors[len];
    }
};
//}  // namespace internal

template <uint32_t psize = PUZZLE_SIZE, typename Heuristic>
auto find_solution(permut_type initial, Heuristic heuristic_dist) {
}

}  // namespace puzzle
