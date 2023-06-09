#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <vector>

#include "15puzzle.h"

namespace puzzle {

struct map_entry;
using map_iterator = std::map<permut_type, map_entry>::iterator;

struct map_entry {
    map_iterator parent;
    uint32_t queue_index;
    dist_type dist_to;
    dist_type dist_h;
};

bool comp(map_iterator lhs, map_iterator rhs) {
    auto sum_l = lhs->second.dist_to + lhs->second.dist_h;
    auto sum_r = rhs->second.dist_to + rhs->second.dist_h;
    if (sum_l < sum_r) {
        return true;
    } else if (sum_l == sum_r) {
        return lhs->second.dist_h < rhs->second.dist_h;
    } else {
        return false;
    }
}

class puzzle_queue {
    std::vector<map_iterator> permut_queue;
    std::map<permut_type, map_entry> permut_map;

    static inline size_t left_child(size_t root) noexcept {
        return 2 * root + 1;
    }
    static inline size_t right_child(size_t root) noexcept {
        return 2 * (root + 1);
    }
    static inline size_t parent(size_t child) noexcept {
        return (child - 1) / 2;
    }
    void heapify(size_t root) noexcept {
        auto l = puzzle_queue::left_child(root);
        auto r = puzzle_queue::right_child(root);
        size_t top = root;
        if (l <= permut_queue.size() && comp(permut_queue[l], permut_queue[root])) {
            top = l;
        }
        if (r <= permut_queue.size() && comp(permut_queue[r], permut_queue[top])) {
            top = r;
        }
        if (root != top) {
            std::swap(permut_queue[root]->second.queue_index, permut_queue[top]->second.queue_index);
            std::swap(permut_queue[root], permut_queue[top]);
            heapify(top);
        }
    }
    void build_heap() noexcept {
        for (size_t i = permut_queue.size() / 2; i != static_cast<size_t>(-1); --i) {
            heapify(i);
        }
    }

public:
    void push(permut_type entry, map_iterator parent, dist_type dist_to, dist_type dist_from) {
        size_t i = permut_queue.size();
        map_iterator current_m_entry = permut_map.insert(std::pair{entry, map_entry{parent, static_cast<uint32_t>(i), dist_to, dist_from}}).first;
        permut_queue.push_back(current_m_entry);
        while (i > 0 && comp(permut_queue[i], permut_queue[puzzle_queue::parent(i)])) {
            map_iterator parent_m_entry = permut_queue[puzzle_queue::parent(i)];
            std::swap(current_m_entry->second.queue_index, parent_m_entry->second.queue_index);
            std::swap(permut_queue[i], permut_queue[puzzle_queue::parent(i)]);
            i = puzzle_queue::parent(i);
            current_m_entry = parent_m_entry;
        }
    }
    const map_iterator& top() const noexcept {
        return permut_queue[0];
    }
    void pop() noexcept {
        permut_queue[0] = permut_queue.back();
        permut_queue.pop_back();
        heapify(0);
    }
    bool empty() const noexcept {
        return permut_queue.empty();
    }
    void decrease_key(map_iterator current_m_entry, map_iterator parent_new, dist_type dist_to_new) {
        size_t i = current_m_entry->second.queue_index;
        current_m_entry->second.dist_to = dist_to_new;
        current_m_entry->second.parent = parent_new;
        while (i > 0 && comp(permut_queue[i], permut_queue[puzzle_queue::parent(i)])) {
            map_iterator parent_m_entry = permut_queue[puzzle_queue::parent(i)];
            std::swap(current_m_entry->second.queue_index, parent_m_entry->second.queue_index);
            std::swap(permut_queue[i], permut_queue[puzzle_queue::parent(i)]);
            i = puzzle_queue::parent(i);
            current_m_entry = parent_m_entry;
        }
    }

    map_iterator find(permut_type element) {
        return permut_map.find(element);
    }

    size_t map_size() const noexcept {
        return permut_map.size();
    }

