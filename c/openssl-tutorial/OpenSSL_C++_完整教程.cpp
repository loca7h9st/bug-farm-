/**
 * OpenSSL C++ 面向对象编程完整教程 - 示例代码
 *
 * 编译命令:
 *   g++ -std=c++11 -o OpenSSL_C++_完整教程 OpenSSL_C++_完整教程.cpp -lssl -lcrypto
 *
 * 作者: 牛东东🐮
 * 日期: 2026-03-14
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>

#include <openssl/bio.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// ============================================
// 第一章：BIO - 抽象IO
// ============================================

class Bio {
protected:
    BIO* bio_;

public:
    explicit Bio(BIO* bio) : bio_(bio) {
        if (!bio_) {
            throw std::runtime_error("Bio: null pointer");
        }
    }

    Bio(Bio&& other) noexcept : bio_(other.bio_) {
        other.bio_ = nullptr;
    }

    Bio& operator=(Bio&& other) noexcept {
        if (this != &other) {
            if (bio_) BIO_free(bio_);
            bio_ = other.bio_;
            other.bio_ = nullptr;
        }
        return *this;
    }

    Bio(const Bio&) = delete;
    Bio& operator=(const Bio&) = delete;

    virtual ~Bio() {
        if (bio_) BIO_free(bio_);
    }

    BIO* get() const { return bio_; }
    int write(const void* data, int len) { return BIO_write(bio_, data, len); }
    int puts(const std::string& str) { return BIO_puts(bio_, str.c_str()); }
    int read(void* buf, int len) { return BIO_read(bio_, buf, len); }

    std::string read(int len) {
        std::string result(len, '\0');
        int n = read(&result[0], len);
        if (n > 0) result.resize(n);
        else result.clear();
        return result;
    }

    std::string gets() {
        char buf[4096];
        if (BIO_gets(bio_, buf, sizeof(buf))) {
            return std::string(buf);
        }
        return "";
    }

    int flush() { return BIO_flush(bio_); }
};

class MemoryBio : public Bio {
public:
    MemoryBio() : Bio(BIO_new(BIO_s_mem())) {}
    MemoryBio(const void* data, int len) : Bio(BIO_new_mem_buf(data, len)) {}
    explicit MemoryBio(const std::string& str) : MemoryBio(str.data(), str.size()) {}

    std::string get_content() {
        BUF_MEM* bptr;
        BIO_get_mem_ptr(bio_, &bptr);
        return std::string(bptr->data, bptr->length);
    }
};

// ============================================
// 第二章：对称加密算法
// ============================================

class AESCipher {
private:
    AES_KEY encrypt_key_;
    AES_KEY decrypt_key_;
    unsigned char iv_[AES_BLOCK_SIZE];

public:
    AESCipher(const unsigned char* key) {
        if (AES_set_encrypt_key(key, 256, &encrypt_key_) != 0) {
            throw std::runtime_error("AES_set_encrypt_key failed");
        }
        if (AES_set_decrypt_key(key, 256, &decrypt_key_) != 0) {
            throw std::runtime_error("AES_set_decrypt_key failed");
        }
        RAND_bytes(iv_, AES_BLOCK_SIZE);
    }

    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& plaintext) {
        size_t padded_len = ((plaintext.size() + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
        std::vector<unsigned char> padded = plaintext;
        padded.resize(padded_len, 0);

        std::vector<unsigned char> ciphertext(AES_BLOCK_SIZE + padded_len);
        memcpy(ciphertext.data(), iv_, AES_BLOCK_SIZE);

        unsigned char temp_iv[AES_BLOCK_SIZE];
        memcpy(temp_iv, iv_, AES_BLOCK_SIZE);

        AES_cbc_encrypt(
            padded.data(),
            ciphertext.data() + AES_BLOCK_SIZE,
            padded_len,
            &encrypt_key_,
            temp_iv,
            AES_ENCRYPT
        );

        return ciphertext;
    }

    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& ciphertext) {
        if (ciphertext.size() < AES_BLOCK_SIZE) {
            throw std::runtime_error("Invalid ciphertext");
        }

        unsigned char temp_iv[AES_BLOCK_SIZE];
        memcpy(temp_iv, ciphertext.data(), AES_BLOCK_SIZE);

        std::vector<unsigned char> plaintext(ciphertext.size() - AES_BLOCK_SIZE);

        AES_cbc_encrypt(
            ciphertext.data() + AES_BLOCK_SIZE,
            plaintext.data(),
            plaintext.size(),
            &decrypt_key_,
            temp_iv,
            AES_DECRYPT
        );

        return plaintext;
    }

    const unsigned char* get_iv() const { return iv_; }
};

// ============================================
// 第三章：非对称加密算法
// ============================================

class RSAWrapper {
private:
    RSA* rsa_;

public:
    RSAWrapper(int bits = 2048) {
        BIGNUM* e = BN_new();
        BN_set_word(e, RSA_F4);
        rsa_ = RSA_new();
        RSA_generate_key_ex(rsa_, bits, e, nullptr);
        BN_free(e);
        
        if (!rsa_) {
            throw std::runtime_error("RSA key generation failed");
        }
    }

    ~RSAWrapper() {
        if (rsa_) RSA_free(rsa_);
    }

    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data) {
        std::vector<unsigned char> encrypted(RSA_size(rsa_));

        int len = RSA_public_encrypt(
            data.size(),
            data.data(),
            encrypted.data(),
            rsa_,
            RSA_PKCS1_OAEP_PADDING
        );

        if (len == -1) {
            throw std::runtime_error("RSA encryption failed");
        }

        encrypted.resize(len);
        return encrypted;
    }

    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data) {
        std::vector<unsigned char> decrypted(RSA_size(rsa_));

        int len = RSA_private_decrypt(
            data.size(),
            data.data(),
            decrypted.data(),
            rsa_,
            RSA_PKCS1_OAEP_PADDING
        );

        if (len == -1) {
            throw std::runtime_error("RSA decryption failed");
        }

        decrypted.resize(len);
        return decrypted;
    }
};

// ============================================
// 第四章：摘要算法与HMAC
// ============================================

class Hash {
public:
    static std::vector<unsigned char> sha256(const std::vector<unsigned char>& data) {
        std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
        SHA256(data.data(), data.size(), hash.data());
        return hash;
    }

    static std::vector<unsigned char> sha256(const std::string& data) {
        return sha256(std::vector<unsigned char>(data.begin(), data.end()));
    }

    static std::string to_hex(const std::vector<unsigned char>& hash) {
        static const char* hex = "0123456789abcdef";
        std::string result;
        result.reserve(hash.size() * 2);
        for (unsigned char c : hash) {
            result += hex[(c >> 4) & 0x0F];
            result += hex[c & 0x0F];
        }
        return result;
    }
};

class HMACUtils {
public:
    static std::vector<unsigned char> sha256(
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& data
    ) {
        std::vector<unsigned char> hmac(SHA256_DIGEST_LENGTH);
        unsigned int len;
        ::HMAC(
            EVP_sha256(),
            key.data(), key.size(),
            data.data(), data.size(),
            hmac.data(), &len
        );
        return hmac;
    }
};

// ============================================
// 测试函数
// ============================================

void test_bio() {
    std::cout << "========================================" << std::endl;
    std::cout << "  测试1：BIO - 抽象IO" << std::endl;
    std::cout << "========================================" << std::endl;

    // 测试1：写数据到内存BIO
    std::cout << "\n[1.1] 写数据到内存BIO" << std::endl;
    {
        MemoryBio bio;
        bio.puts("Hello, OpenSSL BIO!");
        bio.write(" This is additional data.", 23);
        bio.flush();
        
        std::string content = bio.get_content();
        std::cout << "  内存BIO内容: " << content << std::endl;
    }

    // 测试2：从内存BIO读取
    std::cout << "\n[1.2] 从内存BIO读取" << std::endl;
    {
        std::string data = "Line 1\nLine 2\nLine 3\n";
        MemoryBio bio(data);
        
        std::cout << "  第一行: " << bio.gets();
        std::cout << "  第二行: " << bio.gets();
        
        std::string remaining = bio.read(1024);
        std::cout << "  剩余内容: " << remaining << std::endl;
    }

    std::cout << "\n✅ BIO测试完成！" << std::endl;
}

void test_aes() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试2：AES-256对称加密" << std::endl;
    std::cout << "========================================" << std::endl;

    // 生成随机密钥
    unsigned char key[32];
    RAND_bytes(key, 32);

    // 待加密数据
    std::string plaintext_str = "This is a secret message for AES encryption test!";
    std::vector<unsigned char> plaintext(plaintext_str.begin(), plaintext_str.end());

    std::cout << "\n[2.1] 原始数据: " << plaintext_str << std::endl;

    // 加密
    AESCipher cipher(key);
    std::vector<unsigned char> ciphertext = cipher.encrypt(plaintext);
    std::cout << "[2.2] 加密完成！密文大小: " << ciphertext.size() << " 字节" << std::endl;

    // 解密
    std::vector<unsigned char> decrypted = cipher.decrypt(ciphertext);
    std::string decrypted_str(decrypted.begin(), decrypted.end());
    
    // 移除可能的填充
    size_t end = decrypted_str.find_first_of('\0');
    if (end != std::string::npos) {
        decrypted_str = decrypted_str.substr(0, end);
    }

    std::cout << "[2.3] 解密完成！数据: " << decrypted_str << std::endl;

    // 验证
    if (decrypted_str == plaintext_str) {
        std::cout << "✅ AES测试成功！解密数据与原始数据一致！" << std::endl;
    } else {
        std::cout << "❌ AES测试失败！" << std::endl;
    }
}

void test_hash() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试3：SHA-256摘要" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string data = "Hello, SHA-256!";
    std::cout << "\n[3.1] 原始数据: " << data << std::endl;

    std::vector<unsigned char> hash = Hash::sha256(data);
    std::string hex_hash = Hash::to_hex(hash);

    std::cout << "[3.2] SHA-256哈希: " << hex_hash << std::endl;
    std::cout << "✅ SHA-256测试完成！" << std::endl;
}

void test_hmac() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试4：HMAC-SHA256" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string key_str = "secret-key-12345";
    std::string data = "Message to authenticate";

    std::vector<unsigned char> key(key_str.begin(), key_str.end());
    std::vector<unsigned char> data_vec(data.begin(), data.end());

    std::cout << "\n[4.1] 密钥: " << key_str << std::endl;
    std::cout << "[4.2] 数据: " << data << std::endl;

    std::vector<unsigned char> hmac = HMACUtils::sha256(key, data_vec);
    std::string hex_hmac = Hash::to_hex(hmac);

    std::cout << "[4.3] HMAC-SHA256: " << hex_hmac << std::endl;
    std::cout << "✅ HMAC测试完成！" << std::endl;
}

void test_rsa() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试5：RSA非对称加密" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "\n[5.1] 生成RSA-2048密钥对..." << std::endl;
    RSAWrapper rsa(2048);
    std::cout << "[5.2] 密钥生成完成！" << std::endl;

    std::string plaintext_str = "Secret message for RSA!";
    std::vector<unsigned char> plaintext(plaintext_str.begin(), plaintext_str.end());

    std::cout << "\n[5.3] 原始数据: " << plaintext_str << std::endl;

    std::vector<unsigned char> encrypted = rsa.encrypt(plaintext);
    std::cout << "[5.4] 加密完成！密文大小: " << encrypted.size() << " 字节" << std::endl;

    std::vector<unsigned char> decrypted = rsa.decrypt(encrypted);
    std::string decrypted_str(decrypted.begin(), decrypted.end());

    std::cout << "[5.5] 解密完成！数据: " << decrypted_str << std::endl;

    if (decrypted_str == plaintext_str) {
        std::cout << "✅ RSA测试成功！" << std::endl;
    } else {
        std::cout << "❌ RSA测试失败！" << std::endl;
    }
}

// ============================================
// 主函数
// ============================================

int main() {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         OpenSSL C++ 面向对象编程完整教程 - 测试程序          ║" << std::endl;
    std::cout << "║                    作者: 牛东东🐮                             ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

    try {
        test_bio();
        test_aes();
        test_hash();
        test_hmac();
        test_rsa();

        std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    所有测试完成！🎉                          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
