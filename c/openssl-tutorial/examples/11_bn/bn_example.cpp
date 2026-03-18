/**
 * 第11章：大数 - 示例代码
 *
 * 编译: g++ -std=c++11 -o bn_example bn_example.cpp -lssl -lcrypto
 */

#include <openssl/bn.h>
#include <iostream>
#include <vector>
#include <stdexcept>

// ============================================
// 大数类
// ============================================
class BigNum {
private:
    BIGNUM* bn_;

public:
    BigNum() {
        bn_ = BN_new();
        if (!bn_) throw std::runtime_error("BN_new failed");
    }

    BigNum(unsigned long w) : BigNum() {
        BN_set_word(bn_, w);
    }

    BigNum(const std::vector<unsigned char>& data) : BigNum() {
        BN_bin2bn(data.data(), data.size(), bn_);
    }

    ~BigNum() {
        if (bn_) BN_free(bn_);
    }

    BigNum add(const BigNum& other) const {
        BigNum result;
        if (!BN_add(result.bn_, bn_, other.bn_))
            throw std::runtime_error("BN_add failed");
        return result;
    }

    BigNum sub(const BigNum& other) const {
        BigNum result;
        if (!BN_sub(result.bn_, bn_, other.bn_))
            throw std::runtime_error("BN_sub failed");
        return result;
    }

    BigNum mul(const BigNum& other, BN_CTX* ctx) const {
        BigNum result;
        if (!BN_mul(result.bn_, bn_, other.bn_, ctx))
            throw std::runtime_error("BN_mul failed");
        return result;
    }

    BigNum mod_exp(const BigNum& p, const BigNum& m, BN_CTX* ctx) const {
        BigNum result;
        if (!BN_mod_exp(result.bn_, bn_, p.bn_, m.bn_, ctx))
            throw std::runtime_error("BN_mod_exp failed");
        return result;
    }

    bool is_prime(int checks = BN_prime_checks_for_size(2048)) const {
        BN_CTX* ctx = BN_CTX_new();
        int result = BN_is_prime_ex(bn_, checks, ctx, nullptr);
        BN_CTX_free(ctx);
        return result == 1;
    }

    std::vector<unsigned char> to_bin() const {
        int len = BN_num_bytes(bn_);
        std::vector<unsigned char> result(len);
        BN_bn2bin(bn_, result.data());
        return result;
    }

    std::string to_hex() const {
        char* hex = BN_bn2hex(bn_);
        std::string result(hex);
        OPENSSL_free(hex);
        return result;
    }

    unsigned long to_word() const {
        return BN_get_word(bn_);
    }

    BIGNUM* get() { return bn_; }
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
    std::cout << "  第11章：大数 - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        BN_CTX* ctx = BN_CTX_new();

        // 测试1：基本运算
        std::cout << "\n=== 测试1：基本运算 ===" << std::endl;
        BigNum a(100);
        BigNum b(200);
        std::cout << "a = " << a.to_hex() << std::endl;
        std::cout << "b = " << b.to_hex() << std::endl;

        BigNum c = a.add(b);
        std::cout << "a + b = " << c.to_hex() << " (word: " << c.to_word() << ")" << std::endl;

        BigNum d = b.sub(a);
        std::cout << "b - a = " << d.to_hex() << " (word: " << d.to_word() << ")" << std::endl;

        BigNum e = a.mul(b, ctx);
        std::cout << "a * b = " << e.to_hex() << " (word: " << e.to_word() << ")" << std::endl;

        // 测试2：模幂运算（RSA常用）
        std::cout << "\n=== 测试2：模幂运算 ===" << std::endl;
        BigNum base(5);
        BigNum exp(3);
        BigNum mod(100);
        BigNum result = base.mod_exp(exp, mod, ctx);
        std::cout << "5^3 mod 100 = " << result.to_hex() << " (word: " << result.to_word() << ")" << std::endl;

        // 测试3：素数检测
        std::cout << "\n=== 测试3：素数检测 ===" << std::endl;
        BigNum prime1(997);  // 素数
        BigNum prime2(1000); // 不是素数
        std::cout << "997 is prime? " << (prime1.is_prime() ? "Yes" : "No") << std::endl;
        std::cout << "1000 is prime? " << (prime2.is_prime() ? "Yes" : "No") << std::endl;

        BN_CTX_free(ctx);

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
