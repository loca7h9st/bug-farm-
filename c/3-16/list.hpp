#ifndef _HXH_STL_LIST_H
#define _HXH_STL_LIST_H

#include <cstddef>
#include <utility>
template <class T>
struct __list_node
{
    __list_node<T> *next;
    __list_node<T> *prev;
    T data;

    __list_node(const T &x = T{}) : next(nullptr), prev(nullptr), data(x) {}
};

template <class T, class Ref, class Ptr>
struct __list_iterator
{
    typedef __list_iterator<T, T &, T *> iterator;
    typedef __list_iterator<T, const T &, const T *> const_iterator;
    typedef __list_iterator<T, Ref, Ptr> self;
    typedef __list_node<T> *link_type;
    link_type node;

    __list_iterator(link_type x) : node(x) {}
    __list_iterator() {}
    __list_iterator(const iterator &x) : node(x.node) {}
    self& operator=(const self& x) {
        node = x.node;
        return *this;
    }

    bool operator==(const self &x) const { return node == x.node; }
    bool operator!=(const self &x) const { return node != x.node; }
    Ref operator*() const { return (*node).data; }
    Ptr operator->() const { return &(operator*()); }

    self &operator++()
    {
        node = node->next;
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self &operator--()
    {
        node = node->prev;
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template <class T>
class list
{
public:
    typedef __list_node<T> list_node;
    typedef list_node *link_type;

    typedef __list_iterator<T, T &, T *> iterator;
    typedef __list_iterator<T, const T &, const T *> const_iterator;

    typedef T &reference;
    typedef T *pointer;

    typedef const T &const_reference;
    typedef const T *const_pointer;

protected:
    link_type node;
    size_t _size;

public:
    list() : _size(0) { empty_initialize(); }
    list(const list &other)
    {
        // 创建一个哨兵节点
        empty_initialize();
        _size = 0;
        for (auto it = other.begin(); it != other.end(); ++it)
        {
            push_back(*it);
        }
    }

    iterator begin() { return iterator(node->next); }
    iterator end() { return iterator(node); }

    const_iterator begin() const { return iterator(node->next); }
    const_iterator end() const { return iterator(node); }

    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }

    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }
    size_t size() const { return _size; }
    bool empty() const { return _size == 0; }

    iterator insert(iterator position, const T &x)
    {
        link_type tmp = new list_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (link_type(position.node->prev))->next = tmp;
        position.node->prev = tmp;
        ++_size;
        return tmp;
    }

    void reverse()
    {
        if (_size <= 1)
            return;

        link_type cur = node;

        do
        {
            std::swap(cur->next, cur->prev);
            cur = cur->prev;
        } while (cur != node);
    }

    void push_front(const T &x) { insert(begin(), x); }
    void push_back(const T &x) { insert(end(), x); }

    iterator erase(iterator position)
    {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        next_node->prev = prev_node;
        prev_node->next = next_node;
        destory_node(position.node);
        --_size;
        return iterator(next_node);
    }

    void destory_node(link_type p)
    {
        delete p;
    }
    void pop_front() { erase(begin()); }
    void pop_back()
    {
        iterator tmp = end();
        erase(--tmp);
    }

    void clear()
    {
        link_type cur = node->next;
        while (cur != node)
        {
            link_type tmp = cur;
            cur = (link_type)cur->next;
            destory_node(tmp);
        }
        node->next = node;
        node->prev = node;
        _size = 0;
    }
    ~list()
    {
        clear();
        delete node;
    }

private:
    void empty_initialize()
    {
        node = new list_node(T());
        node->next = node;
        node->prev = node;
    }
};

#endif // _HXH_STL_LIST_H