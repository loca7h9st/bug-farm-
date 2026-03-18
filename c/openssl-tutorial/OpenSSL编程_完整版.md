# OpenSSL 编程 - 完整教程

> 完全仿造《openssl编程.pdf》结构
>
> 每章格式：概述 → 数据结构 → 主要函数 → C++面向对象示例
>
> OpenSSL版本：3.6.0
>
> 作者：牛东东🐮
>
> 日期：2026-03-14

---

## 目录

1. [基础知识](#第一章-基础知识)
2. [openssl简介](#第二章-openssl简介)
3. [堆栈](#第三章-堆栈)
4. [哈希表](#第四章-哈希表)
5. [内存分配](#第五章-内存分配)
6. [动态模块加载](#第六章-动态模块加载)
7. [抽象IO (BIO)](#第七章-抽象io-bio) ⭐
8. [配置文件](#第八章-配置文件)
9. [随机数](#第九章-随机数)
10. [文本数据库](#第十章-文本数据库)
11. [大数](#第十一章-大数)
12. [BASE64编解码](#第十二章-base64编解码)
13. [ASN1库](#第十三章-asn1库)
14. [错误处理](#第十四章-错误处理)
15. [摘要与HMAC](#第十五章-摘要与hmac) ⭐
16. [数据压缩](#第十六章-数据压缩)
17. [RSA](#第十七章-rsa) ⭐
18. [DSA](#第十八章-dsa)
19. [DH](#第十九章-dh)
20. [椭圆曲线](#第二十章-椭圆曲线)
21. [EVP](#第二十一章-evp) ⭐⭐⭐（超级重点！）
22. [PEM格式](#第二十二章-pem格式)
23. [Engine](#第二十三章-engine)
24. [通用数据结构](#第二十四章-通用数据结构)
25. [证书申请](#第二十五章-证书申请)
26. [X509数字证书](#第二十六章-x509数字证书)
27. [OCSP](#第二十七章-ocsp)
28. [CRL](#第二十八章-crl)
29. [PKCS7](#第二十九章-pkcs7)
30. [PKCS12](#第三十章-pkcs12)
31. [SSL实现](#第三十一章-ssl实现) ⭐
32. [Openssl命令](#第三十二章-openssl命令)

---

---

## 第一章 基础知识

### 1.1 概述（是干什么的）

本章介绍密码学的基础知识，包括：
- **对称算法**：使用同一个密钥加密和解密
- **摘要算法**：生成固定长度的哈希值，用于完整性验证
- **公钥算法**：使用公钥加密、私钥解密（或反之）
- **回调函数**：OpenSSL中大量使用的编程模式

### 1.2 对称算法

对称算法使用一个密钥。给定一个明文和一个密钥，加密产生密文，其长度和明文大致相同。解密时，使用的密钥与加密密钥相同。

#### 四种加密模式：

| 模式 | 名称 | 特点 |
|------|------|------|
| ECB | 电子密码本模式 | 简单，相同明文产生相同密文 |
| CBC | 加密块链模式 | 使用初始化向量，安全性更好 |
| CFB | 加密反馈模式 | 流加密，可面向字符 |
| OFB | 输出反馈模式 | 类似CFB，但反馈内容不同 |

### 1.3 摘要算法

摘要算法是一种能产生特殊输出格式的算法，特点是：
- 无论输入什么长度，输出都是固定长度
- 输入稍有改变，输出完全不同
- 不可逆，无法从摘要还原原数据

常用摘要算法：
- MD5：16字节输出
- SHA-1：20字节输出
- SHA-256：32字节输出

### 1.4 公钥算法

公钥密码系统中，加密和解密使用不同的密钥。

主要公钥算法：

| 算法 | 用途 | 安全性基础 |
|------|------|-----------|
| RSA | 加密/签名 | 大整数素因子分解 |
| DSA | 仅签名 | 离散对数 |
| DH | 密钥交换 | 离散对数 |
| ECC | 加密/签名 | 椭圆曲线离散对数 |

### 1.5 回调函数

OpenSSL中大量用到了回调函数。回调函数一般定义在数据结构中，是一个函数指针。

回调流程：
1. 用户设置回调函数
2. 用户调用OpenSSL函数
3. OpenSSL函数回调用户提供的函数

如果用户不设置回调函数，OpenSSL会使用默认函数。

---

## 第二章 openssl简介

### 2.1 概述（是干什么的）

OpenSSL是一个功能丰富且自包含的开源安全工具箱。

主要功能：
- SSL/TLS协议实现（SSLv2、SSLv3、TLSv1）
- 大量软算法（对称/非对称/摘要）
- 大数运算
- 非对称算法密钥生成
- ASN.1编解码
- 证书请求编解码
- 数字证书编解码
- CRL编解码
- OCSP协议
- 数字证书验证
- PKCS7/PKCS12标准实现

### 2.2 数据结构

OpenSSL主要源码结构：

```
crypto/        # 基础库（eay库）
├── asn1/      # ASN.1 DER编码解码
├── bio/       # 抽象IO
├── bn/        # 大数运算
├── buffer/    # 字符缓存
├── conf/      # 配置文件
├── dso/       # 动态库加载
├── engine/    # 硬件引擎
├── err/       # 错误处理
├── evp/       # 算法封装（重点！）
├── hmac/      # HMAC
├── lhash/     # 哈希表
├── ocsp/      # OCSP协议
├── pem/       # PEM格式
├── pkcs7/     # PKCS7
├── pkcs12/    # PKCS12
├── rsa/       # RSA
├── dsa/       # DSA
├── dh/        # DH
├── ec/        # 椭圆曲线
└── ...

ssl/           # SSL/TLS协议实现
apps/          # 命令行工具
```

---

## 第三章 抽象IO (BIO) ⭐

### 3.1 概述（是干什么的）

BIO（Basic Input/Output）是OpenSSL的抽象IO层，对各种输入输出进行统一封装。

**作用**：
- 统一文件、内存、socket、SSL等IO操作
- 支持BIO链（可以组合多个BIO）
- 方便进行过滤操作（如加密、摘要）

**BIO类型**：
- mem BIO：内存IO
- file BIO：文件IO
- socket BIO：网络IO
- md BIO：摘要过滤
- cipher BIO：加密过滤
- ssl BIO：SSL/TLS过滤

### 3.2 数据结构

#### BIO 数据结构

```c
struct bio_st {
    const BIO_METHOD *method;  // BIO方法（决定类型）
    BIO *next;                  // 下一个BIO（用于BIO链）
    BIO *prev;                  // 前一个BIO
    int flags;                  // 标志位
    int shutdown;               // 关闭标志
    int (*callback)(...);       // 回调函数
    char *cb_arg;               // 回调参数
    int init;                   // 初始化标志
    int num;                    // 应用自定义数值
    void *ptr;                  // 应用自定义指针
};
```

#### BIO_METHOD 数据结构

```c
struct bio_method_st {
    int type;                   // BIO类型
    const char *name;           // 名称
    int (*bwrite)(...);        // 写函数
    int (*bread)(...);         // 读函数
    int (*bputs)(...);         // 写字符串
    int (*bgets)(...);         // 读字符串
    long (*ctrl)(...);         // 控制函数
    int (*create)(...);        // 创建函数
    int (*destroy)(...);       // 销毁函数
    long (*callback_ctrl)(...); // 回调控制
};
```

### 3.3 主要函数

| 函数 | 作用 |
|------|------|
| `BIO_new(BIO_METHOD *)` | 创建新的BIO |
| `BIO_free(BIO *)` | 释放BIO |
| `BIO_read(BIO *, void *, int)` | 读取数据 |
| `BIO_write(BIO *, const void *, int)` | 写入数据 |
| `BIO_gets(BIO *, char *, int)` | 读取一行 |
| `BIO_puts(BIO *, const char *)` | 写入字符串 |
| `BIO_push(BIO *, BIO *)` | 将BIO加入链 |
| `BIO_pop(BIO *)` | 从链中移除BIO |
| `BIO_s_mem()` | 返回内存BIO方法 |
| `BIO_s_file()` | 返回文件BIO方法 |
| `BIO_s_socket()` | 返回socket BIO方法 |
| `BIO_f_md()` | 返回摘要过滤BIO方法 |
| `BIO_f_cipher()` | 返回加密过滤BIO方法 |
| `BIO_f_ssl()` | 返回SSL过滤BIO方法 |

### 3.4 面向对象编程示例

#### 3.4.1 Mem BIO - 内存IO

```cpp
#include <openssl/bio.h>
#include <string>
#include <stdexcept>

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

// 使用示例
int main() {
    MemBIO bio;
    bio.write("Hello, Mem BIO!");
    std::string data = bio.read();
    return 0;
}
```

#### 3.4.2 File BIO - 文件IO

```cpp
class FileBIO {
private:
    BIO* bio_;

public:
    FileBIO(const std::string& filename, const char* mode = "r") {
        bio_ = BIO_new_file(filename.c_str(), mode);
        if (!bio_) throw std::runtime_error("BIO_new_file failed");
    }

    ~FileBIO() {
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
};
```

#### 3.4.3 MD BIO - 摘要过滤

```cpp
#include <openssl/evp.h>

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
        std::vector<unsigned char> md(EVP_MAX_MD_SIZE);
        unsigned int md_len;
        BIO_get_md(md_bio_, md.data(), &md_len);
        md.resize(md_len);
        return md;
    }
};
```

---

## 第四章 摘要与HMAC ⭐

### 4.1 概述（是干什么的）

摘要算法用于生成固定长度的哈希值，用于：
- 数据完整性验证
- 数字签名
- 密码存储

HMAC（Hash-based Message Authentication Code）是基于摘要算法的消息认证码，用于：
- 消息认证
- 完整性验证

### 4.2 数据结构

#### EVP_MD 数据结构

```c
struct evp_md_st {
    int type;                   // 算法类型（NID_sha256等）
    int pkey_type;              // 对应的公钥算法类型
    int md_size;                // 摘要长度
    int flags;                  // 标志位
    int (*init)(...);          // 初始化函数
    int (*update)(...);        // 更新函数
    int (*final)(...);         // 结束函数
    ...
};
```

#### EVP_MD_CTX 数据结构

```c
struct evp_md_ctx_st {
    const EVP_MD *digest;      // 使用的摘要算法
    ENGINE *engine;             // 硬件引擎
    unsigned long flags;        // 标志位
    void *md_data;              // 算法私有数据
    ...
};
```

### 4.3 主要函数

#### 摘要函数

| 函数 | 作用 |
|------|------|
| `EVP_sha256()` | 返回SHA-256算法 |
| `EVP_sha1()` | 返回SHA-1算法 |
| `EVP_md5()` | 返回MD5算法 |
| `EVP_MD_CTX_new()` | 创建摘要上下文 |
| `EVP_MD_CTX_free(EVP_MD_CTX *)` | 释放摘要上下文 |
| `EVP_DigestInit_ex(EVP_MD_CTX *, EVP_MD *, ENGINE *)` | 初始化摘要 |
| `EVP_DigestUpdate(EVP_MD_CTX *, const void *, size_t)` | 更新摘要 |
| `EVP_DigestFinal_ex(EVP_MD_CTX *, unsigned char *, unsigned int *)` | 完成摘要 |

#### HMAC函数

| 函数 | 作用 |
|------|------|
| `HMAC(EVP_MD *, key, keylen, data, datalen, md, mdlen)` | 计算HMAC |

### 4.4 面向对象编程示例

#### 4.4.1 摘要类

```cpp
#include <openssl/evp.h>
#include <vector>
#include <stdexcept>

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
```

#### 4.4.2 HMAC类

```cpp
#include <openssl/hmac.h>

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
```

---

## 第五章 RSA ⭐

### 5.1 概述（是干什么的）

RSA是最著名的公钥算法，用于：
- 数据加密
- 数字签名
- 密钥交换

**原理**：基于大整数素因子分解的困难性

**密钥长度**：推荐2048位或更长

### 5.2 数据结构

#### RSA 数据结构

```c
struct rsa_st {
    const RSA_METHOD *meth;    // RSA方法
    ENGINE *engine;             // 硬件引擎
    BIGNUM *n;                  // 模数 n = p*q
    BIGNUM *e;                  // 公钥指数 e
    BIGNUM *d;                  // 私钥指数 d
    BIGNUM *p;                  // 素数 p
    BIGNUM *q;                  // 素数 q
    BIGNUM *dmp1;               // d mod (p-1)
    BIGNUM *dmq1;               // d mod (q-1)
    BIGNUM *iqmp;               // q^-1 mod p
    ...
};
```

### 5.3 主要函数

| 函数 | 作用 |
|------|------|
| `RSA_new()` | 创建RSA对象 |
| `RSA_free(RSA *)` | 释放RSA对象 |
| `RSA_generate_key_ex(RSA *, bits, e, cb)` | 生成RSA密钥对 |
| `RSA_public_encrypt(len, from, to, rsa, padding)` | 公钥加密 |
| `RSA_private_decrypt(len, from, to, rsa, padding)` | 私钥解密 |
| `RSA_size(const RSA *)` | 返回模数大小 |

### 5.4 面向对象编程示例

```cpp
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <vector>
#include <stdexcept>

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
};
```

---

## 第六章 EVP ⭐⭐⭐（超级重点！）

### 6.1 概述（是干什么的）

EVP（EnVeloPe）是OpenSSL的高级加密接口，对所有密码算法进行统一封装。

**作用**：
- 统一对称加密、非对称加密、摘要算法接口
- 简化密码学操作
- 方便算法切换（换算法只需改一行代码）

**EVP包含**：
- EVP_MD：摘要算法
- EVP_CIPHER：对称加密算法
- EVP_PKEY：非对称加密算法

### 6.2 数据结构

#### EVP_PKEY 数据结构

```c
struct evp_pkey_st {
    int type;                   // 密钥类型
    int save_type;              // 保存类型
    int references;             // 引用计数
    union {                     // 密钥数据
        RSA *rsa;
        DSA *dsa;
        DH *dh;
        EC_KEY *ec;
        ...
    } pkey;
    ...
};
```

#### EVP_CIPHER 数据结构

```c
struct evp_cipher_st {
    int nid;                    // 算法ID
    int block_size;             // 块大小
    int key_len;                // 密钥长度
    int iv_len;                 // IV长度
    unsigned long flags;        // 标志位
    int (*init)(...);          // 初始化函数
    int (*do_cipher)(...);     // 加密/解密函数
    ...
};
```

#### EVP_CIPHER_CTX 数据结构

```c
struct evp_cipher_ctx_st {
    const EVP_CIPHER *cipher;  // 使用的加密算法
    ENGINE *engine;             // 硬件引擎
    unsigned char *iv;          // 初始化向量
    int key_len;                // 密钥长度
    int iv_len;                 // IV长度
    int encrypt;                // 1=加密，0=解密
    ...
};
```

### 6.3 主要函数

#### EVP对称加密函数

| 函数 | 作用 |
|------|------|
| `EVP_aes_256_cbc()` | 返回AES-256-CBC算法 |
| `EVP_aes_128_cbc()` | 返回AES-128-CBC算法 |
| `EVP_CIPHER_CTX_new()` | 创建加密上下文 |
| `EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *)` | 释放加密上下文 |
| `EVP_EncryptInit_ex(ctx, cipher, e, key, iv)` | 初始化加密 |
| `EVP_EncryptUpdate(ctx, out, outl, in, inl)` | 加密更新 |
| `EVP_EncryptFinal_ex(ctx, out, outl)` | 完成加密 |
| `EVP_DecryptInit_ex(ctx, cipher, e, key, iv)` | 初始化解密 |
| `EVP_DecryptUpdate(ctx, out, outl, in, inl)` | 解密更新 |
| `EVP_DecryptFinal_ex(ctx, out, outl)` | 完成解密 |

### 6.4 面向对象编程示例

#### 6.4.1 EVP对称加密类

```cpp
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <vector>
#include <stdexcept>

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
```

#### 6.4.2 完整测试程序

```cpp
#include <iostream>
#include <string>

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
};

int main() {
    std::cout << "=== EVP AES-256-CBC 测试 ===" << std::endl;

    EVPCipher cipher(EVP_aes_256_cbc());
    std::cout << "密钥: " << Util::to_hex(cipher.get_key()) << std::endl;
    std::cout << "IV:   " << Util::to_hex(cipher.get_iv()) << std::endl;

    std::string plaintext = "Hello, EVP AES-256-CBC!";
    std::cout << "明文: " << plaintext << std::endl;

    std::vector<unsigned char> ciphertext = cipher.encrypt(plaintext);
    std::cout << "密文: " << Util::to_hex(ciphertext) << std::endl;

    std::vector<unsigned char> decrypted = cipher.decrypt(ciphertext);
    std::string decrypted_str(decrypted.begin(), decrypted.end());
    std::cout << "解密: " << decrypted_str << std::endl;

    if (decrypted_str == plaintext) {
        std::cout << "✅ 测试成功！" << std::endl;
    } else {
        std::cout << "❌ 测试失败！" << std::endl;
    }

    return 0;
}
```

---

## 第三十一章 SSL实现 ⭐

### 31.1 概述（是干什么的）

SSL/TLS用于安全网络通信，提供：
- 加密
- 身份认证
- 完整性保护

### 31.2 数据结构（略）

### 31.3 主要函数（略）

### 31.4 面向对象编程示例（略）

---

## 第十一章 大数

### 11.1 概述（是干什么的）

大数运算在密码学中非常重要，用于：
- RSA密钥生成
- DSA签名
- DH密钥交换
- 椭圆曲线运算

OpenSSL的BIGNUM库提供了完整的大数运算功能。

### 11.2 数据结构

#### BIGNUM 数据结构

```c
struct bignum_st {
    BN_ULONG *d;                // 数据数组（低位在前）
    int top;                     // 有效数据的最高位索引
    int dmax;                    // d数组的大小
    int neg;                     // 负数标志（1=负数）
    int flags;                   // 标志位
};
```

### 11.3 主要函数

| 函数 | 作用 |
|------|------|
| `BN_new()` | 创建新的BIGNUM |
| `BN_free(BIGNUM *)` | 释放BIGNUM |
| `BN_set_word(BIGNUM *, BN_ULONG)` | 设置为小整数 |
| `BN_bin2bn(const unsigned char *, int, BIGNUM *)` | 二进制转BIGNUM |
| `BN_bn2bin(const BIGNUM *, unsigned char *)` | BIGNUM转二进制 |
| `BN_add(r, a, b)` | 加法：r = a + b |
| `BN_sub(r, a, b)` | 减法：r = a - b |
| `BN_mul(r, a, b, ctx)` | 乘法：r = a * b |
| `BN_div(r, rem, a, d, ctx)` | 除法：a = d*r + rem |
| `BN_mod_exp(r, a, p, m, ctx)` | 模幂：r = a^p mod m |
| `BN_generate_prime_ex(...)` | 生成素数 |

### 11.4 面向对象编程示例

```cpp
#include <openssl/bn.h>
#include <vector>
#include <stdexcept>

class BigNum {
private:
    BIGNUM* bn_;

public:
    BigNum() {
        bn_ = BN_new();
        if (!bn_) throw std::runtime_error("BN_new failed");
    }

    BigNum(unsigned long w) : BigNum() {
        BN_set_word(bn_, w);
    }

    BigNum(const std::vector<unsigned char>& data) : BigNum() {
        BN_bin2bn(data.data(), data.size(), bn_);
    }

    ~BigNum() {
        if (bn_) BN_free(bn_);
    }

    BigNum add(const BigNum& other) const {
        BigNum result;
        if (!BN_add(result.bn_, bn_, other.bn_))
            throw std::runtime_error("BN_add failed");
        return result;
    }

    BigNum mul(const BigNum& other, BN_CTX* ctx) const {
        BigNum result;
        if (!BN_mul(result.bn_, bn_, other.bn_, ctx))
            throw std::runtime_error("BN_mul failed");
        return result;
    }

    BigNum mod_exp(const BigNum& p, const BigNum& m, BN_CTX* ctx) const {
        BigNum result;
        if (!BN_mod_exp(result.bn_, bn_, p.bn_, m.bn_, ctx))
            throw std::runtime_error("BN_mod_exp failed");
        return result;
    }

    std::vector<unsigned char> to_bin() const {
        int len = BN_num_bytes(bn_);
        std::vector<unsigned char> result(len);
        BN_bn2bin(bn_, result.data());
        return result;
    }

    BIGNUM* get() { return bn_; }
};
```

---

## 第十二章 BASE64编解码

### 12.1 概述（是干什么的）

BASE64是一种用64个字符表示任意二进制数据的编码方式，用于：
- 邮件传输
- URL参数
- 证书和密钥的文本表示

### 12.2 数据结构（略）

### 12.3 主要函数

| 函数 | 作用 |
|------|------|
| `EVP_EncodeInit(EVP_ENCODE_CTX *)` | 初始化编码 |
| `EVP_EncodeUpdate(...)` | 编码更新 |
| `EVP_EncodeFinal(...)` | 完成编码 |
| `EVP_DecodeInit(EVP_ENCODE_CTX *)` | 初始化解码 |
| `EVP_DecodeUpdate(...)` | 解码更新 |
| `EVP_DecodeFinal(...)` | 完成解码 |

### 12.4 面向对象编程示例

```cpp
#include <openssl/evp.h>
#include <vector>
#include <stdexcept>

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
};
```

---

## 第十四章 错误处理

### 14.1 概述（是干什么的）

OpenSSL使用错误堆栈记录错误信息，用于：
- 调试
- 错误诊断
- 用户友好的错误提示

### 14.2 数据结构

```c
struct err_state_st {
    unsigned long err_flags[ERR_NUM_ERRORS];
    const char *err_buffer[ERR_NUM_ERRORS];
    int err_data[ERR_NUM_ERRORS];
    int top;
    int bottom;
    ...
};
```

### 14.3 主要函数

| 函数 | 作用 |
|------|------|
| `ERR_get_error()` | 获取最早的错误码 |
| `ERR_peek_error()` | 查看错误码但不移除 |
| `ERR_error_string(unsigned long, char *)` | 错误码转字符串 |
| `ERR_print_errors_fp(FILE *)` | 打印所有错误到文件 |
| `ERR_print_errors_cb(...)` | 通过回调打印错误 |
| `ERR_clear_error()` | 清除错误堆栈 |

### 14.4 面向对象编程示例

```cpp
#include <openssl/err.h>
#include <string>
#include <vector>
#include <sstream>

class OpenSSLError {
public:
    static std::string get_error_string(unsigned long err) {
        char buf[256];
        ERR_error_string_n(err, buf, sizeof(buf));
        return std::string(buf);
    }

    static std::vector<std::string> get_all_errors() {
        std::vector<std::string> errors;
        unsigned long err;
        while ((err = ERR_get_error()) != 0) {
            errors.push_back(get_error_string(err));
        }
        return errors;
    }

    static std::string get_all_errors_string() {
        std::ostringstream oss;
        unsigned long err;
        while ((err = ERR_get_error()) != 0) {
            oss << get_error_string(err) << std::endl;
        }
        return oss.str();
    }

    static void clear_errors() {
        ERR_clear_error();
    }

    static void print_errors(FILE* fp = stderr) {
        ERR_print_errors_fp(fp);
    }
};
```

---

## 第三十一章 SSL实现 ⭐

### 31.1 概述（是干什么的）

SSL/TLS协议提供安全的网络通信，包括：
- 加密传输
- 身份认证
- 完整性保护

应用场景：
- HTTPS
- 安全邮件
- VPN

### 31.2 数据结构

#### SSL 数据结构

```c
struct ssl_st {
    SSL_CTX *ctx;                  // SSL上下文
    SSL_METHOD *method;            // SSL方法
    BIO *rbio;                     // 读BIO
    BIO *wbio;                     // 写BIO
    int server;                    // 1=服务器，0=客户端
    int state;                     // SSL状态
    ...
};
```

#### SSL_CTX 数据结构

```c
struct ssl_ctx_st {
    const SSL_METHOD *method;      // SSL方法
    X509 *cert;                    // 证书
    EVP_PKEY *key;                 // 私钥
    STACK_OF(X509) *extra_certs;   // 额外证书
    STACK_OF(X509_CRL) *crls;      // CRL
    ...
};
```

### 31.3 主要函数

| 函数 | 作用 |
|------|------|
| `SSL_library_init()` | 初始化SSL库 |
| `SSL_load_error_strings()` | 加载错误字符串 |
| `SSL_CTX_new(SSL_METHOD *)` | 创建SSL上下文 |
| `SSL_CTX_free(SSL_CTX *)` | 释放SSL上下文 |
| `SSL_new(SSL_CTX *)` | 创建SSL对象 |
| `SSL_free(SSL *)` | 释放SSL对象 |
| `SSL_set_fd(SSL *, int)` | 设置文件描述符 |
| `SSL_set_bio(SSL *, BIO *, BIO *)` | 设置BIO |
| `SSL_connect(SSL *)` | 客户端连接 |
| `SSL_accept(SSL *)` | 服务器接受 |
| `SSL_write(SSL *, void *, int)` | 写数据 |
| `SSL_read(SSL *, void *, int)` | 读数据 |
| `SSL_shutdown(SSL *)` | 关闭SSL连接 |

### 31.4 面向对象编程示例

```cpp
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>
#include <stdexcept>

class SSLContext {
private:
    SSL_CTX* ctx_;

public:
    SSLContext(const SSL_METHOD* method) {
        ctx_ = SSL_CTX_new(method);
        if (!ctx_) throw std::runtime_error("SSL_CTX_new failed");
    }

    ~SSLContext() {
        if (ctx_) SSL_CTX_free(ctx_);
    }

    void use_certificate_file(const std::string& file, int type = SSL_FILETYPE_PEM) {
        if (SSL_CTX_use_certificate_file(ctx_, file.c_str(), type) != 1)
            throw std::runtime_error("SSL_CTX_use_certificate_file failed");
    }

    void use_private_key_file(const std::string& file, int type = SSL_FILETYPE_PEM) {
        if (SSL_CTX_use_PrivateKey_file(ctx_, file.c_str(), type) != 1)
            throw std::runtime_error("SSL_CTX_use_PrivateKey_file failed");
    }

    void check_private_key() {
        if (SSL_CTX_check_private_key(ctx_) != 1)
            throw std::runtime_error("SSL_CTX_check_private_key failed");
    }

    SSL_CTX* get() { return ctx_; }
};

class SSLConnection {
private:
    SSL* ssl_;

public:
    SSLConnection(SSLContext& ctx) {
        ssl_ = SSL_new(ctx.get());
        if (!ssl_) throw std::runtime_error("SSL_new failed");
    }

    ~SSLConnection() {
        if (ssl_) {
            SSL_shutdown(ssl_);
            SSL_free(ssl_);
        }
    }

    void set_fd(int fd) {
        SSL_set_fd(ssl_, fd);
    }

    void set_bio(BIO* rbio, BIO* wbio) {
        SSL_set_bio(ssl_, rbio, wbio);
    }

    int connect() {
        return SSL_connect(ssl_);
    }

    int accept() {
        return SSL_accept(ssl_);
    }

    int write(const void* buf, int num) {
        return SSL_write(ssl_, buf, num);
    }

    int write(const std::string& data) {
        return write(data.data(), data.size());
    }

    int read(void* buf, int num) {
        return SSL_read(ssl_, buf, num);
    }

    std::string read(int max_len = 4096) {
        std::vector<char> buf(max_len + 1);
        int len = read(buf.data(), max_len);
        return std::string(buf.data(), len);
    }

    SSL* get() { return ssl_; }
};
```

---

---

## 第三章 堆栈

### 3.1 概述（是干什么的）

堆栈是OpenSSL中广泛使用的数据结构，用于存储和管理对象集合。

主要用途：
- 存储证书链
- 存储CRL列表
- 存储扩展项
- 各种对象列表管理

### 3.2 数据结构

#### _STACK 数据结构

```c
struct stack_st {
    int num;                    // 元素数量
    char **data;               // 数据指针数组
    int sorted;                // 是否已排序
    int num_alloc;             // 已分配的空间
    int (*comp)(const void *, const void *);  // 比较函数
};
```

### 3.3 主要函数

| 函数 | 作用 |
|------|------|
| `sk_new_null()` | 创建空堆栈 |
| `sk_new(cmp_func)` | 创建带比较函数的堆栈 |
| `sk_free(st)` | 释放堆栈 |
| `sk_free_all(st)` | 释放堆栈及所有元素 |
| `sk_num(st)` | 获取元素数量 |
| `sk_value(st, i)` | 获取第i个元素 |
| `sk_set(st, i, val)` | 设置第i个元素 |
| `sk_insert(st, val, i)` | 在位置i插入元素 |
| `sk_push(st, val)` | 在末尾添加元素 |
| `sk_pop(st)` | 弹出末尾元素 |
| `sk_shift(st)` | 弹出第一个元素 |
| `sk_unshift(st, val)` | 在开头插入元素 |
| `sk_delete(st, i)` | 删除第i个元素 |
| `sk_delete_ptr(st, ptr)` | 删除指定指针的元素 |
| `sk_find(st, val)` | 查找元素 |
| `sk_sort(st)` | 排序堆栈 |
| `sk_is_sorted(st)` | 检查是否已排序 |
| `sk_dup(st)` | 复制堆栈 |
| `sk_deep_copy(st, copy_func, free_func)` | 深拷贝堆栈 |

### 3.4 面向对象编程示例

```cpp
#include <openssl/stack.h>
#include <vector>
#include <stdexcept>
#include <functional>

template<typename T>
class Stack {
private:
    STACK_OF(void)* stack_;
    std::function<void(void*)> free_func_;

public:
    Stack() {
        stack_ = sk_new(nullptr);
        if (!stack_) throw std::runtime_error("sk_new failed");
    }

    Stack(std::function<void(void*)> free_func)
        : free_func_(free_func) {
        stack_ = sk_new(nullptr);
        if (!stack_) throw std::runtime_error("sk_new failed");
    }

    ~Stack() {
        if (stack_) {
            if (free_func_) {
                for (int i = 0; i < sk_num(stack_); ++i) {
                    free_func_(sk_value(stack_, i));
                }
            }
            sk_free(stack_);
        }
    }

    int size() const {
        return sk_num(stack_);
    }

    bool empty() const {
        return sk_num(stack_) == 0;
    }

    T* get(int index) const {
        if (index < 0 || index >= sk_num(stack_)) {
            throw std::out_of_range("index out of range");
        }
        return static_cast<T*>(sk_value(stack_, index));
    }

    T* operator[](int index) const {
        return get(index);
    }

    void push(T* val) {
        sk_push(stack_, val);
    }

    T* pop() {
        return static_cast<T*>(sk_pop(stack_));
    }

    T* shift() {
        return static_cast<T*>(sk_shift(stack_));
    }

    void unshift(T* val) {
        sk_unshift(stack_, val);
    }

    void insert(int index, T* val) {
        sk_insert(stack_, val, index);
    }

    void remove(int index) {
        sk_delete(stack_, index);
    }

    void clear() {
        while (!empty()) {
            if (free_func_) {
                T* val = pop();
                free_func_(val);
            } else {
                pop();
            }
        }
    }

    STACK_OF(void)* get() { return stack_; }
};
```

---

## 第四章 哈希表

### 4.1 概述（是干什么的）

哈希表（LHASH）是OpenSSL中的高效数据结构，用于快速查找。

主要用途：
- 存储配置信息
- 存储SSL会话
- 存储ASN1对象
- 各种需要快速查找的场景

### 4.2 数据结构

#### LHASH 数据结构

```c
struct lhash_node_st {
    void *data;
    unsigned long hash;
    struct lhash_node_st *next;
};

struct lhash_st {
    LHASH_NODE **b;
    int num_nodes;
    int num_alloc_nodes;
    int p;
    int pmax;
    unsigned long up_load;
    unsigned long down_load;
    unsigned long num_items;
    unsigned long num_expands;
    unsigned long num_expand_reallocs;
    unsigned long num_contracts;
    unsigned long num_contract_reallocs;
    unsigned long num_hash_calls;
    unsigned long num_comp_calls;
    unsigned long num_insert;
    unsigned long num_replace;
    unsigned long num_delete;
    unsigned long num_no_delete;
    unsigned long num_retrieve;
    unsigned long num_retrieve_miss;
    unsigned long num_hash_comps;
    int error;
};
```

### 4.3 主要函数

| 函数 | 作用 |
|------|------|
| `lh_new(hash_func, comp_func)` | 创建哈希表 |
| `lh_free(lh)` | 释放哈希表 |
| `lh_insert(lh, data)` | 插入数据 |
| `lh_delete(lh, data)` | 删除数据 |
| `lh_retrieve(lh, data)` | 查找数据 |
| `lh_doall(lh, func)` | 遍历所有元素 |
| `lh_doall_arg(lh, func, arg)` | 带参数遍历所有元素 |
| `lh_num_items(lh)` | 获取元素数量 |
| `lh_error(lh)` | 获取错误状态 |

### 4.4 面向对象编程示例

```cpp
#include <openssl/lhash.h>
#include <string>
#include <stdexcept>
#include <functional>

template<typename K, typename V>
class HashTable {
private:
    LHASH* lh_;
    std::function<unsigned long(const void*)> hash_func_;
    std::function<int(const void*, const void*)> comp_func_;
    std::function<void(void*)> free_func_;

public:
    struct Entry {
        K key;
        V value;
    };

    HashTable(
        std::function<unsigned long(const void*)> hash_func,
        std::function<int(const void*, const void*)> comp_func,
        std::function<void(void*)> free_func = nullptr)
        : hash_func_(hash_func), comp_func_(comp_func), free_func_(free_func)
    {
        lh_ = lh_new(
            reinterpret_cast<LHASH_HASH_FUNC>(hash_func_.target<unsigned long(*)(const void*)>()),
            reinterpret_cast<LHASH_COMP_FUNC>(comp_func_.target<int(*)(const void*, const void*)>())
        );
        if (!lh_) throw std::runtime_error("lh_new failed");
    }

    ~HashTable() {
        if (lh_) {
            if (free_func_) {
                // 遍历并释放所有元素
            }
            lh_free(lh_);
        }
    }

    void insert(Entry* entry) {
        lh_insert(lh_, entry);
    }

    Entry* retrieve(Entry* key) {
        return static_cast<Entry*>(lh_retrieve(lh_, key));
    }

    Entry* remove(Entry* key) {
        return static_cast<Entry*>(lh_delete(lh_, key));
    }

    int size() const {
        return lh_num_items(lh_);
    }

    bool empty() const {
        return size() == 0;
    }

    template<typename F>
    void for_each(F func) {
        struct Wrapper {
            F* f;
            static void callback(void* arg) {
                (*static_cast<Wrapper*>(arg)->f)(static_cast<Entry*>(arg));
            }
        };
        // 实际实现需要调整
    }

    LHASH* get() { return lh_; }
};
```

---

## 第五章 内存分配

### 5.1 概述（是干什么的）

OpenSSL提供了自己的内存分配函数，用于：
- 内存跟踪和调试
- 线程安全的内存管理
- 自定义内存分配器

### 5.2 数据结构

```c
// 内存钩子结构
typedef struct mem_st {
    void *(*malloc)(size_t, const char *, int);
    void *(*realloc)(void *, size_t, const char *, int);
    void (*free)(void *, const char *, int);
} MEM;
```

### 5.3 主要函数

| 函数 | 作用 |
|------|------|
| `CRYPTO_malloc(num, file, line)` | 分配内存 |
| `CRYPTO_zalloc(num, file, line)` | 分配并清零内存 |
| `CRYPTO_realloc(ptr, num, file, line)` | 重新分配内存 |
| `CRYPTO_free(ptr, file, line)` | 释放内存 |
| `CRYPTO_set_mem_ex_functions(...)` | 设置内存函数 |
| `CRYPTO_get_mem_ex_functions(...)` | 获取内存函数 |
| `CRYPTO_set_mem_debug(flag)` | 设置内存调试 |
| `CRYPTO_mem_leaks(bio)` | 检查内存泄漏 |

### 5.4 面向对象编程示例

```cpp
#include <openssl/crypto.h>
#include <vector>
#include <stdexcept>

class CryptoMemory {
public:
    static void* malloc(size_t size) {
        void* p = CRYPTO_malloc(size, __FILE__, __LINE__);
        if (!p) throw std::bad_alloc();
        return p;
    }

    static void* zalloc(size_t size) {
        void* p = CRYPTO_zalloc(size, __FILE__, __LINE__);
        if (!p) throw std::bad_alloc();
        return p;
    }

    static void* realloc(void* ptr, size_t size) {
        void* p = CRYPTO_realloc(ptr, size, __FILE__, __LINE__);
        if (!p) throw std::bad_alloc();
        return p;
    }

    static void free(void* ptr) {
        CRYPTO_free(ptr, __FILE__, __LINE__);
    }

    template<typename T>
    static T* new_object() {
        return static_cast<T*>(malloc(sizeof(T)));
    }

    template<typename T>
    static void delete_object(T* ptr) {
        if (ptr) {
            ptr->~T();
            free(ptr);
        }
    }
};

template<typename T>
class CryptoAlloc {
public:
    using value_type = T;

    CryptoAlloc() = default;

    template<typename U>
    CryptoAlloc(const CryptoAlloc<U>&) noexcept {}

    T* allocate(size_t n) {
        return static_cast<T*>(CryptoMemory::malloc(n * sizeof(T)));
    }

    void deallocate(T* p, size_t) noexcept {
        CryptoMemory::free(p);
    }
};

template<typename T, typename U>
bool operator==(const CryptoAlloc<T>&, const CryptoAlloc<U>&) noexcept {
    return true;
}

template<typename T, typename U>
bool operator!=(const CryptoAlloc<T>&, const CryptoAlloc<U>&) noexcept {
    return false;
}
```

---

## 第八章 配置文件

### 8.1 概述（是干什么的）

OpenSSL配置文件用于存储和读取配置信息，默认文件是openssl.cnf。

主要用途：
- 配置默认算法
- 配置CA设置
- 配置扩展项
- 配置引擎

### 8.2 数据结构

```c
struct conf_st {
    LHASH *data;
    char *name;
};
```

### 8.3 主要函数

| 函数 | 作用 |
|------|------|
| `NCONF_new(NULL)` | 创建新配置 |
| `NCONF_free(conf)` | 释放配置 |
| `NCONF_load(conf, file, *err)` | 加载配置文件 |
| `NCONF_load_bio(conf, bio, *err)` | 从BIO加载配置 |
| `NCONF_get_section(conf, name)` | 获取配置节 |
| `NCONF_get_string(conf, sec, name)` | 获取字符串值 |
| `NCONF_get_number(conf, sec, name, *result)` | 获取数值 |
| `NCONF_get1_default_config_file()` | 获取默认配置文件 |

### 8.4 面向对象编程示例

```cpp
#include <openssl/conf.h>
#include <string>
#include <stdexcept>

class Config {
private:
    CONF* conf_;

public:
    Config() {
        conf_ = NCONF_new(nullptr);
        if (!conf_) throw std::runtime_error("NCONF_new failed");
    }

    Config(const std::string& filename) : Config() {
        load(filename);
    }

    ~Config() {
        if (conf_) NCONF_free(conf_);
    }

    void load(const std::string& filename) {
        long errline;
        if (!NCONF_load(conf_, filename.c_str(), &errline)) {
            throw std::runtime_error("NCONF_load failed at line " + std::to_string(errline));
        }
    }

    std::string get_string(const std::string& section,
                          const std::string& name,
                          const std::string& default_value = "") const
    {
        const char* val = NCONF_get_string(conf_, section.c_str(), name.c_str());
        return val ? std::string(val) : default_value;
    }

    long get_number(const std::string& section,
                    const std::string& name,
                    long default_value = 0) const
    {
        long result = default_value;
        NCONF_get_number(conf_, section.c_str(), name.c_str(), &result);
        return result;
    }

    bool has_section(const std::string& section) const {
        return NCONF_get_section(conf_, section.c_str()) != nullptr;
    }

    CONF* get() { return conf_; }
};
```

---

## 第九章 随机数

### 9.1 概述（是干什么的）

随机数在密码学中至关重要，用于：
- 生成密钥
- 生成IV
- 生成nonce
- 各种安全相关的随机数需求

### 9.2 数据结构（略）

### 9.3 主要函数

| 函数 | 作用 |
|------|------|
| `RAND_bytes(buf, num)` | 生成加密安全的随机数 |
| `RAND_priv_bytes(buf, num)` | 生成私钥用随机数 |
| `RAND_pseudo_bytes(buf, num)` | 生成伪随机数 |
| `RAND_seed(buf, num)` | 给随机数生成器添加种子 |
| `RAND_add(buf, num, entropy)` | 添加熵 |
| `RAND_status()` | 检查随机数生成器状态 |
| `RAND_cleanup()` | 清理随机数生成器 |
| `RAND_set_rand_method(method)` | 设置随机数方法 |
| `RAND_get_rand_method()` | 获取随机数方法 |

### 9.4 面向对象编程示例

```cpp
#include <openssl/rand.h>
#include <vector>
#include <stdexcept>
#include <string>

class Random {
public:
    static bool is_ready() {
        return RAND_status() == 1;
    }

    static std::vector<unsigned char> bytes(size_t num) {
        std::vector<unsigned char> result(num);
        if (RAND_bytes(result.data(), num) != 1) {
            throw std::runtime_error("RAND_bytes failed");
        }
        return result;
    }

    static std::vector<unsigned char> priv_bytes(size_t num) {
        std::vector<unsigned char> result(num);
        if (RAND_priv_bytes(result.data(), num) != 1) {
            throw std::runtime_error("RAND_priv_bytes failed");
        }
        return result;
    }

    static void seed(const std::vector<unsigned char>& data) {
        RAND_seed(data.data(), data.size());
    }

    static void add(const std::vector<unsigned char>& data, double entropy) {
        RAND_add(data.data(), data.size(), entropy);
    }

    static void cleanup() {
        RAND_cleanup();
    }

    template<typename T>
    static T random() {
        T result;
        if (RAND_bytes(reinterpret_cast<unsigned char*>(&result), sizeof(T)) != 1) {
            throw std::runtime_error("RAND_bytes failed");
        }
        return result;
    }

    static int random_int(int min, int max) {
        if (min >= max) throw std::invalid_argument("min must be less than max");
        unsigned int range = static_cast<unsigned int>(max - min);
        unsigned int r = random<unsigned int>();
        return min + static_cast<int>(r % (range + 1));
    }
};
```

---

## 第十八章 DSA

### 18.1 概述（是干什么的）

DSA（Digital Signature Algorithm）是数字签名算法，仅用于签名，不能用于加密。

主要用途：
- 数字签名
- 身份认证
- 数据完整性验证

安全性基于：离散对数问题

### 18.2 数据结构

```c
struct dsa_st {
    BIGNUM *p;                  // 大素数 p
    BIGNUM *q;                  // p-1的素因子 q
    BIGNUM *g;                  // 生成元 g
    BIGNUM *priv_key;           // 私钥 x
    BIGNUM *pub_key;            // 公钥 y = g^x mod p
    ...
};
```

### 18.3 主要函数

| 函数 | 作用 |
|------|------|
| `DSA_new()` | 创建DSA对象 |
| `DSA_free(DSA *)` | 释放DSA对象 |
| `DSA_generate_parameters_ex(...)` | 生成DSA参数 |
| `DSA_generate_key(DSA *)` | 生成密钥对 |
| `DSA_sign(type, dgst, len, sig, siglen, dsa)` | 签名 |
| `DSA_verify(type, dgst, len, sig, siglen, dsa)` | 验证签名 |
| `DSA_size(const DSA *)` | 获取签名大小 |

### 18.4 面向对象编程示例

```cpp
#include <openssl/dsa.h>
#include <vector>
#include <stdexcept>

class DSAWrapper {
private:
    DSA* dsa_;

public:
    DSAWrapper(int bits = 1024) {
        dsa_ = DSA_new();
        if (!dsa_) throw std::runtime_error("DSA_new failed");

        DSA_generate_parameters_ex(dsa_, bits, nullptr, 0, nullptr, nullptr, nullptr);
        DSA_generate_key(dsa_);
    }

    ~DSAWrapper() {
        if (dsa_) DSA_free(dsa_);
    }

    std::vector<unsigned char> sign(const std::vector<unsigned char>& digest) {
        std::vector<unsigned char> sig(DSA_size(dsa_));
        unsigned int sig_len;

        if (!DSA_sign(0, digest.data(), digest.size(),
                      sig.data(), &sig_len, dsa_)) {
            throw std::runtime_error("DSA_sign failed");
        }

        sig.resize(sig_len);
        return sig;
    }

    bool verify(const std::vector<unsigned char>& digest,
                const std::vector<unsigned char>& sig)
    {
        return DSA_verify(0, digest.data(), digest.size(),
                         sig.data(), sig.size(), dsa_) == 1;
    }

    int bits() const {
        return DSA_bits(dsa_);
    }

    DSA* get() { return dsa_; }
};
```

---

## 第十九章 DH

### 19.1 概述（是干什么的）

DH（Diffie-Hellman）密钥交换算法，用于双方在不安全信道上协商共享密钥。

主要用途：
- 密钥交换
- TLS/SSL密钥协商
- VPN密钥建立

安全性基于：有限域上的离散对数问题

### 19.2 数据结构

```c
struct dh_st {
    BIGNUM *p;                  // 大素数 p
    BIGNUM *g;                  // 生成元 g
    BIGNUM *priv_key;           // 私钥
    BIGNUM *pub_key;            // 公钥
    ...
};
```

### 19.3 主要函数

| 函数 | 作用 |
|------|------|
| `DH_new()` | 创建DH对象 |
| `DH_free(DH *)` | 释放DH对象 |
| `DH_generate_parameters_ex(...)` | 生成DH参数 |
| `DH_generate_key(DH *)` | 生成密钥对 |
| `DH_compute_key(key, pub_key, dh)` | 计算共享密钥 |
| `DH_size(const DH *)` | 获取密钥大小 |

### 19.4 面向对象编程示例

```cpp
#include <openssl/dh.h>
#include <vector>
#include <stdexcept>

class DHWrapper {
private:
    DH* dh_;

public:
    DHWrapper(int bits = 1024) {
        dh_ = DH_new();
        if (!dh_) throw std::runtime_error("DH_new failed");

        DH_generate_parameters_ex(dh_, bits, DH_GENERATOR_2, nullptr, nullptr, nullptr);
        DH_generate_key(dh_);
    }

    ~DHWrapper() {
        if (dh_) DH_free(dh_);
    }

    std::vector<unsigned char> get_public_key() const {
        int len = BN_num_bytes(dh_->pub_key);
        std::vector<unsigned char> result(len);
        BN_bn2bin(dh_->pub_key, result.data());
        return result;
    }

    std::vector<unsigned char> compute_key(const std::vector<unsigned char>& peer_pub_key) {
        BIGNUM* peer_pub = BN_bin2bn(peer_pub_key.data(), peer_pub_key.size(), nullptr);
        if (!peer_pub) throw std::runtime_error("BN_bin2bn failed");

        std::vector<unsigned char> key(DH_size(dh_));
        int len = DH_compute_key(key.data(), peer_pub, dh_);

        BN_free(peer_pub);

        if (len == -1) throw std::runtime_error("DH_compute_key failed");
        key.resize(len);
        return key;
    }

    int bits() const {
        return DH_bits(dh_);
    }

    DH* get() { return dh_; }
};
```

---

## 第二十章 椭圆曲线

### 20.1 概述（是干什么的）

ECC（Elliptic Curve Cryptography）椭圆曲线密码体制，用于：
- 加密
- 签名
- 密钥交换

相比RSA的优点：
- 更短的密钥长度（256位ECC ≈ 3072位RSA）
- 更快的计算速度
- 更小的存储需求

安全性基于：椭圆曲线离散对数问题

### 20.2 数据结构

```c
struct ec_key_st {
    EC_GROUP *group;            // 椭圆曲线组
    EC_POINT *pub_key;          // 公钥点
    BIGNUM *priv_key;           // 私钥
    ...
};
```

### 20.3 主要函数

| 函数 | 作用 |
|------|------|
| `EC_KEY_new()` | 创建EC_KEY对象 |
| `EC_KEY_free(EC_KEY *)` | 释放EC_KEY对象 |
| `EC_KEY_new_by_curve_name(nid)` | 按曲线名创建 |
| `EC_KEY_generate_key(EC_KEY *)` | 生成密钥对 |
| `ECDSA_sign(type, dgst, len, sig, siglen, eckey)` | ECDSA签名 |
| `ECDSA_verify(type, dgst, len, sig, siglen, eckey)` | ECDSA验证 |
| `ECDH_compute_key(...)` | ECDH密钥交换 |

常用曲线：
- NID_X9_62_prime256v1 (P-256)
- NID_secp384r1 (P-384)
- NID_secp521r1 (P-521)

### 20.4 面向对象编程示例

```cpp
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <vector>
#include <stdexcept>

class ECKey {
private:
    EC_KEY* key_;

public:
    ECKey(int nid = NID_X9_62_prime256v1) {
        key_ = EC_KEY_new_by_curve_name(nid);
        if (!key_) throw std::runtime_error("EC_KEY_new_by_curve_name failed");
        if (!EC_KEY_generate_key(key_)) {
            EC_KEY_free(key_);
            throw std::runtime_error("EC_KEY_generate_key failed");
        }
    }

    ~ECKey() {
        if (key_) EC_KEY_free(key_);
    }

    std::vector<unsigned char> ecdsa_sign(const std::vector<unsigned char>& digest) {
        std::vector<unsigned char> sig(ECDSA_size(key_));
        unsigned int sig_len;

        if (!ECDSA_sign(0, digest.data(), digest.size(),
                       sig.data(), &sig_len, key_)) {
            throw std::runtime_error("ECDSA_sign failed");
        }

        sig.resize(sig_len);
        return sig;
    }

    bool ecdsa_verify(const std::vector<unsigned char>& digest,
                     const std::vector<unsigned char>& sig)
    {
        return ECDSA_verify(0, digest.data(), digest.size(),
                          sig.data(), sig.size(), key_) == 1;
    }

    EC_KEY* get() { return key_; }
};
```

---

## 第二十二章 PEM格式

### 22.1 概述（是干什么的）

PEM（Privacy-Enhanced Mail）是一种Base64编码的格式，用于存储和传输密钥、证书等。

主要用途：
- 存储私钥
- 存储证书
- 存储证书请求
- 存储PKCS7/PKCS12数据

格式特点：
- Base64编码
- 有头部和尾部标记
- 文本格式，便于查看和传输

### 22.2 数据结构（略）

### 22.3 主要函数

| 函数 | 作用 |
|------|------|
| `PEM_write_bio_RSAPrivateKey(...)` | 写RSA私钥 |
| `PEM_read_bio_RSAPrivateKey(...)` | 读RSA私钥 |
| `PEM_write_bio_X509(...)` | 写X509证书 |
| `PEM_read_bio_X509(...)` | 读X509证书 |
| `PEM_write_bio_X509_REQ(...)` | 写证书请求 |
| `PEM_read_bio_X509_REQ(...)` | 读证书请求 |

### 22.4 面向对象编程示例

```cpp
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <string>
#include <stdexcept>

class PEMUtils {
public:
    static void write_rsa_private_key(RSA* rsa, const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "w");
        if (!fp) throw std::runtime_error("fopen failed for writing");

        if (!PEM_write_RSAPrivateKey(fp, rsa, nullptr, nullptr, 0, nullptr, nullptr)) {
            fclose(fp);
            throw std::runtime_error("PEM_write_RSAPrivateKey failed");
        }

        fclose(fp);
    }

    static RSA* read_rsa_private_key(const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "r");
        if (!fp) throw std::runtime_error("fopen failed for reading");

        RSA* rsa = PEM_read_RSAPrivateKey(fp, nullptr, nullptr, nullptr);
        fclose(fp);

        if (!rsa) throw std::runtime_error("PEM_read_RSAPrivateKey failed");
        return rsa;
    }

    static void write_x509(X509* x509, const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "w");
        if (!fp) throw std::runtime_error("fopen failed for writing");

        if (!PEM_write_X509(fp, x509)) {
            fclose(fp);
            throw std::runtime_error("PEM_write_X509 failed");
        }

        fclose(fp);
    }

    static X509* read_x509(const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "r");
        if (!fp) throw std::runtime_error("fopen failed for reading");

        X509* x509 = PEM_read_X509(fp, nullptr, nullptr, nullptr);
        fclose(fp);

        if (!x509) throw std::runtime_error("PEM_read_X509 failed");
        return x509;
    }
};
```

---

## 第二十五章 证书申请

### 25.1 概述（是干什么的）

证书申请（PKCS#10）用于向CA申请数字证书。

主要用途：
- 生成证书请求
- 向CA提交申请
- 包含公钥和身份信息

### 25.2 数据结构

```c
struct X509_req_st {
    X509_REQ_INFO *req_info;    // 请求信息
    X509_ALGOR *sig_alg;        // 签名算法
    ASN1_BIT_STRING *signature;  // 签名
    ...
};
```

### 25.3 主要函数

| 函数 | 作用 |
|------|------|
| `X509_REQ_new()` | 创建证书请求 |
| `X509_REQ_free(X509_REQ *)` | 释放证书请求 |
| `X509_REQ_set_subject_name(...)` | 设置主题名称 |
| `X509_REQ_set_pubkey(...)` | 设置公钥 |
| `X509_REQ_sign(...)` | 签名证书请求 |
| `PEM_write_bio_X509_REQ(...)` | 写PEM格式 |
| `PEM_read_bio_X509_REQ(...)` | 读PEM格式 |

### 25.4 面向对象编程示例

```cpp
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <string>
#include <stdexcept>

class CertRequest {
private:
    X509_REQ* req_;

public:
    CertRequest() {
        req_ = X509_REQ_new();
        if (!req_) throw std::runtime_error("X509_REQ_new failed");
    }

    ~CertRequest() {
        if (req_) X509_REQ_free(req_);
    }

    void set_subject(const std::string& cn,
                    const std::string& org = "",
                    const std::string& country = "")
    {
        X509_NAME* name = X509_REQ_get_subject_name(req_);
        if (!name) throw std::runtime_error("X509_REQ_get_subject_name failed");

        if (!country.empty()) {
            X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC,
                                       (const unsigned char*)country.c_str(), -1, -1, 0);
        }
        if (!org.empty()) {
            X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC,
                                       (const unsigned char*)org.c_str(), -1, -1, 0);
        }
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                                   (const unsigned char*)cn.c_str(), -1, -1, 0);
    }

    void set_public_key(EVP_PKEY* pkey) {
        if (!X509_REQ_set_pubkey(req_, pkey)) {
            throw std::runtime_error("X509_REQ_set_pubkey failed");
        }
    }

    void sign(EVP_PKEY* pkey, const EVP_MD* md = EVP_sha256()) {
        if (!X509_REQ_sign(req_, pkey, md)) {
            throw std::runtime_error("X509_REQ_sign failed");
        }
    }

    void save_pem(const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "w");
        if (!fp) throw std::runtime_error("fopen failed");

        if (!PEM_write_X509_REQ(fp, req_)) {
            fclose(fp);
            throw std::runtime_error("PEM_write_X509_REQ failed");
        }

        fclose(fp);
    }

    X509_REQ* get() { return req_; }
};
```

---

## 第二十六章 X509数字证书

### 26.1 概述（是干什么的）

X509是数字证书标准，用于身份认证。

主要用途：
- HTTPS服务器认证
- 客户端认证
- 代码签名
- 文档签名

### 26.2 数据结构

```c
struct x509_st {
    X509_CINF *cert_info;        // 证书信息
    X509_ALGOR *sig_alg;         // 签名算法
    ASN1_BIT_STRING *signature;   // 签名
    ...
};
```

### 26.3 主要函数

| 函数 | 作用 |
|------|------|
| `X509_new()` | 创建X509证书 |
| `X509_free(X509 *)` | 释放X509证书 |
| `X509_get_subject_name(X509 *)` | 获取主题名称 |
| `X509_get_issuer_name(X509 *)` | 获取颁发者名称 |
| `X509_get_pubkey(X509 *)` | 获取公钥 |
| `X509_verify(X509 *, EVP_PKEY *)` | 验证证书签名 |
| `PEM_write_bio_X509(...)` | 写PEM格式 |
| `PEM_read_bio_X509(...)` | 读PEM格式 |

### 26.4 面向对象编程示例

```cpp
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <string>
#include <stdexcept>
#include <vector>

class X509Cert {
private:
    X509* cert_;

public:
    X509Cert() {
        cert_ = X509_new();
        if (!cert_) throw std::runtime_error("X509_new failed");
    }

    X509Cert(const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "r");
        if (!fp) throw std::runtime_error("fopen failed for reading");

        cert_ = PEM_read_X509(fp, nullptr, nullptr, nullptr);
        fclose(fp);

        if (!cert_) throw std::runtime_error("PEM_read_X509 failed");
    }

    ~X509Cert() {
        if (cert_) X509_free(cert_);
    }

    std::string get_subject_name() const {
        X509_NAME* name = X509_get_subject_name(cert_);
        if (!name) return "";

        char buf[256];
        X509_NAME_oneline(name, buf, sizeof(buf));
        return std::string(buf);
    }

    std::string get_issuer_name() const {
        X509_NAME* name = X509_get_issuer_name(cert_);
        if (!name) return "";

        char buf[256];
        X509_NAME_oneline(name, buf, sizeof(buf));
        return std::string(buf);
    }

    EVP_PKEY* get_public_key() const {
        return X509_get_pubkey(cert_);
    }

    bool verify(EVP_PKEY* pkey) const {
        return X509_verify(cert_, pkey) == 1;
    }

    void save_pem(const std::string& filename) const {
        FILE* fp = fopen(filename.c_str(), "w");
        if (!fp) throw std::runtime_error("fopen failed for writing");

        if (!PEM_write_X509(fp, cert_)) {
            fclose(fp);
            throw std::runtime_error("PEM_write_X509 failed");
        }

        fclose(fp);
    }

    X509* get() { return cert_; }
    const X509* get() const { return cert_; }
};
```

---

## 第六章 动态模块加载

### 6.1 概述（是干什么的）

OpenSSL支持动态加载引擎模块，用于扩展功能。

主要用途：
- 加载硬件加速模块
- 加载自定义加密算法
- 加载第三方安全模块

### 6.2 数据结构（略）

### 6.3 主要函数

| 函数 | 作用 |
|------|------|
| `ENGINE_by_id(id)` | 通过ID获取引擎 |
| `ENGINE_load_builtin_engines()` | 加载内置引擎 |
| `ENGINE_register_all_complete()` | 注册所有引擎 |
| `ENGINE_init(engine)` | 初始化引擎 |
| `ENGINE_finish(engine)` | 结束引擎 |
| `ENGINE_free(engine)` | 释放引擎 |
| `ENGINE_set_default(engine, flags)` | 设置默认引擎 |

### 6.4 面向对象编程示例

```cpp
#include <openssl/engine.h>
#include <string>
#include <stdexcept>

class Engine {
private:
    ENGINE* engine_;

public:
    static void init() {
        ENGINE_load_builtin_engines();
        ENGINE_register_all_complete();
    }

    Engine(const std::string& id) {
        engine_ = ENGINE_by_id(id.c_str());
        if (!engine_) throw std::runtime_error("ENGINE_by_id failed");
        if (!ENGINE_init(engine_)) {
            ENGINE_free(engine_);
            throw std::runtime_error("ENGINE_init failed");
        }
    }

    ~Engine() {
        if (engine_) {
            ENGINE_finish(engine_);
            ENGINE_free(engine_);
        }
    }

    void set_default(int flags = ENGINE_METHOD_ALL) {
        if (!ENGINE_set_default(engine_, flags)) {
            throw std::runtime_error("ENGINE_set_default failed");
        }
    }

    ENGINE* get() { return engine_; }
};
```

---

## 第十章 文本数据库

### 10.1 概述（是干什么的）

OpenSSL提供文本数据库功能，用于存储和查找数据。

主要用途：
- 存储证书撤销列表（CRL）
- 存储用户信息
- 存储配置数据

### 10.2 数据结构（略）

### 10.3 主要函数

| 函数 | 作用 |
|------|------|
| `TXT_DB_new()` | 创建文本数据库 |
| `TXT_DB_read(bio, num)` | 从BIO读取数据库 |
| `TXT_DB_write(bio, db)` | 写入数据库到BIO |
| `TXT_DB_free(db)` | 释放数据库 |
| `TXT_DB_get_by_index(db, idx, value)` | 按索引查找 |
| `TXT_DB_insert(db, row)` | 插入行 |

---

## 第十三章 ASN1库

### 13.1 概述（是干什么的）

ASN1（Abstract Syntax Notation One）是数据表示标准。

主要用途：
- 证书编码
- 私钥编码
- 各种密码学对象的序列化

### 13.2 数据结构（略）

### 13.3 主要函数

| 函数 | 作用 |
|------|------|
| `d2i_X509(...)` | DER解码X509 |
| `i2d_X509(...)` | DER编码X509 |
| `d2i_PrivateKey(...)` | DER解码私钥 |
| `i2d_PrivateKey(...)` | DER编码私钥 |
| `ASN1_INTEGER_new()` | 创建整数 |
| `ASN1_INTEGER_free(a)` | 释放整数 |
| `ASN1_STRING_new()` | 创建字符串 |
| `ASN1_STRING_free(s)` | 释放字符串 |

---

## 第十六章 数据压缩

### 16.1 概述（是干什么的）

OpenSSL提供压缩功能（基于zlib）。

主要用途：
- 压缩数据
- 解压缩数据

### 16.2 数据结构（略）

### 16.3 主要函数

| 函数 | 作用 |
|------|------|
| `COMP_zlib()` | 获取zlib压缩方法 |
| `COMP_compress_block(method, out, outlen, in, inlen)` | 压缩 |
| `COMP_expand_block(method, out, outlen, in, inlen)` | 解压缩 |

---

## 第二十三章 Engine

### 23.1 概述（是干什么的）

Engine是OpenSSL的插件机制，用于扩展加密功能。

主要用途：
- 硬件加密加速
- 自定义加密算法
- 安全模块集成

### 23.2 数据结构

```c
struct engine_st {
    const char *id;                // 引擎ID
    const char *name;              // 引擎名称
    ENGINE_RSA_METHOD *rsa_meth;   // RSA方法
    ENGINE_DSA_METHOD *dsa_meth;   // DSA方法
    ENGINE_ECDSA_METHOD *ecdsa_meth; // ECDSA方法
    ENGINE_DH_METHOD *dh_meth;     // DH方法
    ENGINE_ECDH_METHOD *ecdh_meth; // ECDH方法
    ENGINE_DIGESTS *digests;       // 摘要方法
    ENGINE_CIPHERS *ciphers;       // 加密方法
    ...
};
```

### 23.3 主要函数

| 函数 | 作用 |
|------|------|
| `ENGINE_new()` | 创建新引擎 |
| `ENGINE_free(engine)` | 释放引擎 |
| `ENGINE_by_id(id)` | 通过ID获取引擎 |
| `ENGINE_init(engine)` | 初始化引擎 |
| `ENGINE_finish(engine)` | 结束引擎 |
| `ENGINE_set_id(engine, id)` | 设置引擎ID |
| `ENGINE_set_name(engine, name)` | 设置引擎名称 |
| `ENGINE_set_RSA(engine, rsa_meth)` | 设置RSA方法 |
| `ENGINE_set_default(engine, flags)` | 设置默认引擎 |

---

## 第二十四章 通用数据结构

### 24.1 概述（是干什么的）

OpenSSL提供通用数据结构，用于各种场景。

主要数据结构：
- 链表
- 哈希表
- 堆栈
- 动态数组

### 24.2 主要函数（略）

---

## 第二十七章 OCSP

### 27.1 概述（是干什么的）

OCSP（Online Certificate Status Protocol）用于在线查询证书状态。

主要用途：
- 检查证书是否被撤销
- 实时证书状态验证

### 27.2 数据结构（略）

### 27.3 主要函数

| 函数 | 作用 |
|------|------|
| `OCSP_REQUEST_new()` | 创建OCSP请求 |
| `OCSP_REQUEST_free(req)` | 释放请求 |
| `OCSP_RESPONSE_new()` | 创建OCSP响应 |
| `OCSP_RESPONSE_free(resp)` | 释放响应 |
| `OCSP_cert_status_str(status)` | 获取状态字符串 |

---

## 第二十八章 CRL

### 28.1 概述（是干什么的）

CRL（Certificate Revocation List）证书撤销列表。

主要用途：
- 存储被撤销的证书
- 验证证书有效性

### 28.2 数据结构

```c
struct X509_crl_st {
    X509_CRL_INFO *crl;            // CRL信息
    X509_ALGOR *sig_alg;           // 签名算法
    ASN1_BIT_STRING *signature;    // 签名
    ...
};
```

### 28.3 主要函数

| 函数 | 作用 |
|------|------|
| `X509_CRL_new()` | 创建CRL |
| `X509_CRL_free(crl)` | 释放CRL |
| `PEM_read_X509_CRL(...)` | 读取PEM格式 |
| `PEM_write_X509_CRL(...)` | 写入PEM格式 |
| `X509_CRL_get_issuer(crl)` | 获取颁发者 |
| `X509_CRL_get_lastUpdate(crl)` | 获取上次更新时间 |
| `X509_CRL_get_nextUpdate(crl)` | 获取下次更新时间 |

---

## 第二十九章 PKCS7

### 29.1 概述（是干什么的）

PKCS#7是加密消息语法标准，用于签名和加密数据。

主要用途：
- 数字签名
- 数据加密
- 证书封装

### 29.2 数据结构（略）

### 29.3 主要函数

| 函数 | 作用 |
|------|------|
| `PKCS7_new()` | 创建PKCS7 |
| `PKCS7_free(p7)` | 释放PKCS7 |
| `PKCS7_sign(...)` | 签名 |
| `PKCS7_verify(...)` | 验证签名 |
| `PKCS7_encrypt(...)` | 加密 |
| `PKCS7_decrypt(...)` | 解密 |
| `PEM_read_PKCS7(...)` | 读取PEM格式 |
| `PEM_write_PKCS7(...)` | 写入PEM格式 |

---

## 第三十章 PKCS12

### 30.1 概述（是干什么的）

PKCS#12是个人信息交换标准，用于打包密钥和证书。

主要用途：
- 导出/导入私钥和证书
- 个人证书备份
- 跨平台证书传输

### 30.2 数据结构（略）

### 30.3 主要函数

| 函数 | 作用 |
|------|------|
| `PKCS12_new()` | 创建PKCS12 |
| `PKCS12_free(p12)` | 释放PKCS12 |
| `PKCS12_create(...)` | 创建PKCS12 |
| `PKCS12_parse(...)` | 解析PKCS12 |
| `d2i_PKCS12(...)` | DER解码 |
| `i2d_PKCS12(...)` | DER编码 |
| `PEM_read_PKCS12(...)` | 读取PEM格式 |
| `PEM_write_PKCS12(...)` | 写入PEM格式 |

---

## 第三十二章 Openssl命令

### 32.1 概述（是干什么的）

OpenSSL命令行工具，用于各种密码学操作。

主要命令：
- `openssl genrsa` - 生成RSA密钥
- `openssl req` - 生成证书请求
- `openssl x509` - 证书操作
- `openssl dgst` - 摘要计算
- `openssl enc` - 加密/解密
- `openssl s_client` - SSL客户端
- `openssl s_server` - SSL服务器

### 32.2 常用命令示例

```bash
# 生成RSA私钥
openssl genrsa -out private.key 2048

# 生成证书请求
openssl req -new -key private.key -out req.csr

# 自签名证书
openssl req -x509 -new -key private.key -out cert.pem -days 365

# 计算SHA-256摘要
openssl dgst -sha256 file.txt

# AES-256-CBC加密
openssl enc -aes-256-cbc -salt -in plain.txt -out encrypted.enc

# AES-256-CBC解密
openssl enc -d -aes-256-cbc -in encrypted.enc -out decrypted.txt

# 查看证书信息
openssl x509 -in cert.pem -text -noout

# SSL连接测试
openssl s_client -connect example.com:443
```

---

---

## 附录：示例代码编译说明

### 编译命令

```bash
# EVP对称加密示例
g++ -std=c++11 -o evp_cipher_test evp_cipher_test.cpp -lssl -lcrypto

# RSA示例
g++ -std=c++11 -o rsa_test rsa_test.cpp -lssl -lcrypto

# BIO示例
g++ -std=c++11 -o bio_test bio_test.cpp -lssl -lcrypto
```

---

## 总结

本教程完全按照《openssl编程.pdf》的结构编写，重点章节（第7、15、17、21、31章）都包含：
- 清晰的概述
- 详细的数据结构注释
- 完整的函数表
- 可运行的C++面向对象示例

剩余章节（第3-6、8-14、16、18-20、22-30、32章）框架已搭建，可按需继续填充。

---

*教程版本：v1.0*
*作者：牛东东🐮*
*日期：2026-03-14*
