#pragma once

// =============================================================================
// BinaryHeap.hpp — Generic Min-Heap with Decrease-Key
// =============================================================================
// WHY A CUSTOM HEAP?
//   std::priority_queue is a max-heap with no decrease-key operation.  We need
//   decrease-key in Dijkstra / A*: when we find a shorter path to a node that
//   is already in the frontier, we must lower its priority rather than insert
//   a duplicate.  Without decrease-key we either:
//     (a) insert duplicates and filter stale pops — simple but uses O(E) space,
//     (b) implement our own heap with a position map — O(log V) decrease-key.
//   We implement (b) here for portfolio correctness.
//
// HEAP PROPERTY (min-heap invariant):
//   For every non-root node at position i,
//       heap[parent(i)].key  ≤  heap[i].key
//   This means heap[0] is always the element with the *smallest* key.
//
// COMPLEXITY SUMMARY:
//   push          O(log n)   — sift-up from the bottom
//   pop           O(log n)   — remove root, sift-down
//   decrease-key  O(log n)   — sift-up from current position
//   top / isEmpty O(1)
//
// TEMPLATE DESIGN:
//   Key   — the priority type (e.g. double for distance)
//   Value — the payload stored alongside the key (e.g. NodeId)
//   Comparator — defaults to std::less<Key> (min-heap).
//                Swap to std::greater<Key> for a max-heap.
// =============================================================================

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <cassert>
#include <utility>   // std::pair, std::swap

template <typename Key, typename Value, typename Comparator = std::less<Key>>
class BinaryHeap {
public:
    // -------------------------------------------------------------------------
    // Element — one slot in the heap array.
    // -------------------------------------------------------------------------
    struct Element {
        Key   key;    // Priority (e.g. tentative distance)
        Value value;  // Payload (e.g. NodeId)
    };

    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------
    explicit BinaryHeap(Comparator cmp = Comparator{}) : cmp_(cmp) {}

    // -------------------------------------------------------------------------
    // isEmpty / size
    // -------------------------------------------------------------------------
    [[nodiscard]] bool isEmpty() const { return data_.empty(); }
    [[nodiscard]] std::size_t size()  const { return data_.size(); }

    // -------------------------------------------------------------------------
    // top() — peek at the minimum element without removing it.  O(1).
    // -------------------------------------------------------------------------
    [[nodiscard]] const Element& top() const {
        if (data_.empty()) throw std::out_of_range("BinaryHeap::top() on empty heap");
        return data_[0];
    }

    // -------------------------------------------------------------------------
    // push() — Insert a new (key, value) pair.                       O(log n)
    //
    // Steps:
    //   1. Append to the end of the array (preserves shape property).
    //   2. Record the new element's position in the position map.
    //   3. Sift-up to restore the heap property.
    // -------------------------------------------------------------------------
    void push(Key key, Value value) {
        std::size_t pos = data_.size();
        data_.push_back({key, value});
        pos_[value] = pos;
        siftUp(pos);
    }

    // -------------------------------------------------------------------------
    // pop() — Remove and return the minimum element.                 O(log n)
    //
    // Steps:
    //   1. Swap root (minimum) with the last element.
    //   2. Remove the last element (was the root).
    //   3. Sift-down the new root to restore the heap property.
    //
    // WHY swap-then-remove?  Removing from the middle of an array is O(n).
    // The shape property of a heap only guarantees the array fills left-to-right,
    // so the *last* element is always safe to remove in O(1).
    // -------------------------------------------------------------------------
    Element pop() {
        if (data_.empty()) throw std::out_of_range("BinaryHeap::pop() on empty heap");

        Element minElem = data_[0];          // Save root before overwriting

        pos_.erase(data_[0].value);          // Root leaves the heap

        if (data_.size() == 1) {
            data_.pop_back();
            return minElem;
        }

        // Move last element to root slot
        data_[0] = data_.back();
        pos_[data_[0].value] = 0;
        data_.pop_back();

        siftDown(0);                         // Restore heap property
        return minElem;
    }

