# OpenSSL 编程 - EVP重点版

> 仿造《openssl编程.pdf》章节结构
>
> 重点：EVP加解密
>
> 作者：牛东东🐮
>
> 日期：2026-03-14

---

## 📋 目录

第一章：基础知识
- 1.1 对称算法
- 1.2 摘要算法
- 1.3 公钥算法
- 1.4 回调函数

第七章：抽象IO (BIO)
- 7.1 数据结构
- 7.2 BIO函数
- 7.3 编程示例

第十五章：摘要与HMAC
- 15.1 概述
- 15.2 数据结构
- 15.3 函数说明
- 15.4 编程示例

第十七章：RSA
- 17.1 数据结构
- 17.2 主要函数
- 17.3 编程示例

**第二十一章：EVP（重点！）**
- 21.1 EVP简介
- 21.2 数据结构（详细）
- 21.3 摘要函数
- 21.4 对称加解密函数
- 21.5 非对称函数
- 21.6 对称加密过程
- 21.7 编程示例（完整）

---

## 第七章：抽象IO (BIO)

### 7.1 数据结构

#### BIO_METHOD - BIO方法表

```c
typedef struct bio_method_st {
    int type;                    // BIO类型
    const char *name;            // BIO名称
    int (*bwrite) (BIO *, const char *, int);  // 写函数
    int (*bread) (BIO *, char *, int);         // 读函数
    int (*bputs) (BIO *, const char *);         // 写字符串
    int (*bgets) (BIO *, char *, int);         // 读行
    long (*ctrl) (BIO *, int, long, void *);    // 控制函数
    int (*create) (BIO *);                      // 创建函数
    int (*destroy) (BIO *);                     // 销毁函数
    long (*callback_ctrl) (BIO *, int, bio_info_cb *);  // 回调控制
} BIO_METHOD;
```

#### BIO - BIO主体结构

```c
struct bio_st {
    BIO_METHOD *method;          // 指向BIO方法表
    long (*callback) (...);      // 回调函数
    char *cb_arg;               // 回调参数
    int init;                   // 初始化标志
    int shutdown;               // 关闭标志
    int flags;                  // 标志位
    int retry_reason;           // 重试原因
    int num;                    // 通用数字
    void *ptr;                  // 通用指针
    struct bio_st *next_bio;    // 下一个BIO
    struct bio_st *prev_bio;    // 上一个BIO
    int references;             // 引用计数
    unsigned long num_read;     // 已读字节数
    unsigned long num_write;    // 已写字节数
    CRYPTO_EX_DATA ex_data;     // 额外数据
};
```

---

### 7.2 BIO常用函数

| 函数 | 作用 |
|------|------|
| `BIO_new(BIO_METHOD *)` | 创建新的BIO |
| `BIO_free(BIO *)` | 释放BIO |
| `BIO_free_all(BIO *)` | 释放整个BIO链 |
| `BIO_write(BIO *, buf, len)` | 写数据 |
| `BIO_read(BIO *, buf, len)` | 读数据 |
| `BIO_puts(BIO *, str)` | 写字符串 |
| `BIO_gets(BIO *, buf, len)` | 读一行 |
| `BIO_flush(BIO *)` | 刷新缓冲区 |
| `BIO_push(BIO *, BIO *)` | 链接BIO |
| `BIO_pop(BIO *)` | 弹出BIO |
| `BIO_s_mem()` | 获取内存BIO方法 |
| `BIO_s_file()` | 获取文件BIO方法 |
| `BIO_s_socket()` | 获取Socket BIO方法 |
| `BIO_new_file(filename, mode)` | 创建文件BIO |
| `BIO_new_mem_buf(data, len)` | 从内存创建BIO |
| `BIO_new_socket(fd, close_flag)` | 创建Socket BIO |

---

### 7.3 编程示例

见 `OpenSSL_C++_完整教程.cpp` 中的MemoryBio类。

---

## 第十五章：摘要与HMAC

### 15.2 数据结构

#### EVP_MD - 摘要方法结构

