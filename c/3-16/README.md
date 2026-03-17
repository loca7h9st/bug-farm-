# 双向链表与 LRU 缓存实现

本项目包含一个自定义的双向链表实现（list.hpp）和基于该链表实现的 LRU（最近最少使用）缓存算法。

## 目录结构

```
.
├── list.hpp              # 双向链表实现
├── list_stl_style.hpp    # STL 风格的链表实现
├── lru_cache.hpp         # LRU 缓存实现
├── list_test.cpp         # 链表测试程序
├── lru_test.cpp          # LRU 缓存测试程序
├── Makefile              # 编译脚本
└── README.md             # 本文档
```

---

## 一、list.hpp 架构说明

### 1.1 整体设计

list.hpp 实现了一个双向链表，采用**哨兵节点（sentinel node）**设计模式。

### 1.2 核心组件

#### 1.2.1 `__list_node<T>` - 链表节点结构体

```cpp
template <class T>
struct __list_node {
    __list_node<T> *next;  // 指向下一个节点
    __list_node<T> *prev;  // 指向前一个节点
    T data;                 // 存储的数据
    
    __list_node(const T &x = T{}) : next(nullptr), prev(nullptr), data(x) {}
};
```

#### 1.2.2 `__list_iterator<T, Ref, Ptr>` - 迭代器类

支持双向迭代，提供以下操作：
- `operator*` - 解引用访问节点数据
- `operator->` - 指针访问
- `operator++` / `operator++(int)` - 前向/后向递增
- `operator--` / `operator--(int)` - 前向/后向递减
- `operator==` / `operator!=` - 比较

#### 1.2.3 `list<T>` - 链表主类

**核心成员：**
- `node` - 哨兵节点指针（不存储实际数据）
- `_size` - 链表大小

**哨兵节点设计：**
```
node (哨兵) ←→ 节点1 ←→ 节点2 ←→ ... ←→ 节点N ←→ node (哨兵)
```

- `node->next` 指向第一个实际节点
- `node->prev` 指向最后一个实际节点
- `begin()` 返回 `iterator(node->next)`
- `end()` 返回 `iterator(node)`

### 1.3 主要接口

| 方法 | 说明 |
|------|------|
| `push_back(const T& x)` | 在尾部插入元素 |
| `push_front(const T& x)` | 在头部插入元素 |
| `pop_front()` | 删除头部元素 |
| `pop_back()` | 删除尾部元素 |
| `insert(iterator pos, const T& x)` | 在指定位置插入元素 |
| `erase(iterator pos)` | 删除指定位置元素 |
| `front()` / `back()` | 访问首/尾元素 |
| `begin()` / `end()` | 获取迭代器 |
| `size()` | 获取链表大小 |
| `empty()` | 判断是否为空 |
| `clear()` | 清空链表 |
| `reverse()` | 反转链表 |

---

## 二、LRU 缓存算法实现

### 2.1 LRU 算法简介

LRU（Least Recently Used，最近最少使用）是一种常用的缓存淘汰算法。当缓存已满时，它会淘汰最久未被使用的数据。

### 2.2 实现原理

采用**双向链表 + 哈希表**的组合实现：

```
哈希表 (unordered_map)
┌─────────────────────────────────┐
│  Key  │  链表迭代器              │
├─────────────────────────────────┤
│   1   │  → 节点1 (最近使用)      │
│   2   │  → 节点2                  │
│   3   │  → 节点3                  │
│   4   │  → 节点4 (最久使用)      │
└─────────────────────────────────┘
           ↓
双向链表 (list<CacheEntry>)
┌───────┐    ┌───────┐    ┌───────┐    ┌───────┐
│ Key=1 │ ←→ │ Key=2 │ ←→ │ Key=3 │ ←→ │ Key=4 │
│ Val=A │    │ Val=B │    │ Val=C │    │ Val=D │
└───────┘    └───────┘    └───────┘    └───────┘
   头部                          尾部
```

**核心思想：**
1. **双向链表**：维护缓存条目的使用顺序
   - 头部 = 最近使用（Most Recently Used）
   - 尾部 = 最久未使用（Least Recently Used）
   
2. **哈希表**：提供 O(1) 时间复杂度的查找
   - Key → 链表迭代器的映射

### 2.3 核心操作

#### 2.3.1 get(key) - 获取缓存

