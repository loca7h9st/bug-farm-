# 第一章：BIO - 抽象IO

## 📖 概述

BIO（Basic I/O）是OpenSSL提供的一套抽象IO接口，它统一了各种不同类型的IO操作（文件、内存、网络、SSL等），使得我们可以用相同的方式处理不同的IO源。

**为什么需要BIO？**
- 统一接口：文件、内存、网络、SSL都用同一套API
- 链式处理：可以将多个BIO连接起来形成过滤器链
- 平台无关：屏蔽了不同操作系统的IO差异

---

## 🏗️ BIO核心数据结构

### 1. BIO_METHOD - BIO方法表

```c
typedef struct bio_method_st {
    int type;                    // BIO类型，如BIO_TYPE_MEM、BIO_TYPE_FILE等
    const char *name;            // BIO名称，用于调试
    int (*bwrite) (BIO *, const char *, int);  // 写函数指针
    int (*bread) (BIO *, char *, int);         // 读函数指针
    int (*bputs) (BIO *, const char *);         // 写字符串函数指针
    int (*bgets) (BIO *, char *, int);         // 读行函数指针
    long (*ctrl) (BIO *, int, long, void *);    // 控制函数（通用控制）
    int (*create) (BIO *);                      // 创建函数
    int (*destroy) (BIO *);                     // 销毁函数
    long (*callback_ctrl) (BIO *, int, bio_info_cb *);  // 回调控制
} BIO_METHOD;
```

**成员说明：**

| 成员 | 作用 |
|------|------|
| `type` | BIO类型，如：BIO_TYPE_MEM(内存), BIO_TYPE_FILE(文件), BIO_TYPE_SOCKET(网络), BIO_TYPE_SSL(SSL) |
| `name` | 人类可读的名称，用于调试 |
| `bwrite` | 写数据的函数指针 |
| `bread` | 读数据的函数指针 |
| `bputs` | 写字符串（以\0结尾）的函数指针 |
| `bgets` | 读一行的函数指针 |
| `ctrl` | 通用控制函数，用于各种特殊操作（刷新、关闭、获取信息等） |
| `create` | 创建BIO时调用的初始化函数 |
| `destroy` | 销毁BIO时调用的清理函数 |
| `callback_ctrl` | 回调控制函数 |

---

### 2. BIO - BIO主体结构

```c
struct bio_st {
    BIO_METHOD *method;          // 指向BIO方法表
    long (*callback) (struct bio_st *, int, const char *, int, long, long);  // 回调函数
    char *cb_arg;               // 回调函数的第一个参数
    int init;                   // 是否已初始化标志
    int shutdown;               // 是否需要关闭标志
    int flags;                  // 额外标志位
    int retry_reason;           // 重试原因（当IO需要重试时）
    int num;                    // 通用数字字段
    void *ptr;                  // 通用指针，指向具体BIO的私有数据
    struct bio_st *next_bio;    // 下一个BIO（用于过滤器链）
    struct bio_st *prev_bio;    // 上一个BIO（用于过滤器链）
    int references;             // 引用计数
    unsigned long num_read;     // 已读字节数
    unsigned long num_write;    // 已写字节数
    CRYPTO_EX_DATA ex_data;     // 额外数据
};
```

**成员详细说明：**

| 成员 | 作用 |
|------|------|
| `method` | 指向BIO_METHOD，决定这个BIO的具体行为 |
| `callback` | 回调函数，在IO操作前后被调用 |
| `cb_arg` | 传递给回调函数的参数 |
| `init` | 初始化标志，1表示已初始化 |
| `shutdown` | 关闭标志，1表示需要关闭 |
| `flags` | 标志位，如BIO_FLAGS_READ、BIO_FLAGS_WRITE、BIO_FLAGS_SHOULD_RETRY等 |
| `retry_reason` | 当IO需要重试时，记录原因（如BIO_RR_CONNECT、BIO_RR_ACCEPT） |
| `num` | 通用数字字段，不同BIO用途不同 |
| `ptr` | 通用指针，指向具体BIO的私有数据结构 |
| `next_bio` | 指向链中的下一个BIO（用于过滤器模式） |
| `prev_bio` | 指向链中的上一个BIO（用于过滤器模式） |
| `references` | 引用计数，用于内存管理 |
| `num_read` | 统计：从这个BIO读取的总字节数 |
| `num_write` | 统计：写入这个BIO的总字节数 |
| `ex_data` | 额外数据，用于扩展 |

---

## 🔗 BIO类型

### 源/接收器BIO（Source/Sink）

这些BIO负责实际的IO操作：

| 类型 | 宏定义 | 说明 |
|------|--------|------|
| 内存BIO | BIO_TYPE_MEM | 读写内存缓冲区 |
| 文件BIO | BIO_TYPE_FILE | 读写文件 |
| Socket BIO | BIO_TYPE_SOCKET | 网络Socket读写 |
| FD BIO | BIO_TYPE_FD | 文件描述符读写 |
| NULL BIO | BIO_TYPE_NULL | 空操作（类似/dev/null） |