```c
struct env_md_st {
    int type;                   // 摘要算法NID
    int pkey_type;             // 公钥类型
    int md_size;               // 摘要大小
    unsigned long flags;       // 标志
    int (*init) (EVP_MD_CTX *);      // 初始化
    int (*update) (EVP_MD_CTX *, const void *, size_t);  // 更新
    int (*final) (EVP_MD_CTX *, unsigned char *);  // 完成
    int (*copy) (EVP_MD_CTX *, const EVP_MD_CTX *);  // 复制
    int (*cleanup) (EVP_MD_CTX *);     // 清理
    int block_size;            // 块大小
    int ctx_size;              // 上下文大小
};
```

#### EVP_MD_CTX - 摘要上下文

```c
struct env_md_ctx_st {
    const EVP_MD *digest;      // 摘要方法
    ENGINE *engine;            // 引擎
    unsigned long flags;       // 标志
    void *md_data;             // 摘要数据
    EVP_PKEY_CTX *pctx;       // 公钥上下文
    int (*update) (...);       // 更新函数
};
```

---

### 15.3 常用函数

| 函数 | 作用 |
|------|------|
| `EVP_MD_CTX_new()` | 创建摘要上下文 |
| `EVP_MD_CTX_free(EVP_MD_CTX *)` | 释放上下文 |
| `EVP_DigestInit(EVP_MD_CTX *, EVP_MD *)` | 初始化摘要 |
| `EVP_DigestUpdate(EVP_MD_CTX *, data, len)` | 更新摘要 |
| `EVP_DigestFinal(EVP_MD_CTX *, md, len)` | 完成摘要 |
| `EVP_sha256()` | 获取SHA-256方法 |
| `EVP_sha1()` | 获取SHA-1方法 |
| `EVP_md5()` | 获取MD5方法 |
| `EVP_md_null()` | 获取空摘要方法 |
| `HMAC(EVP_MD *, key, keylen, data, datalen, md, len)` | 计算HMAC |

---

## 第十七章：RSA

### 17.4 数据结构

#### RSA - RSA结构

```c
struct rsa_st {
    int pad;                   // 填充方式
    long version;              // 版本
    const RSA_METHOD *meth;    // RSA方法
    ENGINE *engine;            // 引擎
    BIGNUM *n;                 // 模数
    BIGNUM *e;                 // 公钥指数
    BIGNUM *d;                 // 私钥指数
    BIGNUM *p;                 // 素数p
    BIGNUM *q;                 // 素数q
    BIGNUM *dmp1;              // d mod (p-1)
    BIGNUM *dmq1;              // d mod (q-1)
    BIGNUM *iqmp;              // q^-1 mod p
    int references;            // 引用计数
    int flags;                 // 标志
    CRYPTO_EX_DATA ex_data;    // 额外数据
};
```

---

### 17.5 常用函数

| 函数 | 作用 |
|------|------|
| `RSA_new()` | 创建RSA结构 |
| `RSA_free(RSA *)` | 释放RSA |
| `RSA_generate_key(bits, e, cb, cb_arg)` | 生成RSA密钥 |
| `RSA_generate_key_ex(RSA *, bits, e, cb)` | 生成RSA密钥（新版） |
| `RSA_public_encrypt(len, in, out, rsa, pad)` | 公钥加密 |
| `RSA_private_decrypt(len, in, out, rsa, pad)` | 私钥解密 |
| `RSA_size(RSA *)` | 获取RSA模数字节数 |

---

## 第二十一章：EVP（重点！）

### 21.1 EVP简介

EVP（Envelope）是OpenSSL的高级加密接口，提供了统一的抽象层，可以用相同的方式处理不同的加密算法。

**EVP的优势：**
- 统一接口：对称加密、非对称加密、摘要都用类似的API
- 算法切换方便：只需更换EVP_CIPHER或EVP_MD指针
- 支持多种算法：AES、DES、ChaCha20、RSA、DSA等
- 自动处理填充、IV等细节

---

### 21.2 数据结构（详细）

#### 21.2.1 EVP_PKEY - 公钥/私钥结构

