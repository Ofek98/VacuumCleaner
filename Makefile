CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic
TARGET = myrobot
SRCDIR = src
OUTDIR = bin

$(OUTDIR)/$(TARGET): $(SRCDIR)/myrobot.cpp $(SRCDIR)/common.cpp $(SRCDIR)/House.cpp $(SRCDIR)/Simulator.cpp $(SRCDIR)/Algorithm.cpp $(SRCDIR)/Robot.cpp | $(OUTDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR)/$(TARGET)
