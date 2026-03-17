#ifndef _STL_STYLE_LIST_H
#define _STL_STYLE_LIST_H

#include <cstddef>

// ==========================================
// 节点结构体（参考SGI STL风格）
// ==========================================
template <class T>
struct __list_node
{
    typedef void* void_pointer;
    void_pointer next;
    void_pointer prev;
    T data;

    // 构造函数
    __list_node(const T& x) : data(x), next(nullptr), prev(nullptr) {}
};

// ==========================================
// 迭代器（参考SGI STL风格：template<T, Ref, Ptr>
// ==========================================
template <class T, class Ref, class Ptr>
struct __list_iterator
{
    // 类型定义
    typedef __list_iterator<T, T&, T*>             iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    typedef __list_iterator<T, Ref, Ptr>           self;

    typedef __list_node<T>* link_type;

    // 成员变量
    link_type node;

    // 构造函数
    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator& x) : node(x.node) {}

    // 比较操作符
    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }

    // 解引用
    Ref operator*() const { return (*node).data; }
    Ptr operator->() const { return &(operator*()); }

    // ======================================
    // 前置 ++  (++it)
    // ======================================
    self& operator++()
    {
        node = node->next;
        return *this;
    }

    // ======================================
    // 后置 ++  (it++)  ← 注意有个 int 参数！
    // ======================================
    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    // ======================================
    // 前置 --  (--it)
    // ======================================
    self& operator--()
    {
        node = node->prev;
        return *this;
    }

    // ======================================
    // 后置 --  (it--)  ← 注意有个 int 参数！
    // ======================================
    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

// ==========================================
// 链表类（参考SGI STL风格）
// ==========================================
template <class T>
class list
{
public:
    // 类型定义
    typedef __list_node<T> list_node;
    typedef list_node* link_type;
    typedef __list_iterator<T, T&, T*> iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef size_t size_type;

protected:
    link_type node;    // 哨兵节点
    size_type _size;   // 元素个数

public:
    // ======================================
    // 构造函数
    // ======================================
    list() : _size(0)
    {
        empty_initialize();
    }

    // ======================================
    // 析构函数
    // ======================================
    ~list()
    {
        clear();
        delete node;
    }

    // ======================================
    // 迭代器
    // ======================================
    iterator begin() { return (link_type)((*node).next); }
    iterator end()   { return node; }

    const_iterator begin() const { return (link_type)((*node).next); }
    const_iterator end()   const { return node; }

    // ======================================
    // 访问元素
    // ======================================
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }

    reference back()  { return *(--end()); }
    const_reference back() const { return *(--end()); }

    size_type size() const { return _size; }
    bool empty() const { return _size == 0; }

    // ======================================
    // 插入元素
    // ======================================
    iterator insert(iterator position, const T& x)
    {
        link_type tmp = create_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
        position.node->prev = tmp;
        ++_size;
        return iterator(tmp);
    }

    // ======================================
    // 头尾插入
    // ======================================
    void push_front(const T& x) { insert(begin(), x); }
    void push_back(const T& x)  { insert(end(), x); }

    // ======================================
    // 删除元素
    // ======================================
    iterator erase(iterator position)
    {
        link_type next_node = (link_type)(position.node->next);
        link_type prev_node = (link_type)(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        --_size;
        return iterator(next_node);
    }

    // ======================================
    // 头尾删除
    // ======================================
    void pop_front() { erase(begin()); }
    void pop_back()
    {
        iterator tmp = end();
        erase(--tmp);
    }

    // ======================================
    // 清空
    // ======================================
    void clear()
    {
        link_type cur = (link_type)node->next;
        while (cur != node)
        {
            link_type tmp = cur;
            cur = (link_type)cur->next;
            destroy_node(tmp);
        }
        node->next = node;
        node->prev = node;
        _size = 0;
    }

protected:
    // ======================================
    // 创建节点
    // ======================================
    link_type create_node(const T& x)
    {
        link_type p = new list_node(x);
        return p;
    }

    // ======================================
    // 销毁节点
    // ======================================
    void destroy_node(link_type p)
    {
        delete p;
    }

    // ======================================
    // 空初始化
    // ======================================
    void empty_initialize()
    {
        node = new list_node(T());
        node->next = node;
        node->prev = node;
    }
};

#endif // _STL_STYLE_LIST_H