```
步骤：
1. 在哈希表中查找 key
   ├─ 未找到 → 返回 false
   └─ 找到 → 继续
2. 通过迭代器获取 value
3. 将该节点从链表中删除
4. 将该节点重新插入到链表头部
5. 更新哈希表中的迭代器
6. 返回 true 和 value
```

**时间复杂度：O(1)**

#### 2.3.2 put(key, value) - 插入/更新缓存

```
步骤：
1. 在哈希表中查找 key
   ├─ 已找到 → 更新值并移到头部
   └─ 未找到 → 继续
2. 检查缓存是否已满
   ├─ 已满 → 删除链表尾部节点（最久未使用）
   │          同时从哈希表中删除对应 key
   └─ 未满 → 继续
3. 创建新节点插入到链表头部
4. 在哈希表中添加 key → 头部迭代器的映射
```

**时间复杂度：O(1)**

### 2.4 LRUCache 类接口

```cpp
template <typename Key, typename Value>
class LRUCache {
public:
    LRUCache(size_t capacity);           // 构造函数，指定容量
    
    bool get(const Key& key, Value& value);  // 获取缓存
    void put(const Key& key, const Value& value);  // 插入/更新缓存
    bool remove(const Key& key);         // 删除缓存
    void clear();                         // 清空缓存
    
    size_t size() const;                  // 当前缓存大小
    size_t get_capacity() const;          // 缓存容量
    bool empty() const;                   // 是否为空
    
    void print() const;                   // 打印缓存内容（调试用）
};
```

---

## 三、编译与运行

### 3.1 编译所有程序

```bash
make all
```

### 3.2 运行链表测试

```bash
make run_list
```

或直接运行：

```bash
./list_test
```

### 3.3 运行 LRU 缓存测试

```bash
make run_lru
```

或直接运行：

```bash
./lru_test
```

### 3.4 清理编译文件

```bash
make clean
```

---

## 四、使用示例

### 4.1 使用 list.hpp

```cpp
#include "list.hpp"
#include <iostream>

int main() {
    list<int> lst;
    
    // 插入元素
    lst.push_back(10);
    lst.push_back(20);
    lst.push_front(5);
    
    // 遍历
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    // 输出: 5 10 20
    
    // 访问首尾元素
    std::cout << lst.front() << std::endl;  // 5
    std::cout << lst.back() << std::endl;   // 20
    
    // 删除元素
    lst.pop_front();
    lst.pop_back();
    
    return 0;
}
```

### 4.2 使用 LRUCache

```cpp
#include "lru_cache.hpp"
#include <iostream>
#include <string>

int main() {
    // 创建容量为 3 的 LRU 缓存
    LRUCache<int, std::string> cache(3);
    
    // 插入数据
    cache.put(1, "One");
    cache.put(2, "Two");
    cache.put(3, "Three");
    
    // 缓存状态: [3:Three, 2:Two, 1:One] (头部→尾部)
    
    // 获取数据（会更新使用顺序）
    std::string value;
    if (cache.get(1, value)) {
        std::cout << value << std::endl;  // "One"
    }
    
    // 缓存状态: [1:One, 3:Three, 2:Two]
    
    // 插入新数据（会触发淘汰）
    cache.put(4, "Four");
    
    // 缓存状态: [4:Four, 1:One, 3:Three]
    // Key=2 被淘汰
    
    return 0;
}
```

---

## 五、复杂度分析

| 操作 | list.hpp | LRUCache |
|------|----------|----------|
| 插入（头部/尾部） | O(1) | O(1) |
| 删除（头部/尾部） | O(1) | O(1) |
| 插入（任意位置） | O(1)* | O(1) |
| 删除（任意位置） | O(1)* | O(1) |
| 查找 | O(n) | O(1) |
| 访问首尾 | O(1) | O(1) |

\* 需要先通过迭代器定位到目标位置

---

## 六、技术要点

### 6.1 哨兵节点的优势

1. **简化边界条件**：无需处理 head/tail 为空的情况
2. **统一操作**：首尾节点的插入/删除操作与中间节点一致
3. **代码简洁**：减少大量的 if-else 判断

### 6.2 LRU 的 O(1) 复杂度保证

1. **哈希表**：提供 O(1) 的查找
2. **双向链表**：提供 O(1) 的节点删除和插入（已知节点位置时）

---

## 七、扩展阅读

- STL `std::list` 源码分析
- LRU 的其他实现方式（如时间戳法）
- LFU（最不经常使用）缓存算法
- Redis 中的 LRU 实现
