# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -fPIC -Wall -Wextra -Werror -pedantic -g

# Shared library flags
LDFLAGS = -shared

# Target name (shared object)
TARGET = Algo_214166027_206388969_A.so

# Source files
SRC = Algo_214166027_206388969_A.cpp ../common_algorithm/CommonAlgorithm.cpp ../common_algo_sim/common.cpp

# Object files
OBJ = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to create the shared object
$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

# Rule to compile the object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)
