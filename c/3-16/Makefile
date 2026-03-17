CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# 目标文件
LIST_TEST = list_test
LRU_TEST = lru_test

# 源文件
LIST_SRC = list_test.cpp
LRU_SRC = lru_test.cpp

.PHONY: all clean run run_list run_lru

# 默认目标：编译所有
all: $(LIST_TEST) $(LRU_TEST)

# 编译链表测试
$(LIST_TEST): $(LIST_SRC) list.hpp
	$(CXX) $(CXXFLAGS) -o $(LIST_TEST) $(LIST_SRC)

# 编译 LRU 缓存测试
$(LRU_TEST): $(LRU_SRC) list.hpp lru_cache.hpp
	$(CXX) $(CXXFLAGS) -o $(LRU_TEST) $(LRU_SRC)

# 运行所有测试
run: run_list run_lru

# 运行链表测试
run_list: $(LIST_TEST)
	@echo "========================================"
	@echo "       运行链表测试"
	@echo "========================================"
	./$(LIST_TEST)

# 运行 LRU 缓存测试
run_lru: $(LRU_TEST)
	@echo ""
	@echo "========================================"
	@echo "       运行 LRU 缓存测试"
	@echo "========================================"
	./$(LRU_TEST)

# 清理编译文件
clean:
	rm -f $(LIST_TEST) $(LRU_TEST)
	rm -f *.o

# 帮助信息
help:
	@echo "可用的 make 目标："
	@echo "  make all       - 编译所有测试程序"
	@echo "  make run       - 运行所有测试"
	@echo "  make run_list  - 仅运行链表测试"
	@echo "  make run_lru   - 仅运行 LRU 缓存测试"
	@echo "  make clean     - 清理编译文件"
	@echo "  make help      - 显示此帮助信息"
