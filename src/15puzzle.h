#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <string>

namespace puzzle {
using permut_type = uint64_t;

template <uint32_t psize>
constexpr permut_type permut_create_from_partial(const std::array<uint32_t, psize * psize - 1>& arr) {
    constexpr auto offset = std::bit_width(psize * psize - 1);
    permut_type ret_p = 0;
    for (auto e : arr) {
        ret_p <<= offset;
        ret_p |= static_cast<permut_type>(e);
    }
    ret_p <<= offset;
    ret_p |= static_cast<permut_type>(psize * psize - 1);
    return ret_p;
}

template <uint32_t psize>
constexpr permut_type permut_create(const std::array<uint32_t, psize * psize>& arr) {
    constexpr auto offset = std::bit_width(psize * psize - 1);
    permut_type ret_p = 0;
    for (auto e : arr) {
        ret_p <<= offset;
        ret_p |= static_cast<permut_type>(e);
    }
    return ret_p;
}

template <uint32_t psize>
constexpr permut_type permut_create(std::initializer_list<uint32_t> p) {
    constexpr auto offset = std::bit_width(psize * psize - 1);
    permut_type ret_p = 0;
    for (auto e : p) {
        ret_p <<= offset;
        ret_p |= static_cast<permut_type>(e);
    }
    return ret_p;
}

template <uint32_t array_size>
constexpr bool parity_check(const std::array<unsigned, array_size>& arr) {
    size_t inv_count = 0;
    for (size_t i = 1; i < arr.size(); ++i) {
        for (size_t j = 0; j < i; ++j) {
            if (arr[i] < arr[j]) {
                ++inv_count;
            }
        }
    }
    return inv_count % 2 == 0;
}

template <uint32_t psize>
permut_type permut_read(std::istream& stream) {
    std::array<uint32_t, psize * psize> arr;
    for (uint32_t i = 0; i < psize * psize; ++i) {
        if (!stream >> arr[i]) {
            return 0;
        }
        arr[i] += (psize - 1);
        arr[i] %= psize;
    }
    return permut_create(arr);
}

template <uint32_t psize>
bool permut_check(const std::array<unsigned, psize * psize>& arr) {
    return arr[psize * psize - 1] != psize * psize - 1 && parity_check<psize * psize>(arr);
}

template <uint32_t psize>
std::array<uint32_t, psize * psize> permut_to_array(permut_type p) {
    static constexpr int offset = std::bit_width(psize * psize - 1);
    static constexpr uint32_t mask = ~(~0U << offset);
    std::array<uint32_t, psize* psize> ret_arr = {};
    for (auto it = ret_arr.rbegin(); it != ret_arr.rend(); ++it) {
        *it = p & mask;
        p >>= offset;
    }
    return ret_arr;
}

template <uint32_t psize>
std::ostream& permut_write(std::ostream& stream, permut_type permut, int width = 2) {
    const auto arr = puzzle::permut_to_array<psize>(permut);
    for (size_t i = 0; i < psize * psize; ++i) {
        stream << std::setw(width) << (arr[i] + 1) % (psize * psize);
        stream << (i % psize == (psize - 1) ? '\n' : ' ');
    }
    return stream;
}

template <uint32_t psize>
constexpr uint32_t manhattan_dist(permut_type a) {
    constexpr int offset = std::bit_width(psize * psize - 1);
    constexpr uint32_t mask = ~(~0U << offset);
    constexpr auto diff = [](uint32_t a, uint32_t b) -> uint32_t {
        return a > b ? a - b : b - a;
    };
    uint32_t dist = 0;
    for (uint32_t pozz = psize * psize - 1; pozz != static_cast<uint32_t>(-1); --pozz) {
        auto temp = a & mask;
        if (temp != psize * psize - 1) {
            auto pozz_row = pozz / psize;
            auto pozz_col = pozz % psize;
            auto temp_row = temp / psize;
            auto temp_col = temp % psize;
            dist += diff(pozz_row, temp_row);
            dist += diff(pozz_col, temp_col);
        }
        a >>= offset;
    }
    return dist;
}

template <uint32_t psize>
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

template <uint32_t psize>
class permut_neighbors_itr {
    permut_type neighbors[4];
    unsigned len = 0;

public:
    explicit permut_neighbors_itr(permut_type permut) {
        static constexpr int offset = std::bit_width(psize * psize - 1);
        static constexpr permut_type mask = ~(~0U << offset);
        static constexpr int max_offset = (psize * psize - 1) * offset;
        const int empty_offset = find_empty<psize>(permut);
        const permut_type clean_mask = mask << empty_offset;
        const permut_type empty = permut & clean_mask;
        const permut_type clean_permut = permut & ~clean_mask;
        if (int curr_offset = empty_offset + psize * offset; curr_offset <= max_offset) {
            permut_type curr_mask = mask << curr_offset;
            neighbors[len] = ((clean_permut & curr_mask) >> (curr_offset - empty_offset)) | (clean_permut & ~curr_mask);
            neighbors[len] |= (empty << (curr_offset - empty_offset));
            ++len;
        }
        if (int curr_offset = empty_offset - offset; curr_offset >= 0 && empty_offset / (psize * offset) == curr_offset / (psize * offset)) {
            permut_type curr_mask = mask << curr_offset;
            neighbors[len] = ((clean_permut & curr_mask) << offset) | (clean_permut & ~curr_mask);
            neighbors[len] |= (empty >> offset);
            ++len;
        }
        if (int curr_offset = empty_offset - psize * offset; curr_offset >= 0) {
            permut_type curr_mask = mask << curr_offset;
            neighbors[len] = ((clean_permut & curr_mask) << (empty_offset - curr_offset)) | (clean_permut & ~curr_mask);
            neighbors[len] |= (empty >> (empty_offset - curr_offset));
            ++len;
        }
        if (int curr_offset = empty_offset + offset; curr_offset <= max_offset && empty_offset / (psize * offset) == curr_offset / (psize * offset)) {
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

}  // namespace puzzle
