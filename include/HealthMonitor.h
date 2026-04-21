#pragma once
#include <string>
#include <vector>
#include <chrono>

/**
 * Optional health monitoring module.
 * Records fault events with timestamps for post-run inspection.
 */
struct FaultEvent {
    int cycle;
    std::string partitionName;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
};

class HealthMonitor {
private:
    std::vector<FaultEvent> faultLog;

public:
    void recordFault(int cycle, const std::string& partitionName, const std::string& message);
    void printReport() const;
    int faultCount() const;
};
