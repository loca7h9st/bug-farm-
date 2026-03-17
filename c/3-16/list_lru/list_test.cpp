#include "list.hpp"
#include <iostream>
#include <string>

void print_list(const list<int>& lst, const std::string& name = "链表") {
    std::cout << name << ": [";
    auto it = lst.begin();
    while (it != lst.end()) {
        std::cout << *it;
        auto next = it;
        ++next;
        if (next != lst.end()) std::cout << ", ";
        ++it;
    }
    std::cout << "]" << std::endl;
}

void test_push_operations() {
    std::cout << "\n=== 测试 push 操作 ===" << std::endl;
    
    list<int> lst;
    std::cout << "初始状态 - 大小: " << lst.size() << std::endl;
    
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    print_list(lst, "push_back 后");
    
    lst.push_front(5);
    lst.push_front(1);
    print_list(lst, "push_front 后");
    
    std::cout << "首元素: " << lst.front() << std::endl;
    std::cout << "尾元素: " << lst.back() << std::endl;
    std::cout << "大小: " << lst.size() << std::endl;
}

void test_pop_operations() {
    std::cout << "\n=== 测试 pop 操作 ===" << std::endl;
    
    list<int> lst;
    for (int i = 1; i <= 5; ++i) {
        lst.push_back(i * 10);
    }
    print_list(lst, "初始");
    
    lst.pop_front();
    print_list(lst, "pop_front 后");
    
    lst.pop_back();
    print_list(lst, "pop_back 后");
    
    std::cout << "当前大小: " << lst.size() << std::endl;
}

void test_insert_erase() {
    std::cout << "\n=== 测试 insert 和 erase ===" << std::endl;
    
    list<int> lst;
    lst.push_back(1);
    lst.push_back(3);
    lst.push_back(4);
    print_list(lst, "初始");
    
    auto it = lst.begin();
    ++it;
    lst.insert(it, 2);
    print_list(lst, "在位置 2 插入 2 后");
    
    it = lst.begin();
    ++it;
    ++it;
    lst.erase(it);
    print_list(lst, "删除位置 3 的元素后");
}

void test_reverse() {
    std::cout << "\n=== 测试 reverse ===" << std::endl;
    
    list<int> lst;
    for (int i = 1; i <= 7; ++i) {
        lst.push_back(i);
    }
    print_list(lst, "原始");
    
    lst.reverse();
    print_list(lst, "反转后");
    
    lst.reverse();
    print_list(lst, "再次反转后");
}

void test_copy_constructor() {
    std::cout << "\n=== 测试拷贝构造函数 ===" << std::endl;
    
    list<std::string> lst1;
    lst1.push_back("Hello");
    lst1.push_back("World");
    lst1.push_back("from");
    lst1.push_back("C++");
    
    std::cout << "原始链表: [";
    for (auto it = lst1.begin(); it != lst1.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
    
    list<std::string> lst2(lst1);
    std::cout << "拷贝后链表: [";
    for (auto it = lst2.begin(); it != lst2.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
    
    lst2.push_back("Copy");
    std::cout << "修改拷贝后: [";
    for (auto it = lst2.begin(); it != lst2.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
    
    std::cout << "原始链表(未改变): [";
    for (auto it = lst1.begin(); it != lst1.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
}

void test_clear() {
    std::cout << "\n=== 测试 clear ===" << std::endl;
    
    list<double> lst;
    lst.push_back(1.1);
    lst.push_back(2.2);
    lst.push_back(3.3);
    lst.push_back(4.4);
    
    std::cout << "清空前 - 大小: " << lst.size() << ", 内容: [";
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
    
    lst.clear();
    std::cout << "清空后 - 大小: " << lst.size() << std::endl;
}

void test_iterator() {
    std::cout << "\n=== 测试迭代器 ===" << std::endl;
    
    list<int> lst;
    for (int i = 1; i <= 6; ++i) {
        lst.push_back(i * 10);
    }
    
    std::cout << "正向遍历: ";
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    std::cout << "反向遍历: ";
    auto it = lst.end();
    --it;
    while (true) {
        std::cout << *it << " ";
        if (it == lst.begin()) break;
        --it;
    }
    std::cout << std::endl;
    
    std::cout << "双向遍历测试: ";
    it = lst.begin();
    ++it;
    ++it;
    std::cout << "位置 3: " << *it << ", ";
    --it;
    std::cout << "位置 2: " << *it << ", ";
    ++it;
    ++it;
    ++it;
    std::cout << "位置 4: " << *it << std::endl;
}

void test_edge_cases() {
    std::cout << "\n=== 测试边界情况 ===" << std::endl;
    
    list<int> empty_list;
    std::cout << "空链表大小: " << empty_list.size() << std::endl;
    
    list<int> single_elem;
    single_elem.push_back(42);
    std::cout << "单元素链表 - front: " << single_elem.front() 
              << ", back: " << single_elem.back() 
              << ", size: " << single_elem.size() << std::endl;
    
    single_elem.pop_front();
    std::cout << "pop_front 后大小: " << single_elem.size() << std::endl;
    
    list<int> two_elems;
    two_elems.push_back(1);
    two_elems.push_back(2);
    two_elems.reverse();
    std::cout << "两个元素反转后: [";
    for (auto it = two_elems.begin(); it != two_elems.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "]" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "       双向链表完整测试 Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    
    test_push_operations();
    test_pop_operations();
    test_insert_erase();
    test_reverse();
    test_copy_constructor();
    test_clear();
    test_iterator();
    test_edge_cases();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "       所有测试完成！✓" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
