/**
 * Demo 1: 最简单的子系统启动和终止
 * 
 * 展示：使用明确的 startUp() 和 shutDown() 方法
 * 代替构造函数和析构函数
 */

#include <iostream>
#include <string>

// 前向声明
class MemoryManager;
class FileSystemManager;
class RenderManager;

// 全局单例声明
extern MemoryManager gMemoryManager;
extern FileSystemManager gFileSystemManager;
extern RenderManager gRenderManager;

/**
 * 内存管理器 - 应该最先启动，最后终止
 */
class MemoryManager {
public:
    MemoryManager() {
        // 构造函数不做任何事情
        std::cout << "[MemoryManager] 构造函数被调用（不做实际工作）" << std::endl;
    }
    
    ~MemoryManager() {
        // 析构函数不做任何事情
        std::cout << "[MemoryManager] 析构函数被调用（不做实际工作）" << std::endl;
    }
    
    void startUp() {
        std::cout << "[MemoryManager] 启动中..." << std::endl;
        // 实际的初始化工作
        std::cout << "[MemoryManager] 初始化内存池..." << std::endl;
        std::cout << "[MemoryManager] 启动完成！" << std::endl;
    }
    
    void shutDown() {
        std::cout << "[MemoryManager] 终止中..." << std::endl;
        // 实际的清理工作
        std::cout << "[MemoryManager] 释放内存池..." << std::endl;
        std::cout << "[MemoryManager] 终止完成！" << std::endl;
    }
};

/**
 * 文件系统管理器 - 依赖内存管理器
 */
class FileSystemManager {
public:
    FileSystemManager() {
        std::cout << "[FileSystemManager] 构造函数被调用（不做实际工作）" << std::endl;
    }
    
    ~FileSystemManager() {
        std::cout << "[FileSystemManager] 析构函数被调用（不做实际工作）" << std::endl;
    }
    
    void startUp() {
        std::cout << "[FileSystemManager] 启动中..." << std::endl;
        std::cout << "[FileSystemManager] 挂载文件系统..." << std::endl;
        std::cout << "[FileSystemManager] 启动完成！" << std::endl;
    }
    
    void shutDown() {
        std::cout << "[FileSystemManager] 终止中..." << std::endl;
        std::cout << "[FileSystemManager] 卸载文件系统..." << std::endl;
        std::cout << "[FileSystemManager] 终止完成！" << std::endl;
    }
};

/**
 * 渲染管理器 - 依赖文件系统和内存管理器
 */
class RenderManager {
public:
    RenderManager() {
        std::cout << "[RenderManager] 构造函数被调用（不做实际工作）" << std::endl;
    }
    
    ~RenderManager() {
        std::cout << "[RenderManager] 析构函数被调用（不做实际工作）" << std::endl;
    }
    
    void startUp() {
        std::cout << "[RenderManager] 启动中..." << std::endl;
        std::cout << "[RenderManager] 初始化渲染设备..." << std::endl;
        std::cout << "[RenderManager] 加载着色器..." << std::endl;
        std::cout << "[RenderManager] 启动完成！" << std::endl;
    }
    
    void shutDown() {
        std::cout << "[RenderManager] 终止中..." << std::endl;
        std::cout << "[RenderManager] 卸载着色器..." << std::endl;
        std::cout << "[RenderManager] 关闭渲染设备..." << std::endl;
        std::cout << "[RenderManager] 终止完成！" << std::endl;
    }
};

// 全局单例定义
MemoryManager gMemoryManager;
FileSystemManager gFileSystemManager;
RenderManager gRenderManager;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Demo 1: 简单的子系统启动和终止" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- 开始启动子系统 ---" << std::endl;
    std::cout << "启动顺序：内存 → 文件系统 → 渲染" << std::endl;
    std::cout << std::endl;
    
    // 按正确顺序启动
    gMemoryManager.startUp();
    std::cout << std::endl;
    gFileSystemManager.startUp();
    std::cout << std::endl;
    gRenderManager.startUp();
    std::cout << std::endl;
    
    std::cout << "--- 子系统全部启动完成！" << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- 模拟游戏运行中... ---" << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- 开始终止子系统 ---" << std::endl;
    std::cout << "终止顺序：渲染 → 文件系统 → 内存（启动的逆序）" << std::endl;
    std::cout << std::endl;
    
    // 按启动的逆序终止
    gRenderManager.shutDown();
    std::cout << std::endl;
    gFileSystemManager.shutDown();
    std::cout << std::endl;
    gMemoryManager.shutDown();
    std::cout << std::endl;
    
    std::cout << "--- 子系统全部终止完成！" << std::endl;
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Demo 1 完成！" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
