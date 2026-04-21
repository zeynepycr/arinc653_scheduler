#include "Scheduler.h"
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>

using Clock = std::chrono::steady_clock;
using Ms    = std::chrono::duration<double, std::milli>;

const int CYCLE_TIME_MS = 100;  // Target cycle time in milliseconds

//using namespace std;

Scheduler::Scheduler(int cycles)
    : totalCycles(cycles)
{}

void Scheduler::addPartition(std::unique_ptr<Partition> p) {
    partitions.push_back(std::move(p));
}

void Scheduler::run() {
    std::cout << "========================================\n";
    std::cout << "  ARINC 653 Scheduler - Starting\n";
    std::cout << "  Cycle time: 100 ms | Cycles: " << totalCycles << "\n";
    std::cout << "  Partitions registered: " << partitions.size() << "\n";
    std::cout << "========================================\n";

    for (int cycle = 1; cycle <= totalCycles; ++cycle) {
        auto cycleStart = Clock::now();

        // Wall-clock timestamp for cycle header
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        char timebuf[20];
        std::strftime(timebuf, sizeof(timebuf), "%H:%M:%S", std::localtime(&t));

        std::cout << "\n[Cycle " << cycle << " | " << timebuf << "]\n";

        auto execStart = Clock::now();

        // Execute each partition in fixed order (deterministic)
        for (auto& partition : partitions) {
            auto partStart = Clock::now();
            try {
                partition->execute(cycle);
            } catch (const std::exception& e) {
                std::cerr << "  [HEALTH MONITOR] "
                          << partition->getName()
                          << " ERROR: " << e.what() << "\n";
            }
            double partMs = Ms(Clock::now() - partStart).count();
            std::cout << "    >> " << partition->getName()
                      << " window: " << std::fixed << std::setprecision(1)
                      << partMs << " ms\n";
        }

        double execMs = Ms(Clock::now() - execStart).count();
        std::cout << "  -- Execution time: " << std::fixed << std::setprecision(1)
                  << execMs << " ms --\n";

        // Enforce strict 100 ms cycle time
        double elapsedMs = Ms(Clock::now() - cycleStart).count();
        double remainingMs = CYCLE_TIME_MS - elapsedMs;
        
        if (remainingMs > 1.0) {
            std::this_thread::sleep_for(Ms(remainingMs - 1.0));  // Leave 1 ms buffer
        }

        // Report actual wall-clock cycle time (should be ~100 ms)
        double actualCycleMs = Ms(Clock::now() - cycleStart).count();
        std::cout << "  -- Total cycle time: " << std::fixed << std::setprecision(1)
                  << actualCycleMs << " ms (100 ms enforced) --\n";
    }

    std::cout << "\n========================================\n";
    std::cout << "  Scheduler complete. All " << totalCycles << " cycles finished.\n";
    std::cout << "========================================\n";
}