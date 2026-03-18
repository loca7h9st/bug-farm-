/**
 * 第7章：抽象IO (BIO) - 示例代码
 *
 * 编译: g++ -std=c++11 -o bio_example bio_example.cpp -lssl -lcrypto
 */

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// ============================================
// Mem BIO类
// ============================================
class MemBIO {
private:
    BIO* bio_;

public:
    MemBIO() {
        bio_ = BIO_new(BIO_s_mem());
        if (!bio_) throw std::runtime_error("BIO_new failed");
    }

    ~MemBIO() {
        if (bio_) BIO_free(bio_);
    }

    void write(const std::string& data) {
        BIO_write(bio_, data.c_str(), data.size());
    }

    std::string read(int max_len = 4096) {
        std::vector<char> buf(max_len + 1);
        int len = BIO_read(bio_, buf.data(), max_len);
        return std::string(buf.data(), len);
    }

    BIO* get() { return bio_; }
};

// ============================================
// MD BIO类
// ============================================
class MDBIO {
private:
    BIO* bio_;
    BIO* md_bio_;

public:
    MDBIO(const EVP_MD* md = EVP_sha256()) {
        bio_ = BIO_new(BIO_s_mem());
        md_bio_ = BIO_new(BIO_f_md());
        BIO_set_md(md_bio_, md);
        BIO_push(md_bio_, bio_);
    }

    ~MDBIO() {
        BIO_free_all(md_bio_);
    }

    void update(const std::string& data) {
        BIO_write(md_bio_, data.c_str(), data.size());
    }

    std::vector<unsigned char> final() {
        BIO_flush(md_bio_);
        const EVP_MD* md;
        BIO_get_md(md_bio_, &md);
        
        // 重新计算摘要
        std::string content = read_all_from_mem_bio();
        std::vector<unsigned char> result(EVP_MAX_MD_SIZE);
        unsigned int result_len;
        
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, md, nullptr);
        EVP_DigestUpdate(ctx, content.data(), content.size());
        EVP_DigestFinal_ex(ctx, result.data(), &result_len);
        EVP_MD_CTX_free(ctx);
        
        result.resize(result_len);
        return result;
    }
    
    std::string read_all_from_mem_bio() {
        // 重新从mem bio读取所有内容
        char buf[4096];
        std::string result;
        BIO* mem_bio = BIO_next(md_bio_); // 获取下面的mem bio
        if (mem_bio) {
            while (true) {
                int len = BIO_read(mem_bio, buf, sizeof(buf) - 1);
                if (len <= 0) break;
                buf[len] = '\0';
                result += buf;
            }
        }
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
    std::cout << "  第7章：抽象IO (BIO) - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // 测试1：Mem BIO
        std::cout << "\n=== 测试1：Mem BIO ===" << std::endl;
        MemBIO mem_bio;
        mem_bio.write("Hello, Mem BIO!");
        std::string data = mem_bio.read();
        std::cout << "读取到: " << data << std::endl;
        std::cout << "✅ Mem BIO测试成功！" << std::endl;

        // 测试2：MD BIO (SHA-256)
        std::cout << "\n=== 测试2：MD BIO (SHA-256) ===" << std::endl;
        MDBIO md_bio(EVP_sha256());
        md_bio.update("Hello, MD BIO!");
        std::vector<unsigned char> md = md_bio.final();
        std::cout << "SHA-256: " << to_hex(md) << std::endl;
        std::cout << "✅ MD BIO测试成功！" << std::endl;

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
