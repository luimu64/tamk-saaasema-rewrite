# Makefile for Weather Station MQTT project

# Compiler and flags
CXX := g++
SRC := main.cpp send_to_rest.cpp mqtt.cpp
OBJ := $(SRC:.cpp=.o)

# Output binaries
TARGET := weather_station
DEBUG_TARGET := weather_station_debug

# Common flags
CXXFLAGS := -std=c++20 -pthread
LDFLAGS := -lmosquitto -lmosquittopp -lcurl -ljsoncpp

# Optimized release build
RELEASE_FLAGS := -O2

# Debug build flags
DEBUG_FLAGS := -g -Wall -Wextra

# Default target
all: $(TARGET)

# Release build
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Debug build
debug: $(SRC)
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) $(SRC) -o $(DEBUG_TARGET) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET) $(DEBUG_TARGET) *.o

.PHONY: all debug clean
