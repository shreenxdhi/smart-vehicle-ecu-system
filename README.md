# Smart Vehicle ECU System

> Software simulation of a multi-ECU automotive system using CAN communication in Embedded C.

![Language](https://img.shields.io/badge/Language-C%20(C99)-blue)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS-lightgrey)
![Protocol](https://img.shields.io/badge/Protocol-CAN%20Bus-orange)
![Domain](https://img.shields.io/badge/Domain-Automotive%20%2F%20Embedded-green)
![Testing](https://img.shields.io/badge/Testing-Python%203-yellow)
![License](https://img.shields.io/badge/License-MIT-blue)

---

## Overview

This project simulates a small automotive ECU network running entirely on a PC — no hardware needed. Four independent ECUs each read a sensor value, pack it into a CAN frame, and put it on a shared bus. A master node collects the frames, sorts them by CAN ID (simulating bus arbitration), and triggers vehicle-level actions like deploying the airbag or activating an alarm.

This project was built to get hands-on with how automotive software is structured — ECU separation, CAN message priorities, and event-driven decision making.

---

## Highlights

- Simulated four independent automotive ECUs (Airbag, Obstacle, Fuel, Anti-Theft)
- Software-emulated CAN bus with arbitration via CAN ID priority sorting
- Centralized master node processes frames and issues vehicle commands
- Comparison demos for UART and I2C included alongside CAN
- 20 automated test cases driven by a Python test script and CSV vectors
- Compiles cleanly with GCC under `-Wall -Wextra` with no warnings

---

## Project Structure

```
ecu/
├── src/
│   ├── common.h            # CAN_Frame struct, CAN IDs, alert thresholds
│   ├── protocol_can.h      # CAN module interface
│   ├── protocol_can.c      # Bus buffer, send/receive, arbitration sort
│   ├── protocol_uart.c     # UART stub (comparison demo)
│   ├── protocol_i2c.c      # I2C stub (comparison demo)
│   ├── ecu_airbag.c        # Airbag ECU — CAN ID 0x100
│   ├── ecu_obstacle.c      # Obstacle Detection ECU — CAN ID 0x110
│   ├── ecu_antitheft.c     # Anti-Theft ECU — CAN ID 0x120
│   ├── ecu_fuel.c          # Fuel Level ECU — CAN ID 0x130
│   └── master_node.c       # Main program, CAN frame processing
├── tests/
│   ├── test_vehicle_system.py
│   └── test_vectors.csv
├── docs/
│   └── github_info.md
├── Makefile
├── LICENSE
├── .gitignore
└── README.md
```

---

## Architecture

![Smart Vehicle ECU System Architecture](docs/architecture.png)

---

## CAN Message IDs

| ECU             | CAN ID | Priority | Triggers When    |
|-----------------|--------|----------|------------------|
| Airbag ECU      | 0x100  | Highest  | impact > 80      |
| Obstacle ECU    | 0x110  | High     | distance < 20 cm |
| Anti-Theft ECU  | 0x120  | Medium   | tamper == 1      |
| Fuel ECU        | 0x130  | Lowest   | fuel < 15%       |

Lower CAN ID = higher priority. The master node processes higher-priority frames first.

---

## Build Instructions

Requires `gcc` and `make`.

```bash
make          # build the project
make run      # build and run with all-alerts scenario
make test     # build and run the full test suite
make clean    # remove compiled output
```

---

## Run Instructions

```bash
./vehicle_ecu <impact> <distance_cm> <fuel_pct> <tamper>
```

| Argument      | Range  | Alert condition |
|---------------|--------|-----------------|
| `impact`      | 0–100  | > 80            |
| `distance_cm` | 0–255  | < 20            |
| `fuel_pct`    | 0–100  | < 15            |
| `tamper`      | 0 or 1 | == 1            |

```bash
./vehicle_ecu 90 10 8 1    # all four alerts
./vehicle_ecu 90 50 70 0   # airbag only
./vehicle_ecu 50 50 50 0   # all systems normal
```

---

## Sample Output

```
Starting ECU simulation...

[ECU: Airbag] Impact sensor: 90
[ECU: Airbag] Crash detected!
[CAN TX] ID: 0x100 | DLC: 1 | Data: 90
[ECU: Obstacle] Distance sensor: 10 cm
[ECU: Obstacle] Obstacle too close!
[CAN TX] ID: 0x110 | DLC: 1 | Data: 10
[ECU: Anti-Theft] Tamper flag: 1
[ECU: Anti-Theft] Tamper detected!
[CAN TX] ID: 0x120 | DLC: 1 | Data: 1
[ECU: Fuel] Tank level: 8%
[ECU: Fuel] Fuel level critical!
[CAN TX] ID: 0x130 | DLC: 1 | Data: 8
[CAN RX] Received 4 frame(s)

--- Processing 4 CAN Frames ---
  *** ACTION: DEPLOY AIRBAG ***
  *** ACTION: EMERGENCY BRAKE ***
  *** ACTION: ALARM ON ***
  *** ACTION: LOW FUEL WARNING ***

--- SYSTEM STATUS ---
  4 ALERT(S) ACTIVE
```

---

## Screenshots

### Program Output

![Program Output](docs/program_output.png)

### Test Results

![Test Results](docs/test_results.png)

---

## Testing

The test suite reads `tests/test_vectors.csv` and runs the binary for each row, checking that the expected keyword appears in output.

```bash
make test
```

```
  Test #01  PASS  (DEPLOY AIRBAG)
  Test #02  PASS  (DEPLOY AIRBAG)
  ...
  Test #20  PASS  (ALARM ON)

----------------------------
  Tests Run : 20
  Passed    : 20
  Failed    : 0
----------------------------
```

Tests cover:
- Individual ECU thresholds (on-boundary and off-boundary)
- No-alert baseline cases
- All four ECUs firing simultaneously

---

## Why CAN Instead of UART or I2C?

I initially considered UART, but it's strictly point-to-point — not practical when you have multiple ECUs sharing data. I2C allows a shared bus but was designed for short PCB traces and has no real arbitration. CAN handles collisions automatically and works across the full vehicle harness.

| Feature           | CAN                     | UART              | I2C                  |
|-------------------|-------------------------|-------------------|----------------------|
| Topology          | Multi-node shared bus   | Point-to-point    | Multi-node bus       |
| Arbitration       | Built-in (by CAN ID)    | None              | None (master-driven) |
| Broadcast         | Yes                     | No                | No                   |
| Cable length      | Up to 1 km              | ~15 m             | < 1 m (PCB only)     |
| Error detection   | CRC-15 + bit monitoring | Parity bit only   | ACK/NACK only        |
| Automotive use    | ECU-to-ECU standard     | Debug/logging     | On-board chips only  |

---

## Future Improvements

- Add an Engine ECU broadcasting RPM and throttle position
- Simulate an instrument cluster subscribing to multiple CAN IDs
- Add basic fault code logging (DTC simulation)
- Port the ECU logic to an STM32 and test with a real CAN transceiver

---

## License

MIT — see [LICENSE](LICENSE).
