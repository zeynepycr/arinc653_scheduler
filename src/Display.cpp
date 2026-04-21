#include "Display.h"
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>

Display::Display(int failCycle)
    : screenData("INIT"), failOnCycle(failCycle)
{}

void Display::execute(int cycle) {
    // Simulate ~28 ms time window (reduced for output overhead)
    std::this_thread::sleep_for(std::chrono::milliseconds(28));

    // Controlled fault injection for fault-isolation demonstration
    if (cycle == failOnCycle) {
        throw std::runtime_error("Display failure on cycle " + std::to_string(cycle) + "!");
    }

    screenData = "Frame-" + std::to_string(cycle);
    std::cout << "  Display running... [screen=" << screenData << "]\n";
}

std::string Display::getName() const {
    return "Display";
}