```c
struct evp_pkey_st {
    int type;                   // 密钥类型（EVP_PKEY_RSA等）
    int save_type;             // 保存类型
    int references;            // 引用计数
    const EVP_PKEY_ASN1_METHOD *ameth;  // ASN.1方法
    ENGINE *engine;            // 引擎
    union {
        char *ptr;
        struct rsa_st *rsa;     // RSA密钥
        struct dsa_st *dsa;     // DSA密钥
        struct dh_st *dh;       // DH密钥
        struct ec_key_st *ec;   // ECC密钥
    } pkey;                    // 密钥联合体
    int save_parameters;       // 保存参数标志
    STACK_OF(X509_ATTRIBUTE) *attributes;  // 属性
};
```

**成员说明：**

| 成员 | 作用 |
|------|------|
| `type` | 密钥类型：EVP_PKEY_RSA, EVP_PKEY_DSA, EVP_PKEY_EC等 |
| `references` | 引用计数，用于内存管理 |
| `pkey` | 联合体，根据type存储不同类型的密钥 |
| `pkey.rsa` | RSA密钥（当type=EVP_PKEY_RSA时） |
| `pkey.dsa` | DSA密钥（当type=EVP_PKEY_DSA时） |
| `pkey.ec` | ECC密钥（当type=EVP_PKEY_EC时） |

**EVP_PKEY常用函数：**

| 函数 | 作用 |
|------|------|
| `EVP_PKEY_new()` | 创建新的EVP_PKEY |
| `EVP_PKEY_free(EVP_PKEY *)` | 释放EVP_PKEY |
| `EVP_PKEY_assign_RSA(EVP_PKEY *, RSA *)` | 分配RSA密钥 |
| `EVP_PKEY_get1_RSA(EVP_PKEY *)` | 获取RSA密钥（增加引用） |
| `EVP_PKEY_bits(EVP_PKEY *)` | 获取密钥位数 |
| `EVP_PKEY_size(EVP_PKEY *)` | 获取密钥字节数 |
| `EVP_PKEY_type(EVP_PKEY *)` | 获取密钥类型 |

---

#### 21.2.2 EVP_MD - 摘要方法结构

```c
struct env_md_st {
    int type;                   // 算法NID
    int pkey_type;             // 公钥类型
    int md_size;               // 摘要输出大小（字节）
    unsigned long flags;       // 标志位
    int (*init) (EVP_MD_CTX *);           // 初始化函数
    int (*update) (EVP_MD_CTX *, const void *, size_t);  // 更新函数
    int (*final) (EVP_MD_CTX *, unsigned char *);  // 完成函数
    int (*copy) (EVP_MD_CTX *, const EVP_MD_CTX *);  // 复制函数
    int (*cleanup) (EVP_MD_CTX *);          // 清理函数
    int block_size;            // 块大小
    int ctx_size;              // 上下文大小
};
```

**成员说明：**

| 成员 | 作用 |
|------|------|
| `type` | 算法NID，如NID_sha256, NID_sha1 |
| `md_size` | 摘要输出大小：SHA256=32, SHA1=20, MD5=16 |
| `init` | 初始化函数指针 |
| `update` | 更新函数指针，输入数据 |
| `final` | 完成函数指针，输出摘要 |
| `block_size` | 块大小：SHA256=64, SHA1=64 |

**常用EVP_MD获取函数：**

| 函数 | 算法 | 输出大小 |
|------|------|---------|
| `EVP_sha256()` | SHA-256 | 32字节 |
| `EVP_sha1()` | SHA-1 | 20字节 |
| `EVP_md5()` | MD5 | 16字节 |
| `EVP_sha512()` | SHA-512 | 64字节 |
| `EVP_sm3()` | SM3 | 32字节 |
| `EVP_md_null()` | 空摘要 | 0字节 |

---

#### 21.2.3 EVP_CIPHER - 对称加密方法结构

