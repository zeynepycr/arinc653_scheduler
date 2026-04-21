CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
TARGET   = scheduler
SRCS     = src/main.cpp src/FlightControl.cpp src/Display.cpp \
           src/Scheduler.cpp src/HealthMonitor.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
