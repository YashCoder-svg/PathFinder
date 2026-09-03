#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <cassert>
#include <utility>

template <typename Key, typename Value, typename Comparator = std::less<Key>>
class BinaryHeap {
public:

    struct Element {
        Key   key;
        Value value;
    };

    explicit BinaryHeap(Comparator cmp = Comparator{}) : cmp_(cmp) {}

    [[nodiscard]] bool isEmpty() const { return data_.empty(); }
    [[nodiscard]] std::size_t size()  const { return data_.size(); }

    [[nodiscard]] const Element& top() const {
        if (data_.empty()) throw std::out_of_range("BinaryHeap::top() on empty heap");
        return data_[0];
    }

    void push(Key key, Value value) {
        std::size_t pos = data_.size();
        data_.push_back({key, value});
        pos_[value] = pos;
        siftUp(pos);
    }

    Element pop() {
        if (data_.empty()) throw std::out_of_range("BinaryHeap::pop() on empty heap");

        Element minElem = data_[0];

        pos_.erase(data_[0].value);

        if (data_.size() == 1) {
            data_.pop_back();
            return minElem;
        }

        data_[0] = data_.back();
        pos_[data_[0].value] = 0;
        data_.pop_back();

        siftDown(0);
        return minElem;
    }

    void decreaseKey(Value value, Key newKey) {
        auto it = pos_.find(value);
        if (it == pos_.end()) {
            throw std::invalid_argument("BinaryHeap::decreaseKey — value not in heap");
        }
        std::size_t pos = it->second;

        if (cmp_(newKey, data_[pos].key)) {
            data_[pos].key = newKey;
            siftUp(pos);
        }
    }

    [[nodiscard]] bool contains(const Value& value) const {
        return pos_.count(value) > 0;
    }

    [[nodiscard]] Key keyOf(const Value& value) const {
        auto it = pos_.find(value);
        if (it == pos_.end()) throw std::invalid_argument("BinaryHeap::keyOf — value not found");
        return data_[it->second].key;
    }

    void clear() {
        data_.clear();
        pos_.clear();
    }

private:

    static std::size_t parent(std::size_t i)    { return (i - 1) / 2; }
    static std::size_t leftChild(std::size_t i)  { return 2 * i + 1; }
    static std::size_t rightChild(std::size_t i) { return 2 * i + 2; }

    void siftUp(std::size_t i) {
        while (i > 0) {
            std::size_t p = parent(i);

            if (cmp_(data_[i].key, data_[p].key)) {
                swapAt(i, p);
                i = p;
            } else {
                break;
            }
        }
    }

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

            if (smallest == i) break;

            swapAt(i, smallest);
            i = smallest;
        }
    }

    void swapAt(std::size_t a, std::size_t b) {
        pos_[data_[a].value] = b;
        pos_[data_[b].value] = a;
        std::swap(data_[a], data_[b]);
    }

    std::vector<Element> data_;

    std::unordered_map<Value, std::size_t> pos_;

    Comparator cmp_;
};
