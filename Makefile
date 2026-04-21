CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
TARGET   = scheduler
SRCS     = src/main.cpp src/FlightControl.cpp src/Display.cpp \
           src/Scheduler.cpp src/HealthMonitor.cpp

# Link winmm on Windows for high-resolution sleep timer
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lwinmm
endif

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all clean