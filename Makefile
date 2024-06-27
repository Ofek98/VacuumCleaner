CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic
TARGET = myrobot
SRCDIR = src
OUTDIR = bin

# Add all cpp files from SRCDIR to a variable
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

$(OUTDIR)/$(TARGET): $(SOURCES) | $(OUTDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR)/$(TARGET)

