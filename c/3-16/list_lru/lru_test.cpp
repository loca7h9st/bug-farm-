#include "lru_cache.hpp"
#include <iostream>
#include <string>

void test_basic_operations() {
    std::cout << "========================================" << std::endl;
    std::cout << "   LRU Cache 基础操作测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    LRUCache<int, std::string> cache(3);
    
    std::cout << "\n--- 测试 put 操作 ---" << std::endl;
    cache.put(1, "One");
    cache.print();
    
    cache.put(2, "Two");
    cache.print();
    
    cache.put(3, "Three");
    cache.print();
    
    std::cout << "\n--- 测试超出容量（淘汰机制）---" << std::endl;
    cache.put(4, "Four");
    cache.print();
    std::cout << "说明: Key 1 (One) 被淘汰，因为它是最久未使用的" << std::endl;
    
    std::cout << "\n--- 测试 get 操作（访问会更新使用顺序）---" << std::endl;
    std::string value;
    if (cache.get(2, value)) {
        std::cout << "get(2) = " << value << std::endl;
    }
    cache.print();
    std::cout << "说明: Key 2 被移到头部，表示最近使用" << std::endl;
    
    std::cout << "\n--- 测试再次超出容量 ---" << std::endl;
    cache.put(5, "Five");
    cache.print();
    std::cout << "说明: Key 3 被淘汰，因为 Key 2 刚被访问过" << std::endl;
    
    std::cout << "\n--- 测试更新已存在的 key ---" << std::endl;
    cache.put(2, "Two Updated");
    cache.print();
    std::cout << "说明: Key 2 的值被更新，并移到头部" << std::endl;
}

void test_edge_cases() {
    std::cout << "\n\n========================================" << std::endl;
    std::cout << "   LRU Cache 边界情况测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n--- 测试容量为 1 的缓存 ---" << std::endl;
    LRUCache<std::string, int> small_cache(1);
    small_cache.put("A", 1);
    small_cache.print();
    small_cache.put("B", 2);
    small_cache.print();
    std::cout << "说明: 容量为 1 时，每次新插入都会淘汰旧的" << std::endl;
    
    std::cout << "\n--- 测试访问不存在的 key ---" << std::endl;
    int value;
    if (!small_cache.get("A", value)) {
        std::cout << "get('A'): 未找到（预期行为）" << std::endl;
    }
    
    std::cout << "\n--- 测试 remove 操作 ---" << std::endl;
    LRUCache<int, double> remove_cache(5);
    for (int i = 1; i <= 5; ++i) {
        remove_cache.put(i, i * 1.1);
    }
    remove_cache.print();
    remove_cache.remove(3);
    std::cout << "remove(3) 后:" << std::endl;
    remove_cache.print();
    
    std::cout << "\n--- 测试 clear 操作 ---" << std::endl;
    remove_cache.clear();
    std::cout << "clear 后 - size: " << remove_cache.size() 
              << ", empty: " << (remove_cache.empty() ? "yes" : "no") << std::endl;
}

void test_practical_scenario() {
    std::cout << "\n\n========================================" << std::endl;
    std::cout << "   LRU Cache 实际应用场景模拟" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\n场景: 模拟网页缓存，容量为 3" << std::endl;
    LRUCache<std::string, std::string> web_cache(3);
    
    std::cout << "\n1. 用户访问页面..." << std::endl;
    web_cache.put("/home", "<html>Home Page</html>");
    web_cache.put("/about", "<html>About Page</html>");
    web_cache.put("/products", "<html>Products Page</html>");
    web_cache.print();
    
    std::cout << "\n2. 用户再次访问 /home（命中缓存）..." << std::endl;
    std::string content;
    if (web_cache.get("/home", content)) {
        std::cout << "缓存命中! 快速返回: " << content << std::endl;
    }
    web_cache.print();
    
    std::cout << "\n3. 用户访问新页面 /contact..." << std::endl;
    web_cache.put("/contact", "<html>Contact Page</html>");
    web_cache.print();
    std::cout << "说明: /about 被淘汰，因为它是最久未使用的" << std::endl;
    
    std::cout << "\n4. 用户访问 /about（缓存未命中）..." << std::endl;
    if (!web_cache.get("/about", content)) {
        std::cout << "缓存未命中! 需要从数据库/服务器加载..." << std::endl;
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "       LRU Cache 完整测试 Demo" << std::endl;
    std::cout << "          (使用自定义 list.hpp)" << std::endl;
    std::cout << "========================================" << std::endl;
    
    test_basic_operations();
    test_edge_cases();
    test_practical_scenario();
    
    std::cout << "\n\n========================================" << std::endl;
    std::cout << "       所有测试完成！✓" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
