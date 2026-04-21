#include "HealthMonitor.h"
#include <iostream>
#include <iomanip>
#include <ctime>

void HealthMonitor::recordFault(int cycle,
                                const std::string& partitionName,
                                const std::string& message) {
    faultLog.push_back({
        cycle,
        partitionName,
        message,
        std::chrono::system_clock::now()
    });
}

void HealthMonitor::printReport() const {
    std::cout << "\n--- Health Monitor Report ---\n";
    if (faultLog.empty()) {
        std::cout << "  No faults recorded.\n";
        return;
    }
    for (const auto& f : faultLog) {
        std::time_t t = std::chrono::system_clock::to_time_t(f.timestamp);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
        std::cout << "  [" << buf << "] Cycle " << f.cycle
                  << " | " << f.partitionName
                  << " | " << f.message << "\n";
    }
    std::cout << "  Total faults: " << faultLog.size() << "\n";
}

int HealthMonitor::faultCount() const {
    return static_cast<int>(faultLog.size());
}