```c
struct evp_cipher_st {
    int nid;                    // 算法NID
    int block_size;             // 块大小（字节）
    int key_len;                // 密钥长度（字节）
    int iv_len;                 // IV长度（字节）
    unsigned long flags;       // 标志位
    int (*init) (EVP_CIPHER_CTX *, const unsigned char *, const unsigned char *, int);  // 初始化
    int (*do_cipher) (EVP_CIPHER_CTX *, unsigned char *, const unsigned char *, size_t);  // 加解密
    int (*cleanup) (EVP_CIPHER_CTX *);  // 清理
    int ctx_size;               // 上下文大小
    int (*set_asn1_parameters) (...);  // 设置ASN.1参数
    int (*get_asn1_parameters) (...);  // 获取ASN.1参数
    int (*ctrl) (EVP_CIPHER_CTX *, int, int, void *);  // 控制函数
    void *app_data;             // 应用数据
};
```

**成员说明：**

| 成员 | 作用 |
|------|------|
| `nid` | 算法NID，如NID_aes_256_cbc |
| `block_size` | 块大小：AES=16, DES=8 |
| `key_len` | 密钥长度：AES-256=32, AES-128=16 |
| `iv_len` | IV长度：AES=16, DES=8 |
| `flags` | 标志位，包含模式信息 |
| `init` | 初始化函数 |
| `do_cipher` | 加解密函数 |
| `cleanup` | 清理函数 |

**flags中的模式：**
```c
EVP_CIPH_ECB_MODE      // 电子密码本模式
EVP_CIPH_CBC_MODE      // 密码分组链模式（最常用）
EVP_CIPH_CFB_MODE      // 密码反馈模式
EVP_CIPH_OFB_MODE      // 输出反馈模式
EVP_CIPH_CTR_MODE      // 计数器模式
EVP_CIPH_GCM_MODE      // Galois/Counter模式（认证加密）
EVP_CIPH_STREAM_CIPHER // 流密码模式
```

**常用EVP_CIPHER获取函数：**

| 函数 | 算法 | 密钥长度 | 模式 |
|------|------|---------|------|
| `EVP_aes_256_cbc()` | AES-256 | 32字节 | CBC |
| `EVP_aes_128_cbc()` | AES-128 | 16字节 | CBC |
| `EVP_aes_256_ecb()` | AES-256 | 32字节 | ECB |
| `EVP_aes_256_ctr()` | AES-256 | 32字节 | CTR |
| `EVP_aes_256_gcm()` | AES-256 | 32字节 | GCM |
| `EVP_des_ede3_cbc()` | 3DES | 24字节 | CBC |
| `EVP_rc4()` | RC4 | 16字节 | 流 |
| `EVP_chacha20()` | ChaCha20 | 32字节 | 流 |

---

#### 21.2.4 EVP_CIPHER_CTX - 对称加密上下文

```c
struct evp_cipher_ctx_st {
    const EVP_CIPHER *cipher;   // 加密方法
    ENGINE *engine;             // 引擎
    int encrypt;                // 加密标志：1=加密, 0=解密
    int buf_len;                // 缓冲区长度
    int num;                    // 部分块处理
    unsigned char oiv[EVP_MAX_IV_LENGTH];  // 原始IV
    unsigned char iv[EVP_MAX_IV_LENGTH];   // 当前IV
    unsigned char buf[EVP_MAX_BLOCK_LENGTH];  // 缓冲区
    int flags;                  // 标志
    void *cipher_data;          // 加密器私有数据
    int key_len;                // 密钥长度
    int iv_len;                 // IV长度
    int block_size;             // 块大小
    int mode;                   // 模式
    int padding;                // 填充标志
};
```

**成员说明：**

| 成员 | 作用 |
|------|------|
| `cipher` | 指向EVP_CIPHER结构 |
| `encrypt` | 1=加密, 0=解密 |
| `oiv` | 原始IV（不变） |
| `iv` | 当前IV（会更新） |
| `buf` | 缓冲区，用于存储未处理完的块 |
| `cipher_data` | 加密器私有数据 |
| `padding` | 是否使用填充（1=使用） |

---

### 21.3 EVP摘要函数

