//
// Created by Anya on 2023/7/12.
//

#ifndef ANYA_STL_LRU_HPP
#define ANYA_STL_LRU_HPP

#include "container/unordered_map.hpp"
#include "container/list.hpp"

namespace anya {

template<class K, class V, class Hash = std::hash<K>>
class lru_cache {
private:
    using value_type = std::pair<K, V>;
    using iterator = typename anya::list<value_type>::iterator;

private:
    anya::list<value_type> queue;                 // 实际存储的队列
    anya::unordered_map<K, iterator, Hash> map;   // 建立key到迭代器的映射
    size_t max_size = 1024;

public:
    lru_cache() = default;

    explicit lru_cache(int sz) : max_size(sz) {}

public:
    void
    push(const K& key, const V& value) {
        auto it = map.find(key);
        // 说明是已经存在的旧数据，需要更新在queue中的位置
        if (it != map.end()) {
            queue.erase(it->second);
        }
        queue.template emplace_front(key, value);
        map[key] = queue.begin();
        shrink_to_fit();
    }

    V
    get_or_default(const K& key, const V& def = {}) {
        auto it = map.find(key);
        if (it != map.end()) {
            queue.push_front(std::move(*(it->second))), queue.erase(it->second);
            map[key] = queue.begin();
            return queue.front().second;
        }
        return def;
    }

    void
    erase(const K& key) {
        auto it = map.find(key);
        if (it == map.end()) return;
        queue.erase(it->second);
        map.erase(it);
    }

    void
    clear() { queue.clear(), map.clear(); }

    bool
    contains(const K& key) { return map.count(key); }

    void
    set_max_size(size_t size) {
        max_size = size;
        shrink_to_fit();
    }

private:
    constexpr void
    shrink_to_fit() {
        while (queue.size() > max_size) {
            map.erase(queue.back().first);
            queue.pop_back();
        }
    }

};

}

#endif //ANYA_STL_LRU_HPP
