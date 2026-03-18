# OpenSSL C++ 面向对象编程教程

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

---

## 🛠️ 环境准备

### 系统要求
- Linux (Debian/Ubuntu推荐)
- OpenSSL 1.0.2l 或更高版本
- C++11 或 C++14 编译器
- CMake 3.10+ (可选)

### 安装依赖
```bash
# Debian/Ubuntu
apt install build-essential libssl-dev cmake

# 验证安装
openssl version
g++ --version
```

---

## 📁 项目结构

```
openssl-tutorial/
├── README.md
├── src/                    # C++封装类源码
│   ├── bio/               # BIO封装
│   ├── crypto/            # 加密算法封装
│   └── ssl/               # SSL/TLS封装
├── examples/              # 示例代码
│   ├── 01_bio/           # BIO示例
│   ├── 02_symmetric/     # 对称加密示例
│   ├── 03_asymmetric/    # 非对称加密示例
│   ├── 04_hash/          # 摘要示例
│   └── 05_ssl/           # SSL/TLS示例
└── notes/                # 学习笔记
    ├── 01_bio.md
    ├── 02_crypto.md
    └── 03_ransomware.md
```

---

## 🎯 学习目标

1. ✅ 理解OpenSSL核心数据结构
2. ✅ 掌握C++面向对象封装OpenSSL
3. ✅ 学会对称/非对称加密算法
4. ✅ 掌握SSL/TLS网络编程
5. ✅ 了解勒索软件常用加密方式

---

## ⚠️ 免责声明

本教程仅用于学习和研究目的。任何将本教程内容用于非法活动的行为与作者无关。

---