| 函数 | 作用 |
|------|------|
| `EVP_MD_CTX_new()` | 创建摘要上下文 |
| `EVP_MD_CTX_free(EVP_MD_CTX *)` | 释放摘要上下文 |
| `EVP_DigestInit(EVP_MD_CTX *, EVP_MD *)` | 初始化摘要 |
| `EVP_DigestUpdate(EVP_MD_CTX *, data, len)` | 更新摘要（可多次调用） |
| `EVP_DigestFinal(EVP_MD_CTX *, md, len)` | 完成摘要，输出结果 |
| `EVP_Digest(data, len, md, len, EVP_MD *, NULL)` | 一次性计算摘要 |

**使用流程：**
```c
// 1. 创建上下文
EVP_MD_CTX *ctx = EVP_MD_CTX_new();

// 2. 初始化
EVP_DigestInit(ctx, EVP_sha256());

// 3. 更新（可多次调用）
EVP_DigestUpdate(ctx, data1, len1);
EVP_DigestUpdate(ctx, data2, len2);

// 4. 完成
unsigned char md[32];
unsigned int md_len;
EVP_DigestFinal(ctx, md, &md_len);

// 5. 清理
EVP_MD_CTX_free(ctx);
```

---

### 21.4 EVP对称加解密函数

| 函数 | 作用 |
|------|------|
| `EVP_CIPHER_CTX_new()` | 创建加密上下文 |
| `EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *)` | 释放加密上下文 |
| `EVP_EncryptInit(EVP_CIPHER_CTX *, EVP_CIPHER *, key, iv)` | 初始化加密 |
| `EVP_EncryptUpdate(EVP_CIPHER_CTX *, out, outl, in, inl)` | 加密更新 |
| `EVP_EncryptFinal(EVP_CIPHER_CTX *, out, outl)` | 完成加密（处理最后一块） |
| `EVP_DecryptInit(EVP_CIPHER_CTX *, EVP_CIPHER *, key, iv)` | 初始化解密 |
| `EVP_DecryptUpdate(EVP_CIPHER_CTX *, out, outl, in, inl)` | 解密更新 |
| `EVP_DecryptFinal(EVP_CIPHER_CTX *, out, outl)` | 完成解密 |
| `EVP_CIPHER_CTX_set_padding(EVP_CIPHER_CTX *, pad)` | 设置填充（1=启用，0=禁用） |
| `EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *, type, arg, ptr)` | 控制函数 |

**加密流程：**
```c
// 1. 创建上下文
EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

// 2. 初始化加密
EVP_EncryptInit(ctx, EVP_aes_256_cbc(), key, iv);

// 3. 加密更新（可多次调用）
int outl;
EVP_EncryptUpdate(ctx, out, &outl, in, inl);

// 4. 完成加密（处理最后一块和填充）
int finall;
EVP_EncryptFinal(ctx, out + outl, &finall);

// 总输出长度 = outl + finall

// 5. 清理
EVP_CIPHER_CTX_free(ctx);
```

**解密流程：**
```c
// 1. 创建上下文
EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

// 2. 初始化解密
EVP_DecryptInit(ctx, EVP_aes_256_cbc(), key, iv);

// 3. 解密更新（可多次调用）
int outl;
EVP_DecryptUpdate(ctx, out, &outl, in, inl);

// 4. 完成解密
int finall;
EVP_DecryptFinal(ctx, out + outl, &finall);

// 5. 清理
EVP_CIPHER_CTX_free(ctx);
```

---

### 21.5 EVP非对称函数

