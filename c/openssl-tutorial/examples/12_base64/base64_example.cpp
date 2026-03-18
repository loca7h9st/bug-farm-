/**
 * 第12章：BASE64编解码 - 示例代码
 *
 * 编译: g++ -std=c++11 -o base64_example base64_example.cpp -lssl -lcrypto
 */

#include <openssl/evp.h>
#include <iostream>
#include <vector>
#include <stdexcept>

// ============================================
// BASE64类
// ============================================
class Base64 {
public:
    static std::string encode(const std::vector<unsigned char>& data) {
        EVP_ENCODE_CTX* ctx = EVP_ENCODE_CTX_new();
        if (!ctx) throw std::runtime_error("EVP_ENCODE_CTX_new failed");

        EVP_EncodeInit(ctx);

        std::vector<char> result(data.size() * 2 + 64);
        int outl;

        EVP_EncodeUpdate(ctx, (unsigned char*)result.data(), &outl,
                         data.data(), data.size());
        int total = outl;

        EVP_EncodeFinal(ctx, (unsigned char*)result.data() + outl, &outl);
        total += outl;

        EVP_ENCODE_CTX_free(ctx);
        result.resize(total);
        return std::string(result.begin(), result.end());
    }

    static std::string encode(const std::string& data) {
        return encode(std::vector<unsigned char>(data.begin(), data.end()));
    }

    static std::vector<unsigned char> decode(const std::string& data) {
        EVP_ENCODE_CTX* ctx = EVP_ENCODE_CTX_new();
        if (!ctx) throw std::runtime_error("EVP_ENCODE_CTX_new failed");

        EVP_DecodeInit(ctx);

        std::vector<unsigned char> result(data.size());
        int outl;

        EVP_DecodeUpdate(ctx, result.data(), &outl,
                         (const unsigned char*)data.data(), data.size());
        int total = outl;

        EVP_DecodeFinal(ctx, result.data() + outl, &outl);
        total += outl;

        EVP_ENCODE_CTX_free(ctx);
        result.resize(total);
        return result;
    }

    static std::string decode_to_string(const std::string& data) {
        std::vector<unsigned char> decoded = decode(data);
        return std::string(decoded.begin(), decoded.end());
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
    std::cout << "  第12章：BASE64编解码 - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // 测试1：字符串编码
        std::cout << "\n=== 测试1：字符串编码 ===" << std::endl;
        std::string text = "Hello, OpenSSL BASE64!";
        std::cout << "原始字符串: " << text << std::endl;

        std::string encoded = Base64::encode(text);
        std::cout << "BASE64编码: " << encoded << std::endl;

        std::string decoded = Base64::decode_to_string(encoded);
        std::cout << "BASE64解码: " << decoded << std::endl;

        if (decoded == text) {
            std::cout << "✅ 字符串编码测试成功！" << std::endl;
        } else {
            std::cout << "❌ 字符串编码测试失败！" << std::endl;
            return 1;
        }

        // 测试2：二进制数据编码
        std::cout << "\n=== 测试2：二进制数据编码 ===" << std::endl;
        std::vector<unsigned char> binary = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
        std::cout << "原始二进制: " << to_hex(binary) << std::endl;

        std::string encoded_bin = Base64::encode(binary);
        std::cout << "BASE64编码: " << encoded_bin << std::endl;

        std::vector<unsigned char> decoded_bin = Base64::decode(encoded_bin);
        std::cout << "BASE64解码: " << to_hex(decoded_bin) << std::endl;

        if (decoded_bin == binary) {
            std::cout << "✅ 二进制数据编码测试成功！" << std::endl;
        } else {
            std::cout << "❌ 二进制数据编码测试失败！" << std::endl;
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
