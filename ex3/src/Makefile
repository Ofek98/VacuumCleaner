CXX = g++
CXXFLAGS = -std=c++20 -fPIC -Wall -Wextra -Werror -pedantic -g
TARGET = myrobot

# Get all .cpp files in the current directory
ALL_SOURCES = $(wildcard *.cpp)

# Exclude files starting with "Algo_"
SOURCES = $(filter-out Algo_%.cpp, $(ALL_SOURCES))

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET)

