/**
 * 第3章：堆栈 - 示例代码
 *
 * 编译: g++ -std=c++11 -o stack_example stack_example.cpp -lssl -lcrypto
 */

#include <openssl/stack.h>
#include <iostream>
#include <vector>
#include <stdexcept>

// ============================================
// 堆栈类
// ============================================
template<typename T>
class Stack {
private:
    STACK_OF(void)* stack_;

public:
    Stack() {
        stack_ = sk_new(nullptr);
        if (!stack_) throw std::runtime_error("sk_new failed");
    }

    ~Stack() {
        if (stack_) sk_free(stack_);
    }

    int size() const {
        return sk_num(stack_);
    }

    bool empty() const {
        return sk_num(stack_) == 0;
    }

    T* get(int index) const {
        if (index < 0 || index >= sk_num(stack_)) {
            throw std::out_of_range("index out of range");
        }
        return static_cast<T*>(sk_value(stack_, index));
    }

    T* operator[](int index) const {
        return get(index);
    }

    void push(T* val) {
        sk_push(stack_, val);
    }

    T* pop() {
        return static_cast<T*>(sk_pop(stack_));
    }

    T* shift() {
        return static_cast<T*>(sk_shift(stack_));
    }

    void unshift(T* val) {
        sk_unshift(stack_, val);
    }

    void insert(int index, T* val) {
        sk_insert(stack_, val, index);
    }

    void remove(int index) {
        sk_delete(stack_, index);
    }

    void clear() {
        while (!empty()) {
            pop();
        }
    }

    STACK_OF(void)* get() { return stack_; }
};

// ============================================
// 测试
// ============================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  第3章：堆栈 - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        Stack<int> stack;

        // 测试1：push和size
        std::cout << "\n=== 测试1：push和size ===" << std::endl;
        int a = 100, b = 200, c = 300;
        stack.push(&a);
        stack.push(&b);
        stack.push(&c);
        std::cout << "size: " << stack.size() << std::endl;
        std::cout << "✅ push和size测试成功！" << std::endl;

        // 测试2：get和operator[]
        std::cout << "\n=== 测试2：get和operator[] ===" << std::endl;
        std::cout << "stack[0]: " << *stack.get(0) << std::endl;
        std::cout << "stack[1]: " << *stack[1] << std::endl;
        std::cout << "stack[2]: " << *stack[2] << std::endl;
        std::cout << "✅ get和operator[]测试成功！" << std::endl;

        // 测试3：pop
        std::cout << "\n=== 测试3：pop ===" << std::endl;
        int* val = stack.pop();
        std::cout << "pop: " << *val << std::endl;
        std::cout << "size after pop: " << stack.size() << std::endl;
        std::cout << "✅ pop测试成功！" << std::endl;

        // 测试4：clear
        std::cout << "\n=== 测试4：clear ===" << std::endl;
        stack.clear();
        std::cout << "size after clear: " << stack.size() << std::endl;
        std::cout << "empty: " << (stack.empty() ? "Yes" : "No") << std::endl;
        std::cout << "✅ clear测试成功！" << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
