/**
 * Demo 3: 使用依赖图的子系统管理
 * 
 * 展示：最优雅的方式 - 每个子系统声明自己的依赖
 * 然后自动计算正确的启动顺序
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <stdexcept>

// 前向声明
class Subsystem;

// 子系统基类
class Subsystem {
public:
    Subsystem(const std::string& name) 
        : m_name(name), m_started(false) {}
    
    virtual ~Subsystem() = default;
    
    const std::string& getName() const { return m_name; }
    bool isStarted() const { return m_started; }
    
    // 获取依赖的子系统列表
    virtual std::vector<std::string> getDependencies() const {
        return {};
    }
    
    void startUp() {
        if (!m_started) {
            std::cout << "[" << m_name << "] 启动中" << std::endl;
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
    virtual void onStartUp() = 0;
    virtual void onShutDown() = 0;
    
    std::string m_name;
    bool m_started;
};

// 依赖图管理器 - 使用拓扑排序
class DependencyGraphManager {
public:
    static DependencyGraphManager& getInstance() {
        static DependencyGraphManager instance;
        return instance;
    }
    
    // 注册子系统
    void registerSubsystem(Subsystem* subsystem) {
        m_subsystems[subsystem->getName()] = subsystem;
        std::cout << "[Manager] 注册子系统: " << subsystem->getName() << std::endl;
    }
    
    // 按依赖关系启动所有子系统
    void startAll() {
        std::cout << std::endl;
        std::cout << "[Manager] 分析依赖关系..." << std::endl;
        
        // 构建依赖图
        buildDependencyGraph();
        
        // 拓扑排序
        std::vector<std::string> startOrder = topologicalSort();
        
        std::cout << "[Manager] 计算得到的启动顺序:" << std::endl;
        for (size_t i = 0; i < startOrder.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << startOrder[i] << std::endl;
        }
        std::cout << std::endl;
        
        // 按顺序启动
        std::cout << "[Manager] 开始启动子系统..." << std::endl;
        std::cout << std::endl;
        
        for (const auto& name : startOrder) {
            m_subsystems[name]->startUp();
            std::cout << std::endl;
        }
        
        std::cout << "[Manager] 所有子系统启动完成！" << std::endl;
        std::cout << std::endl;
    }
    
    // 按依赖关系的逆序终止所有子系统
    void shutDownAll() {
        std::cout << "[Manager] 开始终止子系统..." << std::endl;
        std::cout << std::endl;
        
        // 再次拓扑排序，然后逆序终止
        std::vector<std::string> startOrder = topologicalSort();
        std::reverse(startOrder.begin(), startOrder.end());
        
        std::cout << "[Manager] 终止顺序（启动的逆序):" << std::endl;
        for (size_t i = 0; i < startOrder.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << startOrder[i] << std::endl;
        }
        std::cout << std::endl;
        
        for (const auto& name : startOrder) {
            m_subsystems[name]->shutDown();
            std::cout << std::endl;
        }
        
        std::cout << "[Manager] 所有子系统终止完成！" << std::endl;
        std::cout << std::endl;
    }
    
private:
    DependencyGraphManager() = default;
    
    // 构建依赖图
    void buildDependencyGraph() {
        m_adjacency.clear();
        m_inDegree.clear();
        
        // 初始化
        for (const auto& pair : m_subsystems) {
            m_adjacency[pair.first] = {};
            m_inDegree[pair.first] = 0;
        }
        
        // 构建边
        for (const auto& pair : m_subsystems) {
            const std::string& name = pair.first;
            Subsystem* subsystem = pair.second;
            
            for (const auto& dep : subsystem->getDependencies()) {
                // 添加边: dep -> name (dep必须先于name启动)
                m_adjacency[dep].push_back(name);
                m_inDegree[name]++;
            }
        }
    }
    
    // 拓扑排序（Kahn算法）
    std::vector<std::string> topologicalSort() {
        std::vector<std::string> result;
        std::queue<std::string> queue;
        
        // 找到所有入度为0的节点
        for (const auto& pair : m_inDegree) {
            if (pair.second == 0) {
                queue.push(pair.first);
            }
        }
        
        while (!queue.empty()) {
            std::string current = queue.front();
            queue.pop();
            result.push_back(current);
            
            for (const auto& neighbor : m_adjacency[current]) {
                m_inDegree[neighbor]--;
                if (m_inDegree[neighbor] == 0) {
                    queue.push(neighbor);
                }
            }
        }
        
        // 检查是否有环
        if (result.size() != m_subsystems.size()) {
            throw std::runtime_error("依赖图中存在循环依赖！");
        }
        
        return result;
    }
    
    std::unordered_map<std::string, Subsystem*> m_subsystems;
    std::unordered_map<std::string, std::vector<std::string>> m_adjacency;
    std::unordered_map<std::string, int> m_inDegree;
};

// ========== 具体的子系统实现 ==========

class MemoryManager : public Subsystem {
public:
    MemoryManager() : Subsystem("MemoryManager") {}
    
    // 没有依赖
    std::vector<std::string> getDependencies() const override {
        return {};
    }
    
protected:
    void onStartUp() override {
        std::cout << "  [MemoryManager] 初始化内存池..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [MemoryManager] 释放内存池..." << std::endl;
    }
};

class FileSystemManager : public Subsystem {
public:
    FileSystemManager() : Subsystem("FileSystemManager") {}
    
    // 依赖内存管理器
    std::vector<std::string> getDependencies() const override {
        return {"MemoryManager"};
    }
    
protected:
    void onStartUp() override {
        std::cout << "  [FileSystemManager] 挂载文件系统..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [FileSystemManager] 卸载文件系统..." << std::endl;
    }
};

class VideoManager : public Subsystem {
public:
    VideoManager() : Subsystem("VideoManager") {}
    
    // 依赖内存管理器
    std::vector<std::string> getDependencies() const override {
        return {"MemoryManager"};
    }
    
protected:
    void onStartUp() override {
        std::cout << "  [VideoManager] 初始化显示设备..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [VideoManager] 关闭显示设备..." << std::endl;
    }
};

class TextureManager : public Subsystem {
public:
    TextureManager() : Subsystem("TextureManager") {}
    
    // 依赖文件系统和视频管理器
    std::vector<std::string> getDependencies() const override {
        return {"FileSystemManager", "VideoManager"};
    }
    
protected:
    void onStartUp() override {
        std::cout << "  [TextureManager] 初始化纹理缓存..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [TextureManager] 释放纹理缓存..." << std::endl;
    }
};

class RenderManager : public Subsystem {
public:
    RenderManager() : Subsystem("RenderManager") {}
    
    // 依赖纹理管理器和视频管理器
    std::vector<std::string> getDependencies() const override {
        return {"TextureManager", "VideoManager"};
    }
    
protected:
    void onStartUp() override {
        std::cout << "  [RenderManager] 初始化渲染器..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [RenderManager] 关闭渲染器..." << std::endl;
    }
};

class InputManager : public Subsystem {
public:
    InputManager() : Subsystem("InputManager") {}
    
    // 没有依赖
    std::vector<std::string> getDependencies() const override {
        return {};
    }
    
protected:
    void onStartUp() override {
        std::cout << "  [InputManager] 初始化输入设备..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [InputManager] 关闭输入设备..." << std::endl;
    }
};

class GameManager : public Subsystem {
public:
    GameManager() : Subsystem("GameManager") {}
    
    // 依赖渲染管理器和输入管理器
    std::vector<std::string> getDependencies() const override {
        return {"RenderManager", "InputManager"};
    }
    
protected:
    void onStartUp() override {
        std::cout << "  [GameManager] 初始化游戏逻辑..." << std::endl;
        std::cout << "  [GameManager] 加载游戏资源..." << std::endl;
    }
    
    void onShutDown() override {
        std::cout << "  [GameManager] 保存游戏状态..." << std::endl;
        std::cout << "  [GameManager] 卸载游戏资源..." << std::endl;
    }
};

// 全局子系统实例
MemoryManager gMemoryManager;
FileSystemManager gFileSystemManager;
VideoManager gVideoManager;
TextureManager gTextureManager;
RenderManager gRenderManager;
InputManager gInputManager;
GameManager gGameManager;

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Demo 3: 使用依赖图的子系统管理" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // 1. 获取管理器实例
    DependencyGraphManager& manager = DependencyGraphManager::getInstance();
    
    // 2. 注册所有子系统（顺序完全不重要！）
    std::cout << "--- 注册子系统 ---" << std::endl;
    manager.registerSubsystem(&gGameManager);
    manager.registerSubsystem(&gRenderManager);
    manager.registerSubsystem(&gInputManager);
    manager.registerSubsystem(&gMemoryManager);
    manager.registerSubsystem(&gTextureManager);
    manager.registerSubsystem(&gFileSystemManager);
    manager.registerSubsystem(&gVideoManager);
    
    try {
        // 3. 按依赖关系自动计算启动顺序并启动
        manager.startAll();
        
        // 4. 模拟游戏运行
        std::cout << "--- 游戏运行中... ---" << std::endl;
        std::cout << std::endl;
        
        // 5. 按依赖关系的逆序终止
        manager.shutDownAll();
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "========================================" << std::endl;
    std::cout << "  Demo 3 完成！" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
