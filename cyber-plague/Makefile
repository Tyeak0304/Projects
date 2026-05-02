CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
TARGET   = Game
SRCS     = src/main.cpp \
           src/network/Network.cpp \
           src/nodes/Node.cpp \
           src/policies/HardenedPolicy.cpp \
           src/policies/StandardPolicy.cpp \
           src/io/InputParser.cpp \
           src/io/OutputWriter.cpp \
           src/simulation/Simulator.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) Results/infection_results.csv
