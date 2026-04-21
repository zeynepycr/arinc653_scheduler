# ARINC 653 Partitioned Scheduler — C++ Simulation

A simplified ARINC 653-style scheduler demonstrating time partitioning, space
partitioning, deterministic execution, and fault isolation in C++17.

---

## Table of Contents

1. [Project Structure](#project-structure)
2. [Build & Run](#build--run)
3. [Expected Output](#expected-output)
4. [Design Decisions](#design-decisions)
5. [Theoretical Questions](#theoretical-questions)

---

## Project Structure

```
arinc653_scheduler/
├── include/
│   ├── Partition.h        # Abstract base class for all partitions
│   ├── FlightControl.h    # High-criticality partition (60 ms window)
│   ├── Display.h          # Low-criticality partition  (40 ms window)
│   ├── Scheduler.h        # Fixed-cyclic deterministic scheduler
│   └── HealthMonitor.h    # Optional fault logging module
├── src/
│   ├── main.cpp           # Entry point; wires partitions into scheduler
│   ├── FlightControl.cpp
│   ├── Display.cpp
│   ├── Scheduler.cpp
│   └── HealthMonitor.cpp
├── CMakeLists.txt         # CMake build (recommended)
├── Makefile               # Alternative: plain make
└── README.md
```

---

## Build & Run

### Option A — Makefile (quickest)

```bash
make
./scheduler
```

Clean build:

```bash
make clean && make
```

### Option B — CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
./scheduler
```

### Requirements

- C++17-compatible compiler: GCC 7+, Clang 5+, or MSVC 2017+
- `make` or `cmake` (CMake ≥ 3.14)
- No external dependencies — standard library only

---

## Expected Output

```
========================================
  ARINC 653 Scheduler — Starting
  Cycle time: 100 ms | Cycles: 5
  Partitions registered: 2
========================================

[Cycle 1 | 13:29:15]
  Flight Control running... [altitude=10001.5 m, velocity=249.9 m/s]
  Display running... [screen=Frame-1]

[Cycle 2 | 13:29:15]
  Flight Control running... [altitude=10003.0 m, velocity=249.8 m/s]
  [HEALTH MONITOR] Display ERROR: Display failure on cycle 2!

[Cycle 3 | 13:29:15]
  Flight Control running... [altitude=10004.5 m, velocity=249.7 m/s]
  Display running... [screen=Frame-3]

[Cycle 4 | 13:29:15]
  Flight Control running... [altitude=10006.0 m, velocity=249.6 m/s]
  Display running... [screen=Frame-4]

[Cycle 5 | 13:29:16]
  Flight Control running... [altitude=10007.5 m, velocity=249.5 m/s]
  Display running... [screen=Frame-5]

========================================
  Scheduler complete. All 5 cycles finished.
========================================
```

Key observations:
- Cycle 2: Display throws an exception; the error is caught and logged.
- Flight Control continues uninterrupted across all 5 cycles.
- Execution order is deterministic: Flight Control always runs before Display.

---

## Design Decisions

### 1. Abstract `Partition` base class

All partitions derive from `Partition`, which exposes only `execute(int cycle)`
and `getName()`. The scheduler holds a `vector<unique_ptr<Partition>>` and knows
nothing about the concrete types — adding a new partition requires zero changes
to the scheduler.

### 2. Space isolation enforced by class boundaries

`FlightControl` owns `altitude` and `velocity`; `Display` owns `screenData`.
These are private members with no shared globals anywhere in the codebase.
This directly mirrors ARINC 653 memory partitioning: a bug in the Display
partition cannot corrupt Flight Control's data.

### 3. Deterministic fixed-cyclic scheduling

The scheduler iterates `partitions` in registration order every cycle — no
priority queues, no preemption, no dynamic reordering. This matches the ARINC
653 Major Frame / Minor Frame model: the execution order is known statically
and never changes at runtime.

### 4. Fault isolation via per-partition `try-catch`

Each call to `partition->execute()` is individually wrapped in a `try-catch`.
An exception in one partition is logged to `stderr` (Health Monitor channel)
and execution immediately continues with the next partition. The scheduler
itself never crashes. This is the software analogue of ARINC 653 Health Monitor
fault containment.

### 5. Time windows via `std::this_thread::sleep_for`

`FlightControl::execute()` sleeps 60 ms; `Display::execute()` sleeps 40 ms —
summing to the 100 ms system cycle time. In a real ARINC 653 RTOS the kernel
enforces these windows with hardware timers and will forcibly preempt a
partition that overruns. Here, `sleep_for` simulates the allocated window
without the kernel mechanism.

### 6. Timestamped logging

Each cycle header prints a wall-clock timestamp (`HH:MM:SS`) so log output can
be correlated with real time. In production this would use a monotonic clock
referenced to system startup.

---

## Theoretical Questions

### A1 — What is an Interface in IMODE?

An **Interface** in IMODE defines the communication contract between components.
It specifies what data or services a component exposes without revealing internal
implementation details.

Two purposes:
1. **Decoupling**: components from different vendors can interact as long as both
   conform to the agreed interface, regardless of internal implementation.
2. **Verification**: the interface can be formally checked (e.g. via model
   checking) to ensure that connected components are compatible before
   integration, reducing integration-time defects.

---

### B1 — JDK vs JRE vs JVM

| | Scope | Contains | Who needs it |
|---|---|---|---|
| **JVM** | Runtime engine | Bytecode interpreter, JIT compiler, garbage collector | Nobody installs it alone |
| **JRE** | Runtime environment | JVM + Java standard libraries (java.lang, java.util, …) | End users running Java apps |
| **JDK** | Development kit | JRE + `javac` compiler + `javadoc` + `jar` + debugger tools | Developers building Java apps |

The nesting is: **JDK ⊃ JRE ⊃ JVM**.

A developer writes `.java` source → `javac` (JDK) compiles it to `.class`
bytecode → the JVM (inside the JRE) executes that bytecode on the target
machine. Because the JVM is the only platform-specific layer, the same `.class`
file runs on Windows, Linux, or macOS without recompilation ("write once, run
anywhere").

---

### C1 — Why are time and space partitioning critical in ARINC 653?

Modern avionics platforms run multiple applications (flight control, navigation,
displays, ACARS) on a single CPU under Integrated Modular Avionics (IMA).
Without isolation, a single software fault could cascade across every hosted
application — potentially catastrophic on a safety-critical platform.

**Time partitioning** solves the CPU starvation problem. Each partition is
allocated a fixed, guaranteed time window in every Major Frame. A runaway loop
or unexpected computation in the Display application cannot consume CPU time
that belongs to Flight Control. The RTOS kernel enforces these windows with
hardware timers and will forcibly preempt any partition that overruns, logging
the event to the Health Monitor.

**Space partitioning** solves the memory corruption problem. Each partition's
memory regions (code, data, stack, heap) are mapped with hardware MMU protection
so that a pointer bug or buffer overflow in one partition raises a memory
protection fault rather than silently corrupting a neighbour's data. Flight
Control's navigation variables are physically unreachable from the Display
partition.

Together they implement **fault containment**: a failure in a low-criticality
partition cannot propagate to a high-criticality one. This is a prerequisite for
DO-178C Level A certification, which covers software whose failure would
contribute to a catastrophic aircraft failure condition.
