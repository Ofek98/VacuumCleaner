CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -g
TARGET = myrobot

# Add all cpp files from SRCDIR to a variable
SOURCES = $(wildcard *.cpp)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET)

