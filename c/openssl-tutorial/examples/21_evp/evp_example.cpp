/**
 * 第21章：EVP（超级重点！）- 示例代码
 *
 * 编译: g++ -std=c++11 -o evp_example evp_example.cpp -lssl -lcrypto
 */

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// ============================================
// EVP对称加密类
// ============================================
class EVPCipher {
private:
    const EVP_CIPHER* cipher_;
    std::vector<unsigned char> key_;
    std::vector<unsigned char> iv_;

public:
    EVPCipher(const EVP_CIPHER* cipher) : cipher_(cipher) {
        key_.resize(EVP_CIPHER_key_length(cipher));
        iv_.resize(EVP_CIPHER_iv_length(cipher));

        if (RAND_bytes(key_.data(), key_.size()) != 1)
            throw std::runtime_error("RAND_bytes for key failed");
        if (RAND_bytes(iv_.data(), iv_.size()) != 1)
            throw std::runtime_error("RAND_bytes for IV failed");
    }

    EVPCipher(const EVP_CIPHER* cipher,
              const std::vector<unsigned char>& key,
              const std::vector<unsigned char>& iv)
        : cipher_(cipher), key_(key), iv_(iv) {}

    const std::vector<unsigned char>& get_key() const { return key_; }
    const std::vector<unsigned char>& get_iv() const { return iv_; }

    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& plaintext) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

        if (EVP_EncryptInit_ex(ctx, cipher_, nullptr, key_.data(), iv_.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EVP_EncryptInit_ex failed");
        }

        std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(cipher_) * 2);
        int outl;

        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &outl,
                              plaintext.data(), plaintext.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EVP_EncryptUpdate failed");
        }

        int total_len = outl;
        int finall;
        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + outl, &finall) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EVP_EncryptFinal_ex failed");
        }

        total_len += finall;
        ciphertext.resize(total_len);
        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    }

    std::vector<unsigned char> encrypt(const std::string& plaintext) {
        return encrypt(std::vector<unsigned char>(plaintext.begin(), plaintext.end()));
    }

    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& ciphertext) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

        if (EVP_DecryptInit_ex(ctx, cipher_, nullptr, key_.data(), iv_.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EVP_DecryptInit_ex failed");
        }

        std::vector<unsigned char> plaintext(ciphertext.size() + EVP_CIPHER_block_size(cipher_));
        int outl;

        if (EVP_DecryptUpdate(ctx, plaintext.data(), &outl,
                              ciphertext.data(), ciphertext.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EVP_DecryptUpdate failed");
        }

        int total_len = outl;
        int finall;
        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + outl, &finall) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EVP_DecryptFinal_ex failed");
        }

        total_len += finall;
        plaintext.resize(total_len);
        EVP_CIPHER_CTX_free(ctx);
        return plaintext;
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
    std::cout << "  第21章：EVP（超级重点！）- 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        std::string plaintext = "Hello, EVP AES-256-CBC Super Important Message!";
        std::cout << "\n[1] 原始数据: " << plaintext << std::endl;

        // 测试1：AES-256-CBC
        std::cout << "\n=== 测试1：AES-256-CBC ===" << std::endl;
        EVPCipher cipher256(EVP_aes_256_cbc());
        std::cout << "密钥: " << to_hex(cipher256.get_key()) << std::endl;
        std::cout << "IV:   " << to_hex(cipher256.get_iv()) << std::endl;

        std::vector<unsigned char> ciphertext256 = cipher256.encrypt(plaintext);
        std::cout << "加密完成！密文大小: " << ciphertext256.size() << " 字节" << std::endl;
        std::cout << "密文: " << to_hex(ciphertext256) << std::endl;

        std::vector<unsigned char> decrypted256 = cipher256.decrypt(ciphertext256);
        std::string decrypted_str256(decrypted256.begin(), decrypted256.end());
        std::cout << "解密完成！数据: " << decrypted_str256 << std::endl;

        if (decrypted_str256 == plaintext) {
            std::cout << "✅ AES-256-CBC测试成功！" << std::endl;
        } else {
            std::cout << "❌ AES-256-CBC测试失败！" << std::endl;
            return 1;
        }

        // 测试2：AES-128-CBC
        std::cout << "\n=== 测试2：AES-128-CBC ===" << std::endl;
        EVPCipher cipher128(EVP_aes_128_cbc());
        std::cout << "密钥: " << to_hex(cipher128.get_key()) << std::endl;
        std::cout << "IV:   " << to_hex(cipher128.get_iv()) << std::endl;

        std::vector<unsigned char> ciphertext128 = cipher128.encrypt(plaintext);
        std::cout << "加密完成！密文大小: " << ciphertext128.size() << " 字节" << std::endl;

        std::vector<unsigned char> decrypted128 = cipher128.decrypt(ciphertext128);
        std::string decrypted_str128(decrypted128.begin(), decrypted128.end());
        std::cout << "解密完成！数据: " << decrypted_str128 << std::endl;

        if (decrypted_str128 == plaintext) {
            std::cout << "✅ AES-128-CBC测试成功！" << std::endl;
        } else {
            std::cout << "❌ AES-128-CBC测试失败！" << std::endl;
            return 1;
        }

        // 测试3：混合加密（勒索软件常用方案）
        std::cout << "\n=== 测试3：混合加密（勒索软件方案）===" << std::endl;
        std::cout << "这个测试需要RSA，已在第17章示例中演示" << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
