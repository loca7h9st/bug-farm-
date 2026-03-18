/**
 * 示例1：内存BIO读写
 *
 * 演示：
 * 1. 创建内存BIO
 * 2. 写入数据
 * 3. 读取数据
 * 4. 获取所有内容
 */

#include <iostream>
#include <string>
#include <openssl/bio.h>

// 简化的C++封装（仅用于示例）
class MemoryBio {
private:
    BIO* bio_;

public:
    MemoryBio() : bio_(BIO_new(BIO_s_mem())) {}

    MemoryBio(const std::string& data) 
        : bio_(BIO_new_mem_buf(data.data(), data.size())) {}

    ~MemoryBio() {
        if (bio_) {
            BIO_free(bio_);
        }
    }

    int puts(const std::string& str) {
        return BIO_puts(bio_, str.c_str());
    }

    int write(const void* data, int len) {
        return BIO_write(bio_, data, len);
    }

    std::string gets() {
        char buf[4096];
        if (BIO_gets(bio_, buf, sizeof(buf))) {
            return std::string(buf);
        }
        return "";
    }

    std::string read(int len) {
        std::string result(len, '\0');
        int n = BIO_read(bio_, &result[0], len);
        if (n > 0) {
            result.resize(n);
        } else {
            result.clear();
        }
        return result;
    }

    std::string get_content() {
        BUF_MEM* bptr;
        BIO_get_mem_ptr(bio_, &bptr);
        return std::string(bptr->data, bptr->length);
    }

    void flush() {
        BIO_flush(bio_);
    }

    unsigned long num_write() const {
        return bio_->num_write;
    }

    unsigned long num_read() const {
        return bio_->num_read;
    }
};

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  示例1：内存BIO读写" << std::endl;
    std::cout << "========================================" << std::endl;

    // 示例1：写数据到内存BIO
    std::cout << "\n[1] 写数据到内存BIO" << std::endl;
    {
        MemoryBio bio;
        bio.puts("Hello, OpenSSL BIO!");
        bio.write(" This is additional data.", 23);
        bio.flush();
        
        std::string content = bio.get_content();
        std::cout << "  内存BIO内容: " << content << std::endl;
        std::cout << "  已写字节数: " << bio.num_write() << std::endl;
    }

    // 示例2：从内存BIO读取
    std::cout << "\n[2] 从内存BIO读取" << std::endl;
    {
        std::string data = "Line 1\nLine 2\nLine 3\n";
        MemoryBio bio(data);
        
        std::cout << "  第一行: " << bio.gets();
        std::cout << "  第二行: " << bio.gets();
        
        std::string remaining = bio.read(1024);
        std::cout << "  剩余内容: " << remaining << std::endl;
    }

    // 示例3：C风格读写
    std::cout << "\n[3] C风格读写" << std::endl;
    {
        MemoryBio bio;
        
        const char* write_data = "Test data";
        int written = bio.write(write_data, 9);
        std::cout << "  写入了 " << written << " 字节" << std::endl;
        
        char read_buf[100];
        int read_bytes = bio.read(read_buf, sizeof(read_buf));
        if (read_bytes > 0) {
            read_buf[read_bytes] = '\0';
            std::cout << "  读取了 " << read_bytes << " 字节: " << read_buf << std::endl;
        }
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "  所有示例完成！" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
