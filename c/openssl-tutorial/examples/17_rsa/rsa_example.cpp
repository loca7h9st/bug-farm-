/**
 * 第17章：RSA - 示例代码
 *
 * 编译: g++ -std=c++11 -o rsa_example rsa_example.cpp -lssl -lcrypto
 */

#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/pem.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// ============================================
// RSA封装类
// ============================================
class RSAWrapper {
private:
    RSA* rsa_;

public:
    RSAWrapper(int bits = 2048) {
        rsa_ = RSA_new();
        if (!rsa_) throw std::runtime_error("RSA_new failed");

        BIGNUM* e = BN_new();
        BN_set_word(e, RSA_F4);  // e = 65537

        if (!RSA_generate_key_ex(rsa_, bits, e, nullptr)) {
            BN_free(e);
            RSA_free(rsa_);
            throw std::runtime_error("RSA_generate_key_ex failed");
        }
        BN_free(e);
    }

    ~RSAWrapper() {
        if (rsa_) RSA_free(rsa_);
    }

    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data) {
        std::vector<unsigned char> result(RSA_size(rsa_));
        int len = RSA_public_encrypt(
            data.size(), data.data(), result.data(),
            rsa_, RSA_PKCS1_OAEP_PADDING
        );
        if (len == -1) throw std::runtime_error("RSA_public_encrypt failed");
        result.resize(len);
        return result;
    }

    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data) {
        std::vector<unsigned char> result(RSA_size(rsa_));
        int len = RSA_private_decrypt(
            data.size(), data.data(), result.data(),
            rsa_, RSA_PKCS1_OAEP_PADDING
        );
        if (len == -1) throw std::runtime_error("RSA_private_decrypt failed");
        result.resize(len);
        return result;
    }

    int bits() const { return RSA_bits(rsa_); }

    RSA* get() { return rsa_; }
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
    std::cout << "  第17章：RSA - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // 生成RSA密钥对
        std::cout << "\n[1] 生成RSA-2048密钥对..." << std::endl;
        RSAWrapper rsa(2048);
        std::cout << "[2] 密钥生成完成！位数: " << rsa.bits() << std::endl;

        // 原始数据
        std::string plaintext_str = "Secret message for RSA encryption!";
        std::vector<unsigned char> plaintext(plaintext_str.begin(), plaintext_str.end());
        std::cout << "\n[3] 原始数据: " << plaintext_str << std::endl;

        // 加密
        std::cout << "[4] 开始加密..." << std::endl;
        std::vector<unsigned char> encrypted = rsa.encrypt(plaintext);
        std::cout << "[5] 加密完成！密文大小: " << encrypted.size() << " 字节" << std::endl;
        std::cout << "[6] 密文: " << to_hex(encrypted) << std::endl;

        // 解密
        std::cout << "\n[7] 开始解密..." << std::endl;
        std::vector<unsigned char> decrypted = rsa.decrypt(encrypted);
        std::string decrypted_str(decrypted.begin(), decrypted.end());
        std::cout << "[8] 解密完成！数据: " << decrypted_str << std::endl;

        // 验证
        if (decrypted_str == plaintext_str) {
            std::cout << "\n✅ RSA测试成功！" << std::endl;
        } else {
            std::cout << "\n❌ RSA测试失败！" << std::endl;
            return 1;
        }

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
