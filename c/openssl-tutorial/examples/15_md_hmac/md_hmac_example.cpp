/**
 * 第15章：摘要与HMAC - 示例代码
 *
 * 编译: g++ -std=c++11 -o md_hmac_example md_hmac_example.cpp -lssl -lcrypto
 */

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// ============================================
// 摘要类
// ============================================
class Digest {
private:
    EVP_MD_CTX* ctx_;
    const EVP_MD* md_;

public:
    Digest(const EVP_MD* md = EVP_sha256()) : md_(md) {
        ctx_ = EVP_MD_CTX_new();
        if (!ctx_) throw std::runtime_error("EVP_MD_CTX_new failed");
        if (EVP_DigestInit_ex(ctx_, md_, nullptr) != 1) {
            EVP_MD_CTX_free(ctx_);
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }
    }

    ~Digest() {
        if (ctx_) EVP_MD_CTX_free(ctx_);
    }

    void update(const std::vector<unsigned char>& data) {
        if (EVP_DigestUpdate(ctx_, data.data(), data.size()) != 1) {
            throw std::runtime_error("EVP_DigestUpdate failed");
        }
    }

    void update(const std::string& data) {
        update(std::vector<unsigned char>(data.begin(), data.end()));
    }

    std::vector<unsigned char> final() {
        std::vector<unsigned char> result(EVP_MAX_MD_SIZE);
        unsigned int result_len;
        if (EVP_DigestFinal_ex(ctx_, result.data(), &result_len) != 1) {
            throw std::runtime_error("EVP_DigestFinal_ex failed");
        }
        result.resize(result_len);
        return result;
    }
};

// ============================================
// HMAC类
// ============================================
class HMACUtils {
public:
    static std::vector<unsigned char> sha256(
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& data)
    {
        std::vector<unsigned char> result(EVP_MAX_MD_SIZE);
        unsigned int result_len;
        HMAC(EVP_sha256(),
             key.data(), key.size(),
             data.data(), data.size(),
             result.data(), &result_len);
        result.resize(result_len);
        return result;
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
    std::cout << "  第15章：摘要与HMAC - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        std::string data = "Hello, OpenSSL Digest!";
        std::cout << "\n[1] 原始数据: " << data << std::endl;

        // 测试1：SHA-256
        std::cout << "\n=== 测试1：SHA-256 ===" << std::endl;
        Digest sha256(EVP_sha256());
        sha256.update(data);
        std::vector<unsigned char> hash_sha256 = sha256.final();
        std::cout << "SHA-256: " << to_hex(hash_sha256) << std::endl;

        // 测试2：SHA-1
        std::cout << "\n=== 测试2：SHA-1 ===" << std::endl;
        Digest sha1(EVP_sha1());
        sha1.update(data);
        std::vector<unsigned char> hash_sha1 = sha1.final();
        std::cout << "SHA-1:   " << to_hex(hash_sha1) << std::endl;

        // 测试3：MD5
        std::cout << "\n=== 测试3：MD5 ===" << std::endl;
        Digest md5(EVP_md5());
        md5.update(data);
        std::vector<unsigned char> hash_md5 = md5.final();
        std::cout << "MD5:     " << to_hex(hash_md5) << std::endl;

        // 测试4：HMAC-SHA256
        std::cout << "\n=== 测试4：HMAC-SHA256 ===" << std::endl;
        std::vector<unsigned char> key = {'s', 'e', 'c', 'r', 'e', 't', '_', 'k', 'e', 'y'};
        std::vector<unsigned char> data_vec(data.begin(), data.end());
        std::vector<unsigned char> hmac = HMACUtils::sha256(key, data_vec);
        std::cout << "密钥:     " << to_hex(key) << std::endl;
        std::cout << "HMAC:     " << to_hex(hmac) << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
