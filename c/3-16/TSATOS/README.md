# 子系统的启动和终止
由于多数新式游戏引擎皆采用C++为编程语言，我们应考虑一下，C++原生的启动及终止语义是否可做启动及终止引擎子系统之用。在C++中，在调用程序进入点(main()或Win-dows 下的winMain())之前，全局及静态对象已被构建。然而，我们完全不可预知这些构造函数的调用次序。1在main()或WinMain()结束返回之后，会调用全局及静态对象的析构函数，而这些函数的调用次序也是无法预知的。显而易见，此C+十行为并不适合用来初始化及终止游戏引擎的子系统。实际上，这对任何含互相依赖全局对象的软件都不适合。
这实在令人遗憾，因为要实现各主要子系统，例如游戏引擎中的子系统，常见的设计模式是为每个子系统定义单例类(singleton class)，通常称作管理器(manager)。若C++能给予我们更多控制能力，指明全局或静态实例的构建、析构次序，那么我们就可以把单例定义为全局变量，而不必使用动态内存分配。例如，各子系统可写成以下形式:
```c++
class RenderManager {
    public:
    //取得唯一实例
    static RenderManager& get () {
        static RenderManager sSingletion;
        return sSingletion;
    }
    RenderManager(){
        // 对需要依赖管理器，先通过调用它们的get() 启动它们
        VideoManager::get ();
        TextureManager::get ();
        // 现在在启动渲染管理器
    }

    -RenderManager()
    {

    }

};

```

遗憾的是，此方法不可控制析构次序。例如，在RenderManager 析构之前，其依赖的单例可能已被析构。而且，很难预计RenderManager单例的确切构建时间，因为第一次调用RenderManager::get()时，单例就会被构建，天知道那是什么时候!此外，使用该类的程序员可能不会预期，貌似无伤大雅的get()函数可能会有很高的开销，例如，分配及初始化一个重量级的单例。此法仍是难以预计且危险的设计。这促使我们诉诸更直接、有更大控制权
的方法。

假设我们对子系统继续采用单例管理器的概念。最简单的“蛮力”方法就是，明确地为各单例管理器类定义启动和终止函数。这些函数取代了构造函数和析构函数，实际上，我们会让构造函数和析构函数完全不做任何事情。这样的话，就可以在main()中(或某个管理整个引擎的单例中)，按所需的明确次序调用各启动和终止函数。例如:

```c++

class RenderManager
{
public:
    RenderManager()
    {
        //不做事情
    }
    
    ~RenderManager()
    {
        //不做事情
    }
    
    void startUp()
    {
        //启动管理器
    }
    
    void shutDown()
    {
        //终止管理器
    }
    
};

class PhysicsManager    {/*类似内容......*/};
class AnimationManager  {/*类似内容......*/};
class MemoryManager     {/*类似内容......*/};

RenderManager
PhysicsManager
AnimationManager
TextureManager
VideoManager
MemoryManager
FileSystemManager
gRenderManager;
gPhysicsManager;
gAnimationManager;
gTextureManager;
gVideoManager;
gMemoryManager;
gFileSysteManager;
// ...

int main (int argc, const char* argv)
{
    //以正确次序启动各引擎系统
    gMemoryManager.startUp ();
    gFileSystemManager.startUp ();gVideoManager.startUp ();gTextureManager.startUp ();
    gRenderManager.startUp () ;
    gAnimationManager.startUp ();
    gPhysicsManager.startUp () ;
    //运行游戏
    gSimulationManager.run ();
    //以反向次序终止各引擎系统
    gPhysicsManager.shutDown ();
    gAnimationManager.shutDown ();
    gRenderManager.shutDown ();
    gTextureManager.shutDown ();
    gVideoManager.shutDown () ;
    gFileSystemManager.shutDown ();
    gMemoryManager.shutDown ();
    return 0;
}


```
此法还有“更优雅”的实现方式。例如，可以让各管理器把自己登记在一个全局的优先队列(priority queue)中，之后再按恰当次序逐一启动所有管理器。此外，也可以通过每个管理器列举其依赖的管理器，定义一个管理器间的依赖图(dependency graph)，然后按互相依赖关系计算最优的启动次序。
- 此方法既简单又容易实现。
- 此方法是明确的。看看代码就能立即得知启动次序。
- 此方法容易调试和维护。

若某子系统启动时机不够早或过早，只需移动一行代码。用蛮力方法手动启动及终止子系统，还有一个小缺点，就是程序员有可能意外地终止一些子系统，而非按启动的相反次序。但这一缺点并不会使笔者失眠，因为只要能成功启动及终止引擎的各子系统，你的任务就完成了。