### 过滤器BIO（Filter）

这些BIO位于源/接收器BIO之上，处理数据：

| 类型 | 宏定义 | 说明 |
|------|--------|------|
| 缓冲BIO | BIO_TYPE_BUFFER | 提供缓冲功能 |
| 摘要BIO | BIO_TYPE_MD | 计算消息摘要 |
| 加密BIO | BIO_TYPE_CIPHER | 加密/解密数据 |
| Base64 BIO | BIO_TYPE_BASE64 | Base64编解码 |
| SSL BIO | BIO_TYPE_SSL | SSL/TLS加密 |

---

## 🚀 基本BIO操作

### 1. 创建BIO

```c
// 创建内存BIO
BIO *bio = BIO_new(BIO_s_mem());

// 创建文件BIO（只读）
BIO *bio = BIO_new_file("filename.txt", "r");

// 创建Socket BIO
BIO *bio = BIO_new_socket(sockfd, BIO_NOCLOSE);
```

### 2. 写数据

```c
// 写数据
int written = BIO_write(bio, data, len);

// 写字符串
int written = BIO_puts(bio, "Hello World");

// 刷新缓冲区
BIO_flush(bio);
```

### 3. 读数据

```c
// 读数据
char buf[1024];
int read = BIO_read(bio, buf, sizeof(buf));

// 读一行
char buf[1024];
char *result = BIO_gets(bio, buf, sizeof(buf));
```

### 4. 销毁BIO

```c
// 释放一个BIO
BIO_free(bio);

// 释放整个BIO链
BIO_free_all(bio);
```

---

## 🔗 BIO链（过滤器模式）

BIO可以链接起来形成处理链：

```
应用程序 <--> Base64 BIO <--> Cipher BIO <--> SSL BIO <--> Socket BIO <--> 网络
```

### 创建BIO链

```c
// 创建Socket BIO
BIO *sbio = BIO_new_socket(sockfd, BIO_NOCLOSE);

// 创建SSL BIO
BIO *ssl_bio = BIO_new_ssl(ctx, 1);  // 1表示客户端模式

// 创建加密BIO
BIO *cipher_bio = BIO_new_cipher();

// 链接起来
BIO_push(cipher_bio, ssl_bio);
BIO_push(ssl_bio, sbio);

// 现在通过cipher_bio读写，数据会自动经过所有过滤器
BIO_write(cipher_bio, data, len);
```

---

## 💻 C++面向对象封装

让我们用C++11封装BIO，实现RAII（资源获取即初始化）：

### Bio类封装

```cpp
#include <openssl/bio.h>
#include <string>
#include <stdexcept>

class Bio {
protected:
    BIO* bio_;

public:
    // 构造函数：接管一个BIO指针
    explicit Bio(BIO* bio) : bio_(bio) {
        if (!bio_) {
            throw std::runtime_error("Bio: null pointer");
        }
    }

    // 移动构造
    Bio(Bio&& other) noexcept : bio_(other.bio_) {
        other.bio_ = nullptr;
    }

    // 移动赋值
    Bio& operator=(Bio&& other) noexcept {
        if (this != &other) {
            if (bio_) {
                BIO_free(bio_);
            }
            bio_ = other.bio_;
            other.bio_ = nullptr;
        }
        return *this;
    }

    // 禁用拷贝
    Bio(const Bio&) = delete;
    Bio& operator=(const Bio&) = delete;

    // 析构函数
    virtual ~Bio() {
        if (bio_) {
            BIO_free(bio_);
        }
    }

    // 获取原始BIO指针
    BIO* get() const { return bio_; }

    // 写数据
    int write(const void* data, int len) {
        return BIO_write(bio_, data, len);
    }

    // 写字符串
    int puts(const std::string& str) {
        return BIO_puts(bio_, str.c_str());
    }

    // 读数据
    int read(void* buf, int len) {
        return BIO_read(bio_, buf, len);
    }

    // 读字符串
    std::string read(int len) {
        std::string result(len, '\0');
        int n = read(&result[0], len);
        if (n > 0) {
            result.resize(n);
        } else {
            result.clear();
        }
        return result;
    }

    // 读一行
    std::string gets() {
        char buf[4096];
        if (BIO_gets(bio_, buf, sizeof(buf))) {
            return std::string(buf);
        }
        return "";
    }

    // 刷新
    int flush() {
        return BIO_flush(bio_);
    }

    // 链接另一个BIO（push）
    void push(Bio& other) {
        BIO_push(bio_, other.bio_);
    }

    // 弹出一个BIO
    Bio pop() {
        return Bio(BIO_pop(bio_));
    }

    // 获取下一个BIO
    BIO* next() const {
        return BIO_next(bio_);
    }

    // 获取已读字节数
    unsigned long num_read() const {
        return bio_->num_read;
    }

    // 获取已写字节数
    unsigned long num_write() const {
        return bio_->num_write;
    }

    // 检查是否需要重试读
    bool should_retry_read() const {
        return BIO_should_retry(bio_) && BIO_should_read(bio_);
    }

    // 检查是否需要重试写
    bool should_retry_write() const {
        return BIO_should_retry(bio_) && BIO_should_write(bio_);
    }
};
```

