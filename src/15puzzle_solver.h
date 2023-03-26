#pragma once

#include <map>
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
        permut_queue.emplace_back(entry, dist_to + dist_from);
        size_t i = permut_queue.size() - 1;
        auto& current_m_entry = (permut_map[entry] = map_entry{parent, static_cast<uint32_t>(i), dist_to});
        while (i > 0 && comp(permut_queue[i], permut_queue[puzzle_queue::parent(i)])) {
            auto& parent_m_entry = permut_map[puzzle_queue::parent(i)];
            std::swap(current_m_entry.queue_index, parent_m_entry.queue_index);
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
    void decrease_key(permut_type entry, permut_type parent_new, uint32_t dist_to_new, uint32_t dist_from_new) {
        auto& current_m_entry = permut_map[entry];
        size_t i = current_m_entry.queue_index;
        permut_queue[i].key = dist_from_new + dist_to_new;
        current_m_entry.dist_to = dist_to_new;
        current_m_entry.parent = parent_new;
        while (i > 0 && comp(permut_queue[i], permut_queue[puzzle_queue::parent(i)])) {
            auto& parent_m_entry = permut_map[puzzle_queue::parent(i)];
            std::swap(current_m_entry.queue_index, parent_m_entry.queue_index);
            std::swap(permut_queue[i], permut_queue[puzzle_queue::parent(i)]);
            i = puzzle_queue::parent(i);
            current_m_entry = parent_m_entry;
        }
    }
    void update(permut_type current, permut_type neighbor) {
    }
};
}  // namespace puzzle
