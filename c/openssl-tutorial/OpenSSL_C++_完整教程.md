# OpenSSL C++ 面向对象编程完整教程

> 学习目的：网络加密、勒索软件学习与开发
>
> 作者：牛东东🐮
>
> 日期：2026-03-14

---

## 📋 目录

1. [第一章：BIO - 抽象IO](#第一章bio---抽象io)
2. [第二章：对称加密算法](#第二章对称加密算法)
3. [第三章：非对称加密算法](#第三章非对称加密算法)
4. [第四章：摘要算法与HMAC](#第四章摘要算法与hmac)
5. [第五章：SSL/TLS网络编程](#第五章ssltls网络编程)
6. [第六章：勒索软件加密方式对比](#第六章勒索软件加密方式对比)
7. [完整示例代码](#完整示例代码)

---

## 第一章：BIO - 抽象IO

### 📖 概述

BIO（Basic I/O）是OpenSSL提供的一套抽象IO接口，它统一了各种不同类型的IO操作（文件、内存、网络、SSL等），使得我们可以用相同的方式处理不同的IO源。

**为什么需要BIO？**
- 统一接口：文件、内存、网络、SSL都用同一套API
- 链式处理：可以将多个BIO连接起来形成过滤器链
- 平台无关：屏蔽了不同操作系统的IO差异

---

### 🏗️ BIO核心数据结构

#### 1. BIO_METHOD - BIO方法表

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

#### 2. BIO - BIO主体结构

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

### 🔗 BIO类型

#### 源/接收器BIO（Source/Sink）

这些BIO负责实际的IO操作：

| 类型 | 宏定义 | 说明 |
|------|--------|------|
| 内存BIO | BIO_TYPE_MEM | 读写内存缓冲区 |
| 文件BIO | BIO_TYPE_FILE | 读写文件 |
| Socket BIO | BIO_TYPE_SOCKET | 网络Socket读写 |
| FD BIO | BIO_TYPE_FD | 文件描述符读写 |
| NULL BIO | BIO_TYPE_NULL | 空操作（类似/dev/null） |

#### 过滤器BIO（Filter）

这些BIO位于源/接收器BIO之上，处理数据：

| 类型 | 宏定义 | 说明 |
|------|--------|------|
| 缓冲BIO | BIO_TYPE_BUFFER | 提供缓冲功能 |
| 摘要BIO | BIO_TYPE_MD | 计算消息摘要 |
| 加密BIO | BIO_TYPE_CIPHER | 加密/解密数据 |
| Base64 BIO | BIO_TYPE_BASE64 | Base64编解码 |
| SSL BIO | BIO_TYPE_SSL | SSL/TLS加密 |

---

### 🚀 基本BIO操作

#### 1. 创建BIO

```c
// 创建内存BIO
BIO *bio = BIO_new(BIO_s_mem());

// 创建文件BIO（只读）
BIO *bio = BIO_new_file("filename.txt", "r");

// 创建Socket BIO
BIO *bio = BIO_new_socket(sockfd, BIO_NOCLOSE);
```

#### 2. 写数据

```c
// 写数据
int written = BIO_write(bio, data, len);

// 写字符串
int written = BIO_puts(bio, "Hello World");

// 刷新缓冲区
BIO_flush(bio);
```

#### 3. 读数据

```c
// 读数据
char buf[1024];
int read = BIO_read(bio, buf, sizeof(buf));

// 读一行
char buf[1024];
char *result = BIO_gets(bio, buf, sizeof(buf));
```

#### 4. 销毁BIO

```c
// 释放一个BIO
BIO_free(bio);

// 释放整个BIO链
BIO_free_all(bio);
```

---

### 💻 C++面向对象封装

```cpp
#include <openssl/bio.h>
#include <string>
#include <stdexcept>

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
    void push(Bio& other) { BIO_push(bio_, other.bio_); }
    BIO* next() const { return BIO_next(bio_); }
    unsigned long num_read() const { return bio_->num_read; }
    unsigned long num_write() const { return bio_->num_write; }
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
```

---

## 第二章：对称加密算法

### 📖 概述

对称加密算法使用相同的密钥进行加密和解密。OpenSSL支持多种对称加密算法：AES、DES、3DES、ChaCha20等。

**特点**：
- 速度快
- 适合大量数据
- 需要安全的密钥交换

---

### 🔐 常用对称加密算法

| 算法 | 密钥长度 | 块大小 | 速度 | 安全性 |
|------|---------|--------|------|--------|
| AES-128 | 128位 | 128位 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| AES-256 | 256位 | 128位 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| ChaCha20 | 256位 | 流 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| 3DES | 168位 | 64位 | ⭐⭐ | ⭐⭐ |

---

### 💻 AES-256 CBC C++封装

```cpp
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <vector>
#include <stdexcept>
#include <cstring>

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
```

---

## 第三章：非对称加密算法

### 📖 概述

非对称加密使用一对密钥：公钥和私钥。公钥加密，私钥解密。

**特点**：
- 无需安全通道交换密钥
- 速度慢
- 适合小量数据或密钥交换

---

### 🔐 常用非对称加密算法

| 算法 | 密钥长度 | 速度 | 安全性 |
|------|---------|------|--------|
| RSA-2048 | 2048位 | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| RSA-4096 | 4096位 | ⭐ | ⭐⭐⭐⭐⭐ |
| ECC (Curve25519) | 256位 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |

---

### 💻 RSA C++封装

```cpp
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <vector>
#include <stdexcept>
#include <cstring>

class RSAWrapper {
private:
    RSA* rsa_;

public:
    RSAWrapper(int bits = 2048) {
        rsa_ = RSA_generate_key(bits, RSA_F4, nullptr, nullptr);
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
```

---

## 第四章：摘要算法与HMAC

### 📖 概述

摘要算法（Hash）将任意长度的数据转换为固定长度的指纹。HMAC使用密钥生成带认证的摘要。

---

### 🔐 常用摘要算法

| 算法 | 输出长度 | 速度 | 安全性 |
|------|---------|------|--------|
| MD5 | 128位 | ⭐⭐⭐⭐⭐ | ⭐ |
| SHA-1 | 160位 | ⭐⭐⭐⭐ | ⭐⭐ |
| SHA-256 | 256位 | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| SHA-512 | 512位 | ⭐⭐ | ⭐⭐⭐⭐⭐ |

---

### 💻 SHA-256 C++封装

```cpp
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <vector>
#include <string>
#include <cstring>

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

class HMAC {
public:
    static std::vector<unsigned char> sha256(
        const std::vector<unsigned char>& key,
        const std::vector<unsigned char>& data
    ) {
        std::vector<unsigned char> hmac(SHA256_DIGEST_LENGTH);
        unsigned int len;
        HMAC(
            EVP_sha256(),
            key.data(), key.size(),
            data.data(), data.size(),
            hmac.data(), &len
        );
        return hmac;
    }
};
```

---

## 第五章：SSL/TLS网络编程

### 📖 概述

SSL/TLS提供安全的网络通信。OpenSSL的SSL库实现了TLS/SSL协议。

---

### 💻 TLS客户端C++封装

```cpp
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <string>
#include <stdexcept>

class TLSClient {
private:
    SSL_CTX* ctx_;
    BIO* bio_;

public:
    TLSClient() {
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        ctx_ = SSL_CTX_new(TLS_client_method());
        if (!ctx_) {
            throw std::runtime_error("SSL_CTX_new failed");
        }

        SSL_CTX_set_verify(ctx_, SSL_VERIFY_NONE, nullptr);
    }

    ~TLSClient() {
        if (bio_) BIO_free_all(bio_);
        if (ctx_) SSL_CTX_free(ctx_);
    }

    void connect(const std::string& host, int port) {
        bio_ = BIO_new_ssl_connect(ctx_);
        if (!bio_) {
            throw std::runtime_error("BIO_new_ssl_connect failed");
        }

        std::string host_port = host + ":" + std::to_string(port);
        BIO_set_conn_hostname(bio_, host_port.c_str());

        if (BIO_do_connect(bio_) <= 0) {
            throw std::runtime_error("BIO_do_connect failed");
        }

        if (BIO_do_handshake(bio_) <= 0) {
            throw std::runtime_error("BIO_do_handshake failed");
        }
    }

    int write(const std::string& data) {
        return BIO_write(bio_, data.c_str(), data.size());
    }

    std::string read(int len = 4096) {
        std::string result(len, '\0');
        int n = BIO_read(bio_, &result[0], len);
        if (n > 0) result.resize(n);
        else result.clear();
        return result;
    }
};
```

---

## 第六章：勒索软件加密方式对比

### 📋 各勒索组织使用的加密方式

#### 1. LockBit 3.0 / 4.0 / 5.0

**加密方案：AES-256 + RSA-2048（混合加密）**

**特点**：
- 使用**间歇加密**：每512字节加密16字节
- 速度极快，不易被检测

**性能**：
- 加密速度：⭐⭐⭐⭐⭐ (极快)
- 检测难度：⭐⭐⭐⭐⭐ (很难检测)

---

#### 2. RansomHub

**加密方案：AES-256 + Curve25519（ECC）**

**特点**：
- 使用椭圆曲线加密（ECC）替代RSA
- 密钥更小，安全性相同

---

#### 3. Conti

**加密方案：ChaCha20 + RSA-4096**

**特点**：
- 使用流密码ChaCha20
- 完全加密（不使用间歇加密）

---

### 📊 加密算法对比

| 算法 | 密钥长度 | 速度 | 安全性 | 勒索组织使用 |
|------|---------|------|--------|-------------|
| AES-256 | 256位 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | LockBit, RansomHub |
| ChaCha20 | 256位 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | Conti, Babuk |
| RSA-2048 | 2048位 | ⭐⭐ | ⭐⭐⭐⭐⭐ | 几乎所有 |
| Curve25519 | 256位 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Babuk, RansomHub |

---

## 完整示例代码

见单独的 `OpenSSL_C++_完整教程.cpp` 文件。

---

*教程完*