    map_iterator map_end() noexcept {
        return permut_map.end();
    }
};

struct solution {
    size_t touched;
    size_t processed;
    std::vector<permut_type> steps;
};

template <uint32_t psize, typename Heuristic>
std::optional<solution> find_solution(permut_type initial, Heuristic heuristic_dist) {
    constexpr auto create_goal = []() -> permut_type {
        if constexpr (psize == 3) {
            return permut_create<psize>({0, 1, 2, 3, 4, 5, 6, 7, 8});
        }
        return permut_create<psize>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    };
    constexpr permut_type goal = create_goal();
    size_t processed = 0;
    puzzle_queue queue;
    queue.push(initial, queue.map_end(), 0, heuristic_dist(initial));
    while (!queue.empty()) {
        map_iterator current = queue.top();
        if (current->first == goal) {
            std::vector<permut_type> steps;
            while (current->first != initial) {
                steps.push_back(current->first);
                current = current->second.parent;
            }
            steps.push_back(initial);
            return solution{queue.map_size(), processed, std::move(steps)};
        }
        ++processed;
        queue.pop();
        permut_neighbors_itr<psize> neighbours(current->first);
        for (permut_type n : neighbours) {
            const uint32_t dist_new = current->second.dist_to + 1;
            map_iterator n_map_itr = queue.find(n);
            if (n_map_itr == queue.map_end()) {
                queue.push(n, current, dist_new, heuristic_dist(n));
            } else {
                if (dist_new < n_map_itr->second.dist_to) {
                    queue.decrease_key(n_map_itr, current, dist_new);
                }
            }
        }
    }
    return std::nullopt;
}

template <uint32_t psize>
std::optional<solution> find_solution_manhattan(permut_type initial) {
    constexpr auto create_goal = []() -> permut_type {
        if constexpr (psize == 3) {
            return permut_create<psize>({0, 1, 2, 3, 4, 5, 6, 7, 8});
        }
        return permut_create<psize>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    };
    constexpr permut_type goal = create_goal();
    size_t processed = 0;
    puzzle_queue queue;
    queue.push(initial, queue.map_end(), 0, manhattan_dist<psize>(initial));
    while (!queue.empty()) {
        map_iterator current = queue.top();
        if (current->first == goal) {
            std::vector<permut_type> steps;
            while (current->first != initial) {
                steps.push_back(current->first);
                current = current->second.parent;
            }
            steps.push_back(initial);
            return solution{queue.map_size(), processed, std::move(steps)};
        }
        ++processed;
        queue.pop();
        permut_neighbors_itr_winfo<psize> neighbours(current->first);
        for (auto& n : neighbours) {
            const uint32_t dist_new = current->second.dist_to + 1;
            map_iterator n_map_itr = queue.find(n.first);
            if (n_map_itr == queue.map_end()) {
                queue.push(n.first, current, dist_new, manhattan_dist_winfo<psize>(current->second.dist_h, n.second));
            } else {
                if (dist_new < n_map_itr->second.dist_to) {
                    queue.decrease_key(n_map_itr, current, dist_new);
                }
            }
        }
    }
    return std::nullopt;
}

template <uint32_t psize, typename Heuristic>
std::optional<solution> find_solution_manhattan_wadditional(permut_type initial, Heuristic additional) {
    constexpr auto create_goal = []() -> permut_type {
        if constexpr (psize == 3) {
            return permut_create<psize>({0, 1, 2, 3, 4, 5, 6, 7, 8});
        }
        return permut_create<psize>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    };
    constexpr permut_type goal = create_goal();
    size_t processed = 0;
    puzzle_queue queue;
    queue.push(initial, queue.map_end(), 0, manhattan_dist<psize>(initial) + additional(initial));
    while (!queue.empty()) {
        map_iterator current = queue.top();
        if (current->first == goal) {
            std::vector<permut_type> steps;
            while (current->first != initial) {
                steps.push_back(current->first);
                current = current->second.parent;
            }
            steps.push_back(initial);
            return solution{queue.map_size(), processed, std::move(steps)};
        }
        ++processed;
        queue.pop();
        permut_neighbors_itr_winfo<psize> neighbours(current->first);
        dist_type mdist = manhattan_dist<psize>(current->first);
        for (auto& n : neighbours) {
            const uint32_t dist_new = current->second.dist_to + 1;
            map_iterator n_map_itr = queue.find(n.first);
            if (n_map_itr == queue.map_end()) {
                queue.push(n.first, current, dist_new, manhattan_dist_winfo<psize>(mdist, n.second) + additional(n.first));
            } else {
                if (dist_new < n_map_itr->second.dist_to) {
                    queue.decrease_key(n_map_itr, current, dist_new);
                }
            }
        }
    }
    return std::nullopt;
}

}  // namespace puzzle
