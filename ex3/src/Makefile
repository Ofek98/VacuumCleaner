# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -fPIC -Wall -Wextra -Werror -pedantic -g

# Target name (executable)
TARGET = myrobot

# Get all .cpp files in the current directory
ALL_SOURCES = $(wildcard *.cpp)

# Exclude files starting with "Algo_"
SOURCES = $(filter-out Algo_%.cpp, $(ALL_SOURCES))

$(TARGET): $(SOURCES)
	$(CXX) -rdynamic $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET)





