CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# 目标文件
DEMO1 = demo1_simple
DEMO2 = demo2_priority
DEMO3 = demo3_dependency

# 源文件
SRC1 = demo1_simple.cpp
SRC2 = demo2_priority.cpp
SRC3 = demo3_dependency.cpp

.PHONY: all clean run run1 run2 run3 help

# 默认目标：编译所有
all: $(DEMO1) $(DEMO2) $(DEMO3)

# 编译 demo1
$(DEMO1): $(SRC1)
	$(CXX) $(CXXFLAGS) -o $(DEMO1) $(SRC1)

# 编译 demo2
$(DEMO2): $(SRC2)
	$(CXX) $(CXXFLAGS) -o $(DEMO2) $(SRC2)

# 编译 demo3
$(DEMO3): $(SRC3)
	$(CXX) $(CXXFLAGS) -o $(DEMO3) $(SRC3)

# 运行所有 demo
run: run1 run2 run3

# 运行 demo1
run1: $(DEMO1)
	@echo ""
	@echo "========================================"
	@echo "  运行 Demo 1: 简单的子系统启动和终止"
	@echo "========================================"
	@echo ""
	./$(DEMO1)

# 运行 demo2
run2: $(DEMO2)
	@echo ""
	@echo "========================================"
	@echo "  运行 Demo 2: 使用优先级队列"
	@echo "========================================"
	@echo ""
	./$(DEMO2)

# 运行 demo3
run3: $(DEMO3)
	@echo ""
	@echo "========================================"
	@echo "  运行 Demo 3: 使用依赖图"
	@echo "========================================"
	@echo ""
	./$(DEMO3)

# 清理编译文件
clean:
	rm -f $(DEMO1) $(DEMO2) $(DEMO3)
	rm -f *.o

# 帮助信息
help:
	@echo "可用的 make 目标："
	@echo "  make all       - 编译所有 demo"
	@echo "  make run       - 运行所有 demo"
	@echo "  make run1      - 仅运行 Demo 1（简单方式）"
	@echo "  make run2      - 仅运行 Demo 2（优先级队列）"
	@echo "  make run3      - 仅运行 Demo 3（依赖图）"
	@echo "  make clean     - 清理编译文件"
	@echo "  make help      - 显示此帮助信息"
	@echo ""
	@echo "Demo 说明："
	@echo "  Demo 1 - 最简单的方式：手动在 main() 中按顺序调用 startUp/shutDown"
	@echo "  Demo 2 - 使用优先级队列：子系统注册优先级，自动排序启动"
	@echo "  Demo 3 - 使用依赖图：子系统声明依赖，拓扑排序自动计算启动顺序"