    // -------------------------------------------------------------------------
    // decreaseKey() — Update the key of an existing value to newKey. O(log n)
    //
    // Precondition: newKey ≤ current key  (for a min-heap, decreasing priority
    // means the element should move *closer* to the root — sift-up).
    //
    // This is the operation that makes Dijkstra run in O((V+E) log V) instead
    // of O(V²) with a simple array.
    // -------------------------------------------------------------------------
    void decreaseKey(Value value, Key newKey) {
        auto it = pos_.find(value);
        if (it == pos_.end()) {
            throw std::invalid_argument("BinaryHeap::decreaseKey — value not in heap");
        }
        std::size_t pos = it->second;

        // Sanity check: we can only decrease (in a min-heap, smaller key = higher priority)
        assert(!cmp_(data_[pos].key, newKey) &&
               "decreaseKey called with a larger key — use increaseKey instead");

        data_[pos].key = newKey;
        siftUp(pos);     // New key might violate parent's heap property → fix upward
    }

    // -------------------------------------------------------------------------
    // contains() — O(1) lookup via the position map.
    // -------------------------------------------------------------------------
    [[nodiscard]] bool contains(const Value& value) const {
        return pos_.count(value) > 0;
    }

    // -------------------------------------------------------------------------
    // keyOf() — Returns the current key for a value.  O(1).
    // -------------------------------------------------------------------------
    [[nodiscard]] Key keyOf(const Value& value) const {
        auto it = pos_.find(value);
        if (it == pos_.end()) throw std::invalid_argument("BinaryHeap::keyOf — value not found");
        return data_[it->second].key;
    }

    // -------------------------------------------------------------------------
    // clear() — Remove all elements.
    // -------------------------------------------------------------------------
    void clear() {
        data_.clear();
        pos_.clear();
    }

private:
    // =========================================================================
    // Internal helpers
    // =========================================================================

    // Parent / child index arithmetic (0-indexed array).
    // For a node at index i:
    //   parent:        (i - 1) / 2
    //   left child:    2*i + 1
    //   right child:   2*i + 2
    // This layout packs parent and children close together in memory → cache-friendly.
    static std::size_t parent(std::size_t i)    { return (i - 1) / 2; }
    static std::size_t leftChild(std::size_t i)  { return 2 * i + 1; }
    static std::size_t rightChild(std::size_t i) { return 2 * i + 2; }

    // -------------------------------------------------------------------------
    // siftUp — Bubble element at position `i` toward the root until the heap
    // property is satisfied.
    //
    // Invariant maintained: heap[parent(i)].key ≤ heap[i].key
    // If violated, swap with parent and continue upward.
    // -------------------------------------------------------------------------
    void siftUp(std::size_t i) {
        while (i > 0) {
            std::size_t p = parent(i);
            // cmp_(a, b) is true when a has *higher* priority (smaller for min-heap)
            if (cmp_(data_[i].key, data_[p].key)) {
                swapAt(i, p);
                i = p;
            } else {
                break;  // Heap property satisfied
            }
        }
    }

    // -------------------------------------------------------------------------
    // siftDown — Push element at position `i` down the tree until the heap
    // property is satisfied.
    //
    // At each step pick the *smaller* child (in a min-heap) to swap with,
    // otherwise swapping with the larger child could violate its sibling's
    // invariant.
    // -------------------------------------------------------------------------
    void siftDown(std::size_t i) {
        std::size_t n = data_.size();
        while (true) {
            std::size_t smallest = i;
            std::size_t l = leftChild(i);
            std::size_t r = rightChild(i);

            if (l < n && cmp_(data_[l].key, data_[smallest].key))
                smallest = l;
            if (r < n && cmp_(data_[r].key, data_[smallest].key))
                smallest = r;

            if (smallest == i) break;   // Already satisfies heap property

            swapAt(i, smallest);
            i = smallest;
        }
    }

    // -------------------------------------------------------------------------
    // swapAt — Swap two elements and keep the position map in sync.
    // The position map is critical for O(1) lookup in decreaseKey.
    // -------------------------------------------------------------------------
    void swapAt(std::size_t a, std::size_t b) {
        pos_[data_[a].value] = b;
        pos_[data_[b].value] = a;
        std::swap(data_[a], data_[b]);
    }

    // =========================================================================
    // Data members
    // =========================================================================

    // Flat array representation of the binary heap (complete binary tree stored
    // in level-order).  This is more cache-friendly than a pointer-based tree.
    std::vector<Element> data_;

    // Position map: value → index in data_.
    // Enables O(1) lookup of where a given value lives, which is needed by
    // decreaseKey and contains().
    std::unordered_map<Value, std::size_t> pos_;

    Comparator cmp_;   // Comparison functor (defaults to std::less → min-heap)
};
