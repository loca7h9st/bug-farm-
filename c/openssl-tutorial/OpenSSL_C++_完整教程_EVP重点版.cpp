/**
 * OpenSSL C++ 完整教程 - EVP重点版
 *
 * 仿造《openssl编程.pdf》章节结构
 * 重点：EVP加解密
 *
 * 编译命令:
 *   g++ -std=c++11 -o OpenSSL_C++_完整教程_EVP重点版 OpenSSL_C++_完整教程_EVP重点版.cpp -lssl -lcrypto
 *
 * 作者: 牛东东🐮
 * 日期: 2026-03-14
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>

// ============================================
// 工具函数
// ============================================

class Util {
public:
    static std::string to_hex(const std::vector<unsigned char>& data) {
        static const char* hex = "0123456789abcdef";
        std::string result;
        result.reserve(data.size() * 2);
        for (unsigned char c : data) {
            result += hex[(c >> 4) & 0x0F];
            result += hex[c & 0x0F];
        }
        return result;
    }

    static void print_hex(const std::string& label, const std::vector<unsigned char>& data) {
        std::cout << label << ": " << to_hex(data) << std::endl;
    }

    static void print_hex(const std::string& label, const unsigned char* data, size_t len) {
        std::vector<unsigned char> v(data, data + len);
        print_hex(label, v);
    }
};

// ============================================
// 第十五章：EVP摘要（SHA-256, SHA-1, MD5）
// ============================================

class EVPDigest {
public:
    static std::vector<unsigned char> sha256(const std::vector<unsigned char>& data) {
        return digest(data, EVP_sha256());
    }

    static std::vector<unsigned char> sha256(const std::string& data) {
        return sha256(std::vector<unsigned char>(data.begin(), data.end()));
    }

    static std::vector<unsigned char> sha1(const std::vector<unsigned char>& data) {
        return digest(data, EVP_sha1());
    }

    static std::vector<unsigned char> md5(const std::vector<unsigned char>& data) {
        return digest(data, EVP_md5());
    }

private:
    static std::vector<unsigned char> digest(const std::vector<unsigned char>& data, const EVP_MD* md) {
        EVP_MD_CTX* ctx = EVP_MD_CTX_create();
        if (!ctx) {
            throw std::runtime_error("EVP_MD_CTX_create failed");
        }

        if (EVP_DigestInit_ex(ctx, md, nullptr) != 1) {
            EVP_MD_CTX_destroy(ctx);
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }

        if (EVP_DigestUpdate(ctx, data.data(), data.size()) != 1) {
            EVP_MD_CTX_destroy(ctx);
            throw std::runtime_error("EVP_DigestUpdate failed");
        }

        std::vector<unsigned char> result(EVP_MAX_MD_SIZE);
        unsigned int result_len;
        if (EVP_DigestFinal_ex(ctx, result.data(), &result_len) != 1) {
            EVP_MD_CTX_destroy(ctx);
            throw std::runtime_error("EVP_DigestFinal_ex failed");
        }

        result.resize(result_len);
        EVP_MD_CTX_destroy(ctx);
        return result;
    }
};

// ============================================
// 第二十一章：EVP对称加密（重点！）
// ============================================

class EVPCipher {
private:
    const EVP_CIPHER* cipher_;
    std::vector<unsigned char> key_;
    std::vector<unsigned char> iv_;

public:
    // 构造函数：指定加密算法
    // cipher: EVP_aes_256_cbc(), EVP_aes_128_cbc() 等
    EVPCipher(const EVP_CIPHER* cipher) : cipher_(cipher) {
        key_.resize(EVP_CIPHER_key_length(cipher));
        iv_.resize(EVP_CIPHER_iv_length(cipher));
        
        // 生成随机密钥和IV
        if (RAND_bytes(key_.data(), key_.size()) != 1) {
            throw std::runtime_error("RAND_bytes for key failed");
        }
        if (RAND_bytes(iv_.data(), iv_.size()) != 1) {
            throw std::runtime_error("RAND_bytes for IV failed");
        }
    }

    // 构造函数：使用指定的密钥和IV
    EVPCipher(const EVP_CIPHER* cipher, 
              const std::vector<unsigned char>& key, 
              const std::vector<unsigned char>& iv)
        : cipher_(cipher), key_(key), iv_(iv) {}

    // 获取密钥
    const std::vector<unsigned char>& get_key() const { return key_; }

    // 获取IV
    const std::vector<unsigned char>& get_iv() const { return iv_; }

    // 加密
    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& plaintext) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("EVP_CIPHER_CTX_new failed");
        }

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

    // 解密
    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& ciphertext) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            throw std::runtime_error("EVP_CIPHER_CTX_new failed");
        }

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
// 第十七章：RSA（使用RSA直接API）
// ============================================

class RSAWrapper {
private:
    RSA* rsa_;

public:
    // 生成RSA密钥对
    RSAWrapper(int bits = 2048) {
        BIGNUM* e = BN_new();
        BN_set_word(e, RSA_F4);
        rsa_ = RSA_new();
        if (!RSA_generate_key_ex(rsa_, bits, e, nullptr)) {
            BN_free(e);
            RSA_free(rsa_);
            throw std::runtime_error("RSA_generate_key_ex failed");
        }
        BN_free(e);
    }

    ~RSAWrapper() {
        if (rsa_) {
            RSA_free(rsa_);
        }
    }

    // 公钥加密
    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data) {
        std::vector<unsigned char> result(RSA_size(rsa_));

        int len = RSA_public_encrypt(
            data.size(),
            data.data(),
            result.data(),
            rsa_,
            RSA_PKCS1_OAEP_PADDING
        );

        if (len == -1) {
            throw std::runtime_error("RSA_public_encrypt failed");
        }

        result.resize(len);
        return result;
    }

    // 私钥解密
    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data) {
        std::vector<unsigned char> result(RSA_size(rsa_));

        int len = RSA_private_decrypt(
            data.size(),
            data.data(),
            result.data(),
            rsa_,
            RSA_PKCS1_OAEP_PADDING
        );

        if (len == -1) {
            throw std::runtime_error("RSA_private_decrypt failed");
        }

        result.resize(len);
        return result;
    }

    // 获取密钥位数
    int bits() const {
        return RSA_bits(rsa_);
    }
};

// ============================================
// 测试函数
// ============================================

void test_evp_digest() {
    std::cout << "========================================" << std::endl;
    std::cout << "  测试1：EVP摘要（SHA-256, SHA-1, MD5）" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string data = "Hello, EVP Digest!";
    std::cout << "\n[1.1] 原始数据: " << data << std::endl;

    std::vector<unsigned char> sha256 = EVPDigest::sha256(data);
    std::vector<unsigned char> sha1 = EVPDigest::sha1(std::vector<unsigned char>(data.begin(), data.end()));
    std::vector<unsigned char> md5 = EVPDigest::md5(std::vector<unsigned char>(data.begin(), data.end()));

    Util::print_hex("[1.2] SHA-256", sha256);
    Util::print_hex("[1.3] SHA-1  ", sha1);
    Util::print_hex("[1.4] MD5    ", md5);

    std::cout << "✅ EVP摘要测试完成！" << std::endl;
}

void test_evp_cipher_aes256cbc() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试2：EVP对称加密（AES-256-CBC）" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string plaintext = "This is a secret message for EVP AES-256-CBC encryption!";
    std::cout << "\n[2.1] 原始数据: " << plaintext << std::endl;

    // 创建加密器（自动生成随机密钥和IV）
    EVPCipher cipher(EVP_aes_256_cbc());

    Util::print_hex("[2.2] 密钥  ", cipher.get_key());
    Util::print_hex("[2.3] IV    ", cipher.get_iv());

    // 加密
    std::vector<unsigned char> ciphertext = cipher.encrypt(plaintext);
    std::cout << "[2.4] 加密完成！密文大小: " << ciphertext.size() << " 字节" << std::endl;

    // 解密
    std::vector<unsigned char> decrypted = cipher.decrypt(ciphertext);
    std::string decrypted_str(decrypted.begin(), decrypted.end());
    std::cout << "[2.5] 解密完成！数据: " << decrypted_str << std::endl;

    // 验证
    if (decrypted_str == plaintext) {
        std::cout << "✅ EVP AES-256-CBC测试成功！" << std::endl;
    } else {
        std::cout << "❌ EVP AES-256-CBC测试失败！" << std::endl;
    }
}

void test_evp_cipher_aes128cbc() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试3：EVP对称加密（AES-128-CBC）" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string plaintext = "This is a secret message for EVP AES-128-CBC encryption!";
    std::cout << "\n[3.1] 原始数据: " << plaintext << std::endl;

    EVPCipher cipher(EVP_aes_128_cbc());

    Util::print_hex("[3.2] 密钥  ", cipher.get_key());
    Util::print_hex("[3.3] IV    ", cipher.get_iv());

    std::vector<unsigned char> ciphertext = cipher.encrypt(plaintext);
    std::cout << "[3.4] 加密完成！密文大小: " << ciphertext.size() << " 字节" << std::endl;

    std::vector<unsigned char> decrypted = cipher.decrypt(ciphertext);
    std::string decrypted_str(decrypted.begin(), decrypted.end());
    std::cout << "[3.5] 解密完成！数据: " << decrypted_str << std::endl;

    if (decrypted_str == plaintext) {
        std::cout << "✅ EVP AES-128-CBC测试成功！" << std::endl;
    } else {
        std::cout << "❌ EVP AES-128-CBC测试失败！" << std::endl;
    }
}

void test_rsa() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试4：RSA非对称加密（RSA-2048）" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "\n[4.1] 生成RSA-2048密钥对..." << std::endl;
    RSAWrapper rsa(2048);
    std::cout << "[4.2] 密钥生成完成！位数: " << rsa.bits() << std::endl;

    std::string plaintext_str = "Secret message for RSA!";
    std::vector<unsigned char> plaintext(plaintext_str.begin(), plaintext_str.end());
    std::cout << "\n[4.3] 原始数据: " << plaintext_str << std::endl;

    std::vector<unsigned char> encrypted = rsa.encrypt(plaintext);
    std::cout << "[4.4] 加密完成！密文大小: " << encrypted.size() << " 字节" << std::endl;

    std::vector<unsigned char> decrypted = rsa.decrypt(encrypted);
    std::string decrypted_str(decrypted.begin(), decrypted.end());
    std::cout << "[4.5] 解密完成！数据: " << decrypted_str << std::endl;

    if (decrypted_str == plaintext_str) {
        std::cout << "✅ RSA测试成功！" << std::endl;
    } else {
        std::cout << "❌ RSA测试失败！" << std::endl;
    }
}

void test_hybrid_encryption() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试5：混合加密（AES+RSA）" << std::endl;
    std::cout << "         （勒索软件常用方案）" << std::endl;
    std::cout << "========================================" << std::endl;

    // 模拟勒索软件加密流程
    std::cout << "\n[5.1] 生成RSA密钥对（攻击者的密钥对）" << std::endl;
    RSAWrapper rsa(2048);

    std::string file_data = "This is the content of an important file that needs to be encrypted!";
    std::cout << "[5.2] 文件内容: " << file_data << std::endl;

    // 1. 生成随机AES密钥（每个文件一个）
    std::cout << "\n[5.3] 生成随机AES-256密钥（文件密钥）" << std::endl;
    EVPCipher aes_cipher(EVP_aes_256_cbc());
    Util::print_hex("      密钥  ", aes_cipher.get_key());
    Util::print_hex("      IV    ", aes_cipher.get_iv());

    // 2. 用AES加密文件内容
    std::cout << "[5.4] 用AES-256-CBC加密文件内容" << std::endl;
    std::vector<unsigned char> encrypted_file = aes_cipher.encrypt(file_data);
    std::cout << "      加密后大小: " << encrypted_file.size() << " 字节" << std::endl;

    // 3. 用RSA加密AES密钥
    std::cout << "[5.5] 用RSA-2048加密AES密钥" << std::endl;
    std::vector<unsigned char> aes_key_and_iv = aes_cipher.get_key();
    aes_key_and_iv.insert(aes_key_and_iv.end(), aes_cipher.get_iv().begin(), aes_cipher.get_iv().end());
    std::vector<unsigned char> encrypted_aes_key = rsa.encrypt(aes_key_and_iv);
    std::cout << "      加密的AES密钥大小: " << encrypted_aes_key.size() << " 字节" << std::endl;

    // 4. 解密流程（模拟攻击者解密）
    std::cout << "\n[5.6] 解密流程（攻击者）" << std::endl;
    
    // a. 用RSA解密AES密钥
    std::cout << "      a. 用RSA解密AES密钥" << std::endl;
    std::vector<unsigned char> decrypted_aes_key_and_iv = rsa.decrypt(encrypted_aes_key);
    
    std::vector<unsigned char> key(decrypted_aes_key_and_iv.begin(), 
                                    decrypted_aes_key_and_iv.begin() + 32);
    std::vector<unsigned char> iv(decrypted_aes_key_and_iv.begin() + 32, 
                                   decrypted_aes_key_and_iv.end());
    
    // b. 用AES解密文件
    std::cout << "      b. 用AES解密文件" << std::endl;
    EVPCipher decryption_cipher(EVP_aes_256_cbc(), key, iv);
    std::vector<unsigned char> decrypted_file = decryption_cipher.decrypt(encrypted_file);
    std::string decrypted_str(decrypted_file.begin(), decrypted_file.end());
    
    std::cout << "[5.7] 解密完成！文件内容: " << decrypted_str << std::endl;

    if (decrypted_str == file_data) {
        std::cout << "✅ 混合加密测试成功！" << std::endl;
    } else {
        std::cout << "❌ 混合加密测试失败！" << std::endl;
    }
}

// ============================================
// 主函数
// ============================================

int main() {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║      OpenSSL C++ 完整教程 - EVP重点版 - 测试程序           ║" << std::endl;
    std::cout << "║         仿造《openssl编程.pdf》章节结构                     ║" << std::endl;
    std::cout << "║                    作者: 牛东东🐮                             ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

    try {
        test_evp_digest();           // 第十五章：摘要
        test_evp_cipher_aes256cbc(); // 第二十一章：AES-256-CBC
        test_evp_cipher_aes128cbc(); // 第二十一章：AES-128-CBC
        test_rsa();                   // 第十七章：RSA
        test_hybrid_encryption();     // 混合加密（勒索软件方案）

        std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    所有测试完成！🎉                          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
