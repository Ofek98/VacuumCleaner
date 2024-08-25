# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -fPIC -Wall -Wextra -Werror -pedantic -g

# Target name (executable)
TARGET = myrobot

# Get all .cpp files in the current directory
SOURCES = $(wildcard *.cpp) ../common_algo_sim/common.cpp

$(TARGET): $(SOURCES)
	$(CXX) -rdynamic $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET)