### MemoryBio - 内存BIO封装

```cpp
class MemoryBio : public Bio {
public:
    // 创建空的内存BIO
    MemoryBio() : Bio(BIO_new(BIO_s_mem())) {}

    // 从现有数据创建
    MemoryBio(const void* data, int len) : Bio(BIO_new_mem_buf(data, len)) {}

    // 从字符串创建
    explicit MemoryBio(const std::string& str) 
        : MemoryBio(str.data(), str.size()) {}

    // 获取内存中的所有数据
    std::string get_content() {
        BUF_MEM* bptr;
        BIO_get_mem_ptr(bio_, &bptr);
        return std::string(bptr->data, bptr->length);
    }

    // 设置内存为只读
    void set_read_only() {
        BIO_set_mem_eof_return(bio_, 0);
    }
};
```

### FileBio - 文件BIO封装

```cpp
class FileBio : public Bio {
public:
    // 打开文件
    // mode: "r", "w", "a", "r+", "w+", "a+" 等
    FileBio(const std::string& filename, const std::string& mode)
        : Bio(BIO_new_file(filename.c_str(), mode.c_str())) {
        if (!bio_) {
            throw std::runtime_error("FileBio: cannot open file " + filename);
        }
    }

    // 从FILE*创建
    explicit FileBio(FILE* fp, int close_flag = BIO_NOCLOSE)
        : Bio(BIO_new_fp(fp, close_flag)) {}
};
```

### SocketBio - Socket BIO封装

```cpp
class SocketBio : public Bio {
public:
    // 从socket创建
    // close_flag: BIO_NOCLOSE 或 BIO_CLOSE
    SocketBio(int sockfd, int close_flag = BIO_NOCLOSE)
        : Bio(BIO_new_socket(sockfd, close_flag)) {}

    // 设置非阻塞模式
    void set_nonblocking() {
        BIO_set_nbio(bio_, 1);
    }

    // 设置阻塞模式
    void set_blocking() {
        BIO_set_nbio(bio_, 0);
    }

    // 获取socket fd
    int get_fd() {
        int fd;
        BIO_get_fd(bio_, &fd);
        return fd;
    }
};
```

---

## 📝 完整示例：内存BIO读写

```cpp
#include "bio_wrapper.h"
#include <iostream>

int main() {
    try {
        // 示例1：写数据到内存BIO
        std::cout << "=== 示例1：写数据到内存BIO ===" << std::endl;
        {
            MemoryBio bio;
            bio.puts("Hello, OpenSSL BIO!");
            bio.write(" This is additional data.", 23);
            bio.flush();
            
            std::string content = bio.get_content();
            std::cout << "内存BIO内容: " << content << std::endl;
            std::cout << "已写字节数: " << bio.num_write() << std::endl;
        }

        // 示例2：从内存BIO读取
        std::cout << "\n=== 示例2：从内存BIO读取 ===" << std::endl;
        {
            std::string data = "Line 1\nLine 2\nLine 3\n";
            MemoryBio bio(data);
            
            std::cout << "第一行: " << bio.gets();
            std::cout << "第二行: " << bio.gets();
            
            std::string remaining = bio.read(1024);
            std::cout << "剩余内容: " << remaining << std::endl;
        }

        // 示例3：C风格读写
        std::cout << "\n=== 示例3：C风格读写 ===" << std::endl;
        {
            MemoryBio bio;
            
            // 写
            const char* write_data = "Test data";
            int written = bio.write(write_data, 9);
            std::cout << "写入了 " << written << " 字节" << std::endl;
            
            // 读
            char read_buf[100];
            int read_bytes = bio.read(read_buf, sizeof(read_buf));
            if (read_bytes > 0) {
                read_buf[read_bytes] = '\0';
                std::cout << "读取了 " << read_bytes << " 字节: " << read_buf << std::endl;
            }
        }

        std::cout << "\n所有示例完成！" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}
```

---

## 🛠️ 编译说明

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(OpenSSL_Tutorial)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(example_01_memory_bio examples/01_bio/01_memory_bio.cpp)
target_link_libraries(example_01_memory_bio ${OPENSSL_LIBRARIES})
```

### 编译命令

```bash
# 使用CMake
mkdir build && cd build
cmake ..
make

# 或直接编译
g++ -std=c++11 -o example_01_memory_bio example_01_memory_bio.cpp -lssl -lcrypto
```

---

## 📚 参考资料

- OpenSSL源码: crypto/bio/bio.h
- OpenSSL源码: crypto/bio/bio_lcl.h
- OpenSSL官方文档: https://www.openssl.org/docs/

---

*第一章完*
