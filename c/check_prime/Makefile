CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# 目标文件
RPIME_TEST = check_prime
# 源文件

PRIME_SRC = is_prime.cpp

.PHONY: all clean



all: $(RPIME_TEST)


$(RPIME_TEST): $(PRIME_SRC)
	$(CXX) $(CXXFLAGS) $(PRIME_SRC) -o $(RPIME_TEST)


clean:
	rm $(RPIME_TEST)
