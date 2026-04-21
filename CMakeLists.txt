cmake_minimum_required(VERSION 3.14)
project(ARINC653_Scheduler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(scheduler
    src/main.cpp
    src/FlightControl.cpp
    src/Display.cpp
    src/Scheduler.cpp
    src/HealthMonitor.cpp
)

target_compile_options(scheduler PRIVATE -Wall -Wextra -Wpedantic)
