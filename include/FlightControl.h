#pragma once
#include "Partition.h"

/**
 * Flight Control partition — HIGH criticality.
 * Owns: altitude, velocity (private, no shared globals).
 * Time window: 60 ms per cycle.
 */
class FlightControl : public Partition {
private:
    double altitude;   // meters
    double velocity;   // m/s

public:
    FlightControl();
    void execute(int cycle) override;
    std::string getName() const override;
};
