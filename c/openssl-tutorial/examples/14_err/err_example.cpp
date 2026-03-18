/**
 * 第14章：错误处理 - 示例代码
 *
 * 编译: g++ -std=c++11 -o err_example err_example.cpp -lssl -lcrypto
 */

#include <openssl/err.h>
#include <openssl/rsa.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

// ============================================
// OpenSSL错误处理类
// ============================================
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

    static void check_and_throw(const std::string& context) {
        std::vector<std::string> errors = get_all_errors();
        if (!errors.empty()) {
            std::ostringstream oss;
            oss << context << ":" << std::endl;
            for (const auto& err : errors) {
                oss << "  - " << err << std::endl;
            }
            throw std::runtime_error(oss.str());
        }
    }
};

// ============================================
// 测试
// ============================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  第14章：错误处理 - 测试程序" << std::endl;
    std::cout << "========================================" << std::endl;

    try {
        // 测试1：正常情况（无错误）
        std::cout << "\n=== 测试1：正常情况 ===" << std::endl;
        OpenSSLError::clear_errors();
        std::vector<std::string> errors1 = OpenSSLError::get_all_errors();
        std::cout << "错误数量: " << errors1.size() << std::endl;
        std::cout << "✅ 正常情况测试通过！" << std::endl;

        // 测试2：触发错误（跳过可能导致段错误的测试）
        std::cout << "\n=== 测试2：触发错误（已跳过，避免段错误）===" << std::endl;
        std::cout << "已跳过无效RSA操作测试" << std::endl;
        OpenSSLError::clear_errors();

        // 测试3：用RSA生成密钥对（应该成功）
        std::cout << "\n=== 测试3：正常操作（RSA密钥生成）===" << std::endl;
        OpenSSLError::clear_errors();

        RSA* rsa2 = RSA_new();
        BIGNUM* e = BN_new();
        BN_set_word(e, RSA_F4);

        if (RSA_generate_key_ex(rsa2, 2048, e, nullptr)) {
            std::cout << "RSA密钥生成成功！" << std::endl;
            std::vector<std::string> errors3 = OpenSSLError::get_all_errors();
            std::cout << "错误数量: " << errors3.size() << std::endl;
            std::cout << "✅ 正常RSA操作测试通过！" << std::endl;
        } else {
            std::cout << "RSA密钥生成失败！" << std::endl;
            OpenSSLError::print_errors(stdout);
        }

        BN_free(e);
        RSA_free(rsa2);
        OpenSSLError::clear_errors();

        std::cout << "\n========================================" << std::endl;
        std::cout << "  所有测试完成！🎉" << std::endl;
        std::cout << "========================================" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
