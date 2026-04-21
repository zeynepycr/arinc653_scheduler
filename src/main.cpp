#include "Scheduler.h"
#include "FlightControl.h"
#include "Display.h"
#include <memory>

// On Windows, sleep_for uses the system timer which defaults to ~15 ms resolution.
// timeBeginPeriod(1) raises it to 1 ms so sleep durations are accurate.
#ifdef _WIN32
  #include <windows.h>
  #pragma comment(lib, "winmm.lib")
#endif

int main() {
#ifdef _WIN32
    timeBeginPeriod(1);
#endif

    // Create scheduler: 5 cycles, 100 ms each
    Scheduler scheduler(5);

    // Register partitions in strict execution order
    // Flight Control (60 ms) runs before Display (40 ms) — total = 100 ms
    scheduler.addPartition(std::make_unique<FlightControl>());
    scheduler.addPartition(std::make_unique<Display>(2)); // fault injected on cycle 2

    scheduler.run();

#ifdef _WIN32
    timeEndPeriod(1);
#endif
    return 0;
}