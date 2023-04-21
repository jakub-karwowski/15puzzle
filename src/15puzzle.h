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
using dist_type = uint16_t;

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
inline dist_type manhattan_dist_winfo(dist_type a, int info) {
    return info > 0 ? ++a : --a;
}

template <uint32_t psize>
constexpr dist_type manhattan_dist(permut_type a) {
    constexpr int offset = std::bit_width(psize * psize - 1);
    constexpr uint32_t mask = ~(~0U << offset);
    constexpr auto diff = [](uint32_t a, uint32_t b) -> uint32_t {
        return a > b ? a - b : b - a;
    };
    dist_type dist = 0;
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
constexpr dist_type linear_conflict(permut_type a) {
    constexpr uint32_t empty = psize * psize - 1;
    auto find_max_nonzero = [](const std::array<int, psize>& arr) {
        size_t max_pos = psize;
        int max_val = 1;
        for (size_t i = 0; i < arr.size(); ++i) {
            if (arr[i] >= max_val) {
                max_val = arr[i];
                max_pos = i;
            }
        }
        return max_pos;
    };
    const auto permut_array = permut_to_array<psize>(a);
    dist_type lc_value = 0;
    for (size_t first = 0; first < psize; ++first) {
        std::array<int, psize> n_tiles_in_conflict_row{};
        std::array<int, psize> n_tiles_in_conflict_col{};
        for (size_t second = 0; second < psize - 1; ++second) {
            auto start_tile_row = permut_array[first * psize + second];
            if (start_tile_row != empty && start_tile_row < (first + 1) * psize && start_tile_row >= first * psize) {
                for (size_t other = second + 1; other < psize; ++other) {
                    auto other_tile_row = permut_array[first * psize + other];
                    if (other_tile_row != empty && start_tile_row > other_tile_row &&
                        other_tile_row < (first + 1) * psize && other_tile_row >= first * psize) {
                        ++n_tiles_in_conflict_row[second];
                    }
                }
            }
            auto start_tile_col = permut_array[second * psize + first];
            if (start_tile_col != empty && start_tile_col % psize == first) {
                for (size_t other = second + 1; other < psize; ++other) {
                    auto other_tile_col = permut_array[other * psize + first];
                    if (other_tile_col != empty && start_tile_col > other_tile_col && other_tile_col % psize == first) {
                        ++n_tiles_in_conflict_col[second];
                    }
                }
            }
        }
        {
            size_t nonzero_pos = find_max_nonzero(n_tiles_in_conflict_row);
            while (nonzero_pos < psize) {
                n_tiles_in_conflict_row[nonzero_pos] = 0;
                auto nonzero_tile = permut_array[first * psize + nonzero_pos];
                for (size_t other = nonzero_pos + 1; other < psize; ++other) {
                    auto other_tile_col = permut_array[first * psize + other];
                    if (other_tile_col != empty && nonzero_tile > other_tile_col &&
                        other_tile_col < (first + 1) * psize && other_tile_col >= first * psize) {
                        --n_tiles_in_conflict_row[other];
                    }
                }
                ++lc_value;
                nonzero_pos = find_max_nonzero(n_tiles_in_conflict_row);
            }
        }
        {
            size_t nonzero_pos = find_max_nonzero(n_tiles_in_conflict_col);
            while (nonzero_pos < psize) {
                n_tiles_in_conflict_col[nonzero_pos] = 0;
                auto nonzero_tile = permut_array[nonzero_pos * psize + first];
                for (size_t other = nonzero_pos + 1; other < psize; ++other) {
                    auto other_tile = permut_array[other * psize + first];
                    if (other_tile != empty && nonzero_tile > other_tile && other_tile % psize == first) {
                        --n_tiles_in_conflict_col[other];
                    }
                }
                ++lc_value;
                nonzero_pos = find_max_nonzero(n_tiles_in_conflict_col);
            }
        }
    }
    return lc_value * 2;
}

template <uint32_t psize>
constexpr dist_type manhattan_dist_wlc(permut_type a) {
    return manhattan_dist<psize>(a) + linear_conflict<psize>(a);
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
class permut_neighbors_itr_winfo {
    std::pair<permut_type, int> neighbors[4];
    unsigned len = 0;

public:
    explicit permut_neighbors_itr_winfo(permut_type permut) {
        static constexpr int offset = std::bit_width(psize * psize - 1);
        static constexpr permut_type mask = ~(~0U << offset);
        static constexpr int max_offset = (psize * psize - 1) * offset;
        const int empty_offset = find_empty<psize>(permut);
        const permut_type clean_mask = mask << empty_offset;
        const permut_type empty = permut & clean_mask;
        const permut_type clean_permut = permut & ~clean_mask;
        // shift down
        if (int curr_offset = empty_offset + psize * offset; curr_offset <= max_offset) {
            permut_type curr_mask = mask << curr_offset;
            permut_type curr_tile = (clean_permut & curr_mask);
            neighbors[len].first = (curr_tile >> (curr_offset - empty_offset)) | (clean_permut & ~curr_mask);
            neighbors[len].first |= (empty << (curr_offset - empty_offset));
            curr_tile >>= curr_offset;
            int from_row = psize - 1 - curr_offset / (offset * psize);
            int dest_row = curr_tile / psize;
            neighbors[len].second = from_row < dest_row ? -1 : 1;
            ++len;
        }
        // shift left
        if (int curr_offset = empty_offset - offset; curr_offset >= 0 && empty_offset / (psize * offset) == curr_offset / (psize * offset)) {
            permut_type curr_mask = mask << curr_offset;
            permut_type curr_tile = (clean_permut & curr_mask);
            neighbors[len].first = (curr_tile << offset) | (clean_permut & ~curr_mask);
            neighbors[len].first |= (empty >> offset);
            curr_tile >>= curr_offset;
            int from_col = psize - 1 - ((curr_offset / offset) % psize);
            int dest_col = curr_tile % psize;
            neighbors[len].second = dest_col < from_col ? -1 : 1;
            ++len;
        }
        // shift up
        if (int curr_offset = empty_offset - psize * offset; curr_offset >= 0) {
            permut_type curr_mask = mask << curr_offset;
            permut_type curr_tile = (clean_permut & curr_mask);
            neighbors[len].first = (curr_tile << (empty_offset - curr_offset)) | (clean_permut & ~curr_mask);
            neighbors[len].first |= (empty >> (empty_offset - curr_offset));
            curr_tile >>= curr_offset;
            int from_row = psize - 1 - curr_offset / (offset * psize);
            int dest_row = curr_tile / psize;
            neighbors[len].second = from_row > dest_row ? -1 : 1;
            ++len;
        }
        // shift right
        if (int curr_offset = empty_offset + offset; curr_offset <= max_offset && empty_offset / (psize * offset) == curr_offset / (psize * offset)) {
            permut_type curr_mask = mask << curr_offset;
            permut_type curr_tile = (clean_permut & curr_mask);
            neighbors[len].first = (curr_tile >> offset) | (clean_permut & ~curr_mask);
            neighbors[len].first |= (empty << offset);
            curr_tile >>= curr_offset;
            int from_col = psize - 1 - ((curr_offset / offset) % psize);
            int dest_col = curr_tile % psize;
            neighbors[len].second = dest_col > from_col ? -1 : 1;
            ++len;
        }
    }
    auto begin() {
        return &neighbors[0];
    }
    auto end() {
        return &neighbors[len];
    }
};

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
