#pragma once
#include "Partition.h"
#include <vector>
#include <memory>

/**
 * Fixed-cyclic scheduler — deterministic execution, no preemption.
 * Partitions run in registration order every cycle.
 * Faults in one partition are caught and logged; others continue.
 *
 * Total cycle time: 100 ms (enforced by individual partition windows).
 */
class Scheduler {
private:
    std::vector<std::unique_ptr<Partition>> partitions;
    int totalCycles;

public:
    explicit Scheduler(int cycles = 5);
    void addPartition(std::unique_ptr<Partition> p);
    void run();
};
