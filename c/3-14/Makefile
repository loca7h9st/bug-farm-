CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g
TARGET = array_conver
SOURCE = array_conver.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
