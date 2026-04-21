#pragma once
#include "Partition.h"

/**
 * Display System partition — LOW criticality.
 * Owns: screenData (private, no shared globals).
 * Time window: 40 ms per cycle.
 * Simulates a controlled fault on a configurable cycle.
 */
class Display : public Partition {
private:
    std::string screenData;
    int failOnCycle;

public:
    /**
     * @param failCycle  Cycle number on which a simulated fault is thrown.
     *                   Set to -1 to disable fault injection.
     */
    explicit Display(int failCycle = 2);
    void execute(int cycle) override;
    std::string getName() const override;
};
