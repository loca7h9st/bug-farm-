/**
 * 第9章：随机数 - 示例代码
 *
 * 编译: g++ -std=c++11 -o rand_example rand_example.cpp -lssl -lcrypto
 */

#include <openssl/rand.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

// ============================================
// 随机数类
// ============================================
class Random {
public:
    static bool is_ready() {
        return RAND_status() == 1;
    }

    static std::vector<unsigned char> bytes(size_t num) {
        std::vector<unsigned char> result(num);
        if (RAND_bytes(result.data(), num) != 1) {
            throw std::runtime_error("RAND_bytes failed");
        }
        return result;
    }

    static std::vector<unsigned char> priv_bytes(size_t num) {
        std::vector<unsigned char> result(num);
        if (RAND_priv_bytes(result.data(), num) != 1) {
            throw std::runtime_error("RAND_priv_bytes failed");
        }
        return result;
    }

    static void seed(const std::vector<unsigned char>& data) {
        RAND_seed(data.data(), data.size());
    }

    static void cleanup() {
        RAND_cleanup();
    }

    static int random_int(int min, int max) {
        if (min >= max) throw std::invalid_argument("min must be less than max");
        unsigned int range = static_cast<unsigned int>(max - min);
        unsigned int r;
        if (RAND_bytes(reinterpret_cast<unsigned char*>(&r), sizeof(r)) != 1) {
            throw std::runtime_error("RAND_bytes failed");
        }
        return min + static_cast<int>(r % (range + 1));
    }
};

// ============================================
// 工具函数
// ============================================
std::string to_hex(const std::vector<unsigned char>& data) {
    static const char* hex = "0123456789abcdef";
    std::string result;
    result.reserve(data.size() * 2);
    for (unsigned char c : data) {
        result += hex[(c >> 4) & 0x0F];
        result += hex[c & 0x0F];
    }
    return result;
}

// ============================================
// 测试
// ============================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  第9章：随机数 - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // 测试1：检查随机数生成器状态
        std::cout << "\n=== 测试1：随机数生成器状态 ===" << std::endl;
        std::cout << "RAND_status(): " << (Random::is_ready() ? "Ready" : "Not Ready") << std::endl;
        std::cout << "✅ 状态检查成功！" << std::endl;

        // 测试2：生成随机字节
        std::cout << "\n=== 测试2：生成随机字节 ===" << std::endl;
        std::vector<unsigned char> rand_bytes = Random::bytes(16);
        std::cout << "16 bytes random: " << to_hex(rand_bytes) << std::endl;
        std::cout << "✅ 随机字节生成成功！" << std::endl;

        // 测试3：生成随机整数
        std::cout << "\n=== 测试3：生成随机整数 ===" << std::endl;
        int r1 = Random::random_int(1, 100);
        int r2 = Random::random_int(1, 100);
        int r3 = Random::random_int(1, 100);
        std::cout << "Random 1-100: " << r1 << ", " << r2 << ", " << r3 << std::endl;
        std::cout << "✅ 随机整数生成成功！" << std::endl;

        // 测试4：私钥用随机数
        std::cout << "\n=== 测试4：私钥用随机数 ===" << std::endl;
        std::vector<unsigned char> priv_rand = Random::priv_bytes(32);
        std::cout << "32 bytes private random: " << to_hex(priv_rand).substr(0, 32) << "..." << std::endl;
        std::cout << "✅ 私钥用随机数生成成功！" << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
