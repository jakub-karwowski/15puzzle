#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <variant>
#include <vector>

#include "15puzzle.h"

namespace puzzle {
struct queue_entry {
    permut_type permut;
    uint32_t key;
};
struct map_entry {
    permut_type parent;
    uint32_t queue_index;
    uint32_t dist_to;
};

constexpr bool comp(queue_entry lhs, queue_entry rhs) {
    return lhs.key < rhs.key;
}

class puzzle_queue {
    std::vector<queue_entry> permut_queue;
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
            std::swap(permut_map[root].queue_index, permut_map[top].queue_index);
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
    void push(permut_type entry, permut_type parent, uint32_t dist_to, uint32_t dist_from) {
        permut_queue.push_back(queue_entry{entry, dist_to + dist_from});
        size_t i = permut_queue.size() - 1;
        map_entry* current_m_entry = &(permut_map[entry] = map_entry{parent, static_cast<uint32_t>(i), dist_to});
        while (i > 0 && comp(permut_queue[i], permut_queue[puzzle_queue::parent(i)])) {
            map_entry* parent_m_entry = &permut_map[permut_queue[puzzle_queue::parent(i)].permut];
            std::swap(current_m_entry->queue_index, parent_m_entry->queue_index);
            std::swap(permut_queue[i], permut_queue[puzzle_queue::parent(i)]);
            i = puzzle_queue::parent(i);
            current_m_entry = parent_m_entry;
        }
    }
    const queue_entry& top() const {
        return permut_queue[0];
    }
    void pop() {
        permut_queue[0] = permut_queue.back();
        permut_queue.pop_back();
        heapify(0);
    }
    bool empty() const {
        return permut_queue.empty();
    }
    void decrease_key(map_entry& current_m_entry, permut_type parent_new, uint32_t dist_to_new) {
        map_entry* current_m_entry_ptr = &current_m_entry;
        size_t i = current_m_entry_ptr->queue_index;
        permut_queue[i].key -= (current_m_entry_ptr->dist_to - dist_to_new);
        current_m_entry_ptr->dist_to = dist_to_new;
        current_m_entry_ptr->parent = parent_new;
        while (i > 0 && comp(permut_queue[i], permut_queue[puzzle_queue::parent(i)])) {
            map_entry* parent_m_entry = &permut_map[permut_queue[puzzle_queue::parent(i)].permut];
            std::swap(current_m_entry_ptr->queue_index, parent_m_entry->queue_index);
            std::swap(permut_queue[i], permut_queue[puzzle_queue::parent(i)]);
            i = puzzle_queue::parent(i);
            current_m_entry_ptr = parent_m_entry;
        }
    }
    void decrease_key(permut_type entry, permut_type parent_new, uint32_t dist_to_new) {
        auto& current_m_entry = permut_map[entry];
        decrease_key(current_m_entry, parent_new, dist_to_new);
    }
    map_entry* find_in_map(permut_type element) {
        auto res = permut_map.find(element);
        if (res != permut_map.end()) {
            return &res->second;
        }
        return nullptr;
    }
};

void get_solution_steps(puzzle_queue& queue, permut_type initial, permut_type goal) {
    std::vector<permut_type> steps;
    while (goal != initial) {
        steps.push_back(goal);
        goal = queue.find_in_map(goal)->parent;
    }
    std::cout << steps.size() << "\n\n";
    std::cout << permut_to_string(initial) << "\n\n";
    auto it = steps.rbegin();
    while (it != steps.rend()) {
        std::cout << permut_to_string(*it) << "\n\n";
        ++it;
    }
}

template <uint32_t psize = PUZZLE_SIZE, typename Heuristic>
auto find_solution(permut_type initial, Heuristic heuristic_dist) {
    constexpr auto create_goal = []() -> permut_type {
        if constexpr (psize == 3) {
            return permut_create({0, 1, 2, 3, 4, 5, 6, 7, 8});
        }
        return permut_create({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    };
    constexpr permut_type goal = create_goal();
    puzzle_queue queue;
    queue.push(initial, 0, 0, heuristic_dist(initial));
    while (!queue.empty()) {
        auto current = queue.top();
        if (current.permut == goal) {
            get_solution_steps(queue, initial, goal);
            return true;
        }
        queue.pop();
        auto current_map_entry = *(queue.find_in_map(current.permut));
        permut_neighbors_itr<psize> neigbours(current.permut);
        for (auto n : neigbours) {
            uint32_t dist_new = current_map_entry.dist_to + 1;
            map_entry* neigbour_map_entry = queue.find_in_map(n);
            if (neigbour_map_entry != nullptr) {
                if (dist_new < neigbour_map_entry->dist_to) {
                    queue.decrease_key(*neigbour_map_entry, current.permut, dist_new);
                }
            } else {
                queue.push(n, current.permut, dist_new, heuristic_dist(n));
            }
        }
    }
    return false;
}

}  // namespace puzzle
