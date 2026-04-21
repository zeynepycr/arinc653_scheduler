#include "FlightControl.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

FlightControl::FlightControl()
    : altitude(10000.0), velocity(250.0)
{}

void FlightControl::execute(int /*cycle*/) {
    // Simulate ~42 ms time window (reduced for output overhead)
    std::this_thread::sleep_for(std::chrono::milliseconds(42));

    // Safety-critical computation (simplified simulation)
    altitude += 1.5;
    velocity -= 0.1;

    std::cout << "  Flight Control running... "
              << std::fixed << std::setprecision(1)
              << "[altitude=" << altitude << " m"
              << ", velocity=" << velocity << " m/s]\n";
}

std::string FlightControl::getName() const {
    return "Flight Control";
}
