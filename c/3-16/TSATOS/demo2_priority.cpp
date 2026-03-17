/**
 * Demo 2: 使用优先级队列的子系统管理
 * 
 * 展示：更优雅的方式 - 让子系统自己注册优先级
 * 然后按优先级自动排序启动和终止
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// 子系统基类
class Subsystem {
public:
    Subsystem(const std::string& name, int priority) 
        : m_name(name), m_priority(priority), m_started(false) {}
    
    virtual ~Subsystem() = default;
    
    const std::string& getName() const { return m_name; }
    int getPriority() const { return m_priority; }
    bool isStarted() const { return m_started; }
    
    void startUp() {
        if (!m_started) {
            std::cout << "[" << m_name << "] 启动中 (优先级: " << m_priority << ")" << std::endl;
            onStartUp();
            m_started = true;
            std::cout << "[" << m_name << "] 启动完成！" << std::endl;
        }
    }
    
    void shutDown() {
        if (m_started) {
            std::cout << "[" << m_name << "] 终止中" << std::endl;
            onShutDown();
            m_started = false;
            std::cout << "[" << m_name << "] 终止完成！" << std::endl;
        }
    }
    
protected:
    virtual void onStartUp() = 0;  // 子类实现实际启动逻辑
    virtual void onShutDown() = 0;  // 子类实现实际终止逻辑
    
    std::string m_name;
    int m_priority;  // 优先级：数字越小越先启动
    bool m_started;
};

// 子系统管理器
class SubsystemManager {
public:
    static SubsystemManager& getInstance() {
        static SubsystemManager instance;
        return instance;
    }
    
    // 注册子系统
    void registerSubsystem(Subsystem* subsystem) {
        m_subsystems.push_back(subsystem);
        std::cout << "[Manager] 注册子系统: " << subsystem->getName() 
                  << " (优先级: " << subsystem->getPriority() << ")" << std::endl;
    }
    
    // 按优先级启动所有子系统
    void startAll() {
        std::cout << std::endl;
        std::cout << "[Manager] 开始按优先级启动所有子系统..." << std::endl;
        std::cout << std::endl;
        
        // 按优先级排序（优先级数字小的在前）
        std::sort(m_subsystems.begin(), m_subsystems.end(),
            [](Subsystem* a, Subsystem* b) {
                return a->getPriority() < b->getPriority();
            });
        
        // 按顺序启动
        for (auto* subsystem : m_subsystems) {
            subsystem->startUp();
            std::cout << std::endl;
        }
        
        std::cout << "[Manager] 所有子系统启动完成！" << std::endl;
        std::cout << std::endl;
    }
    
    // 按优先级的逆序终止所有子系统
    void shutDownAll() {
        std::cout << "[Manager] 开始按优先级逆序终止所有子系统..." << std::endl;
        std::cout << std::endl;
        
        // 按优先级逆序终止（优先级数字大的先终止）
        std::sort(m_subsystems.begin(), m_subsystems.end(),
            [](Subsystem* a, Subsystem* b) {
                return a->getPriority() > b->getPriority();
            });
        
        // 按顺序终止
        for (auto* subsystem : m_subsystems) {
            subsystem->shutDown();
            std::cout << std::endl;
        }
        
        std::cout << "[Manager] 所有子系统终止完成！" << std::endl;
        std::cout << std::endl;
    }
    
private:
    SubsystemManager() = default;
    std::vector<Subsystem*> m_subsystems;
};

// ========== 具体的子系统实现 ==========

class MemoryManager : public Subsystem {
public:
    MemoryManager() : Subsystem("MemoryManager", 100) {}  // 优先级 100（最高）
    
protected:
    void onStartUp() override {
        std::cout << "  [MemoryManager] 初始化内存池..." << std::endl;
        std::cout << "  [MemoryManager] 设置内存跟踪器..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [MemoryManager] 停止内存跟踪..." << std::endl;
        std::cout << "  [MemoryManager] 释放内存池..." << std::endl;
    }
};

class FileSystemManager : public Subsystem {
public:
    FileSystemManager() : Subsystem("FileSystemManager", 200) {}  // 优先级 200
    
protected:
    void onStartUp() override {
        std::cout << "  [FileSystemManager] 挂载文件系统..." << std::endl;
        std::cout << "  [FileSystemManager] 加载配置文件..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [FileSystemManager] 保存配置文件..." << std::endl;
        std::cout << "  [FileSystemManager] 卸载文件系统..." << std::endl;
    }
};

class VideoManager : public Subsystem {
public:
    VideoManager() : Subsystem("VideoManager", 300) {}  // 优先级 300
    
protected:
    void onStartUp() override {
        std::cout << "  [VideoManager] 初始化显示设备..." << std::endl;
        std::cout << "  [VideoManager] 设置视频模式..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [VideoManager] 恢复视频模式..." << std::endl;
        std::cout << "  [VideoManager] 关闭显示设备..." << std::endl;
    }
};

class TextureManager : public Subsystem {
public:
    TextureManager() : Subsystem("TextureManager", 400) {}  // 优先级 400
    
protected:
    void onStartUp() override {
        std::cout << "  [TextureManager] 初始化纹理缓存..." << std::endl;
        std::cout << "  [TextureManager] 加载默认纹理..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [TextureManager] 卸载所有纹理..." << std::endl;
        std::cout << "  [TextureManager] 释放纹理缓存..." << std::endl;
    }
};

class RenderManager : public Subsystem {
public:
    RenderManager() : Subsystem("RenderManager", 500) {}  // 优先级 500
    
protected:
    void onStartUp() override {
        std::cout << "  [RenderManager] 初始化渲染器..." << std::endl;
        std::cout << "  [RenderManager] 编译着色器..." << std::endl;
        std::cout << "  [RenderManager] 设置渲染状态..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [RenderManager] 释放着色器..." << std::endl;
        std::cout << "  [RenderManager] 关闭渲染器..." << std::endl;
    }
};

class InputManager : public Subsystem {
public:
    InputManager() : Subsystem("InputManager", 600) {}  // 优先级 600
    
protected:
    void onStartUp() override {
        std::cout << "  [InputManager] 初始化输入设备..." << std::endl;
        std::cout << "  [InputManager] 注册按键映射..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [InputManager] 注销按键映射..." << std::endl;
        std::cout << "  [InputManager] 关闭输入设备..." << std::endl;
    }
};

// 全局子系统实例
MemoryManager gMemoryManager;
FileSystemManager gFileSystemManager;
VideoManager gVideoManager;
TextureManager gTextureManager;
RenderManager gRenderManager;
InputManager gInputManager;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Demo 2: 使用优先级队列的子系统管理" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // 1. 获取管理器实例
    SubsystemManager& manager = SubsystemManager::getInstance();
    
    // 2. 注册所有子系统（顺序不重要，会按优先级排序）
    std::cout << "--- 注册子系统 ---" << std::endl;
    manager.registerSubsystem(&gRenderManager);    // 优先级 500
    manager.registerSubsystem(&gInputManager);      // 优先级 600
    manager.registerSubsystem(&gMemoryManager);     // 优先级 100
    manager.registerSubsystem(&gTextureManager);    // 优先级 400
    manager.registerSubsystem(&gFileSystemManager); // 优先级 200
    manager.registerSubsystem(&gVideoManager);      // 优先级 300
    
    // 3. 按优先级启动所有子系统
    manager.startAll();
    
    // 4. 模拟游戏运行
    std::cout << "--- 游戏运行中... ---" << std::endl;
    std::cout << std::endl;
    
    // 5. 按优先级逆序终止所有子系统
    manager.shutDownAll();
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Demo 2 完成！" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