| 函数 | 作用 |
|------|------|
| `EVP_PKEY_new()` | 创建EVP_PKEY |
| `EVP_PKEY_free(EVP_PKEY *)` | 释放EVP_PKEY |
| `EVP_PKEY_keygen_init(EVP_PKEY_CTX *)` | 初始化密钥生成 |
| `EVP_PKEY_keygen(EVP_PKEY_CTX *, EVP_PKEY **)` | 生成密钥 |
| `EVP_PKEY_CTX_new(EVP_PKEY *, ENGINE *)` | 创建公钥上下文 |
| `EVP_PKEY_CTX_free(EVP_PKEY_CTX *)` | 释放公钥上下文 |
| `EVP_PKEY_encrypt_init(EVP_PKEY_CTX *)` | 初始化加密 |
| `EVP_PKEY_encrypt(EVP_PKEY_CTX *, out, outl, in, inl)` | 公钥加密 |
| `EVP_PKEY_decrypt_init(EVP_PKEY_CTX *)` | 初始化解密 |
| `EVP_PKEY_decrypt(EVP_PKEY_CTX *, out, outl, in, inl)` | 私钥解密 |
| `EVP_PKEY_sign_init(EVP_PKEY_CTX *)` | 初始化签名 |
| `EVP_PKEY_sign(EVP_PKEY_CTX *, sig, sigl, md, mdl)` | 签名 |
| `EVP_PKEY_verify_init(EVP_PKEY_CTX *)` | 初始化验证 |
| `EVP_PKEY_verify(EVP_PKEY_CTX *, sig, sigl, md, mdl)` | 验证签名 |

---

### 21.6 EVP对称加密过程详解

#### 加密过程

1. **初始化阶段**
   - 设置加密算法（如AES-256-CBC）
   - 设置密钥
   - 设置IV（初始化向量）
   - 准备工作：扩展密钥、设置IV

2. **更新阶段**
   - 可以多次调用，输入任意长度数据
   - 内部按块处理
   - 未满一块的数据存入缓冲区
   - 输出 = 完整块数量 × 块大小

3. **完成阶段**
   - 处理缓冲区中的剩余数据
   - 添加PKCS#7填充（如果启用）
   - 加密最后一块
   - 输出 = 1或2个块大小

**关于填充：**
- PKCS#7填充：最后一个字节表示填充字节数
- 例如：需要填充3字节 → 0x03 0x03 0x03
- 如果数据刚好是块大小的整数倍 → 添加一整块填充（0x10...0x10）

#### 解密过程

1. **初始化阶段**
   - 与加密相同
   - 设置相同的算法、密钥、IV

2. **更新阶段**
   - 解密数据
   - 输出完整块

3. **完成阶段**
   - 处理最后一块
   - 验证并移除填充
   - 输出 = 解密后的数据长度（可能小于块大小）

---

### 21.7 EVP编程示例（完整C++封装）

见 `OpenSSL_C++_完整教程_EVP重点版.cpp`

---

## 各章数据结构与常用函数速查表

### BIO章

| 数据结构 | 常用函数 |
|---------|---------|
| `BIO` | `BIO_new()`, `BIO_free()`, `BIO_write()`, `BIO_read()` |
| `BIO_METHOD` | `BIO_s_mem()`, `BIO_s_file()`, `BIO_s_socket()` |

### 摘要与HMAC章

| 数据结构 | 常用函数 |
|---------|---------|
| `EVP_MD` | `EVP_sha256()`, `EVP_sha1()`, `EVP_md5()` |
| `EVP_MD_CTX` | `EVP_DigestInit()`, `EVP_DigestUpdate()`, `EVP_DigestFinal()` |

### RSA章

| 数据结构 | 常用函数 |
|---------|---------|
| `RSA` | `RSA_new()`, `RSA_free()`, `RSA_generate_key_ex()` |
| - | `RSA_public_encrypt()`, `RSA_private_decrypt()` |

### EVP章（重点）

| 数据结构 | 常用函数 |
|---------|---------|
| `EVP_PKEY` | `EVP_PKEY_new()`, `EVP_PKEY_free()`, `EVP_PKEY_assign_RSA()` |
| `EVP_MD` | `EVP_sha256()`, `EVP_sha1()`, `EVP_md5()` |
| `EVP_MD_CTX` | `EVP_DigestInit()`, `EVP_DigestUpdate()`, `EVP_DigestFinal()` |
| `EVP_CIPHER` | `EVP_aes_256_cbc()`, `EVP_aes_128_cbc()`, `EVP_des_ede3_cbc()` |
| `EVP_CIPHER_CTX` | `EVP_EncryptInit()`, `EVP_EncryptUpdate()`, `EVP_EncryptFinal()` |
| - | `EVP_DecryptInit()`, `EVP_DecryptUpdate()`, `EVP_DecryptFinal()` |

---

*教程完*
