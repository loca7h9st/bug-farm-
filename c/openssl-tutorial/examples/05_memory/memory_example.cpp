/**
 * 第5章：内存分配 - 示例代码
 *
 * 编译: g++ -std=c++11 -o memory_example memory_example.cpp -lssl -lcrypto
 */

#include <openssl/crypto.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>

// ============================================
// 内存管理类
// ============================================
class CryptoMemory {
public:
    static void* malloc(size_t size) {
        void* p = CRYPTO_malloc(size, __FILE__, __LINE__);
        if (!p) throw std::bad_alloc();
        return p;
    }

    static void* zalloc(size_t size) {
        void* p = CRYPTO_zalloc(size, __FILE__, __LINE__);
        if (!p) throw std::bad_alloc();
        return p;
    }

    static void* realloc(void* ptr, size_t size) {
        void* p = CRYPTO_realloc(ptr, size, __FILE__, __LINE__);
        if (!p) throw std::bad_alloc();
        return p;
    }

    static void free(void* ptr) {
        CRYPTO_free(ptr, __FILE__, __LINE__);
    }
};

// ============================================
// 测试
// ============================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  第5章：内存分配 - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // 测试1：malloc和free
        std::cout << "\n=== 测试1：malloc和free ===" << std::endl;
        void* p1 = CryptoMemory::malloc(100);
        std::cout << "malloc(100) success!" << std::endl;
        CryptoMemory::free(p1);
        std::cout << "✅ malloc和free测试成功！" << std::endl;

        // 测试2：zalloc（清零）
        std::cout << "\n=== 测试2：zalloc（清零）===" << std::endl;
        char* p2 = static_cast<char*>(CryptoMemory::zalloc(10));
        bool all_zero = true;
        for (int i = 0; i < 10; ++i) {
            if (p2[i] != 0) {
                all_zero = false;
                break;
            }
        }
        std::cout << "zalloc 10 bytes, all zero? " << (all_zero ? "Yes" : "No") << std::endl;
        CryptoMemory::free(p2);
        std::cout << "✅ zalloc测试成功！" << std::endl;

        // 测试3：realloc
        std::cout << "\n=== 测试3：realloc ===" << std::endl;
        char* p3 = static_cast<char*>(CryptoMemory::malloc(10));
        strcpy(p3, "Hello");
        std::cout << "Original: " << p3 << std::endl;

        p3 = static_cast<char*>(CryptoMemory::realloc(p3, 100));
        strcat(p3, ", World!");
        std::cout << "After realloc and append: " << p3 << std::endl;
        CryptoMemory::free(p3);
        std::cout << "✅ realloc测试成功！" << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
