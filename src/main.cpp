#include "Scheduler.h"
#include "FlightControl.h"
#include "Display.h"
#include <memory>

int main() {
    // Create scheduler: 5 cycles, 100 ms each
    Scheduler scheduler(5);

    // Register partitions in strict execution order
    // Flight Control (60 ms) runs before Display (40 ms) — total = 100 ms
    scheduler.addPartition(std::make_unique<FlightControl>());
    scheduler.addPartition(std::make_unique<Display>(2)); // fault injected on cycle 2

    scheduler.run();
    return 0;
}
