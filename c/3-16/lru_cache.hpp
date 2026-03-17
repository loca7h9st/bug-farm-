#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include "list.hpp"
#include <unordered_map>
#include <utility>
#include <iostream>

template <typename Key, typename Value>
class LRUCache {
private:
    struct CacheEntry {
        Key key;
        Value value;
        CacheEntry() : key(), value() {}
        CacheEntry(const Key& k, const Value& v) : key(k), value(v) {}
    };

    typedef typename list<CacheEntry>::iterator ListIterator;
    typedef typename std::unordered_map<Key, ListIterator>::iterator MapIterator;

    list<CacheEntry> cache_list;  // 双向链表存储缓存条目
    std::unordered_map<Key, ListIterator> cache_map;  // 哈希表快速查找
    size_t capacity;  // 缓存容量

public:
    LRUCache(size_t cap) : capacity(cap) {}

    // 获取缓存值
    bool get(const Key& key, Value& value) {
        auto map_it = cache_map.find(key);
        if (map_it == cache_map.end()) {
            return false;  // 未找到
        }

        // 找到后，将该条目移到链表头部（表示最近使用）
        ListIterator list_it = map_it->second;
        CacheEntry entry = *list_it;
        value = entry.value;

        // 从原位置删除
        cache_list.erase(list_it);
        // 插入到头部
        cache_list.push_front(entry);
        // 更新哈希表中的迭代器
        cache_map[key] = cache_list.begin();

        return true;
    }

    // 插入或更新缓存
    void put(const Key& key, const Value& value) {
        // 先检查是否已存在
        auto map_it = cache_map.find(key);
        if (map_it != cache_map.end()) {
            // 已存在，更新值并移到头部
            ListIterator list_it = map_it->second;
            cache_list.erase(list_it);
            cache_list.push_front(CacheEntry(key, value));
            cache_map[key] = cache_list.begin();
            return;
        }

        // 不存在，检查是否已满
        if (cache_list.size() >= capacity) {
            // 已满，删除链表尾部的条目（最少使用）
            Key oldest_key = cache_list.back().key;
            cache_map.erase(oldest_key);
            cache_list.pop_back();
        }

        // 插入新条目到头部
        cache_list.push_front(CacheEntry(key, value));
        cache_map[key] = cache_list.begin();
    }

    // 删除缓存条目
    bool remove(const Key& key) {
        auto map_it = cache_map.find(key);
        if (map_it == cache_map.end()) {
            return false;
        }

        cache_list.erase(map_it->second);
        cache_map.erase(map_it);
        return true;
    }

    // 清空缓存
    void clear() {
        cache_list.clear();
        cache_map.clear();
    }

    // 获取当前缓存大小
    size_t size() const {
        return cache_list.size();
    }

    // 获取缓存容量
    size_t get_capacity() const {
        return capacity;
    }

    // 检查缓存是否为空
    bool empty() const {
        return cache_list.empty();
    }

    // 打印缓存内容（用于调试）
    void print() const {
        std::cout << "LRU Cache (capacity: " << capacity << ", size: " << cache_list.size() << ")" << std::endl;
        std::cout << "Cache order (most recent -> least recent):" << std::endl;
        
        auto it = cache_list.begin();
        int index = 0;
        while (it != cache_list.end()) {
            std::cout << "  [" << index << "] Key: " << (*it).key << ", Value: " << (*it).value << std::endl;
            ++it;
            ++index;
        }
        std::cout << std::endl;
    }
};

#endif
