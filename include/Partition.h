#pragma once
#include <string>

/**
 * Abstract base class for all ARINC 653 partitions.
 * Each partition encapsulates its own data and logic,
 * enforcing space isolation by design.
 */
class Partition {
public:
    virtual void execute(int cycle) = 0;
    virtual std::string getName() const = 0;
    virtual ~Partition() = default;
};
