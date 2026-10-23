# Smart Vehicle ECU System

A portable C99 software simulation of four automotive ECU messages, a bounded priority queue and a supervisory master. It runs on Linux or macOS without hardware. Sensor boundary checks, deterministic packet-loss injection, malformed-message rejection and communication-loss diagnostics are tested independently.

## Build and run

Requires a C compiler, make and Python 3. Sanitized tests require Clang or GCC with AddressSanitizer and UndefinedBehaviorSanitizer.

```sh
make
./vehicle_ecu 90 10 8 1
make test CC=clang
```

Arguments are impact (0–100), obstacle distance (0–255 cm), fuel (0–100 percent) and tamper (0 or 1). Malformed numbers, out-of-range values, missing option values and unknown options fail before simulation starts.

| Message | Standard CAN ID | Alert predicate |
| --- | --- | --- |
| Airbag | 0x100 | impact > 80 |
| Obstacle | 0x110 | distance < 20 |
| Anti-theft | 0x120 | tamper == 1 |
| Fuel | 0x130 | fuel < 15 |

Lower identifiers are dequeued first. The queue holds eight classical CAN-shaped frames with up to eight payload bytes. Known application messages require exactly one byte and validate its domain before it can refresh communication status or trigger an action. Unknown standard IDs are logged separately. Communication-loss detection means a message was absent from the current processing batch; there is no elapsed-time heartbeat timeout in this simulator. Diagnostic codes are local demonstration labels, not a production vehicle diagnostic specification.

## Fault and scheduling experiments

```sh
./vehicle_ecu 90 10 8 1 --error-rate 0.5 --seed 123
./vehicle_ecu 50 50 50 0 --fault-inject
./vehicle_ecu 90 50 8 0 --starve-test
./vehicle_ecu 90 50 8 0 --starve-test --round-robin
```

`--error-rate` accepts a finite value in [0,1]. A fixed xorshift32 generator makes each seed reproducible; the default is 1 and seed 0 maps to 1. Dropped messages model loss at the software queue boundary. They do not model CAN electrical collisions, error counters, CRC or retransmission.

`--fault-inject` submits an invalid DLC, a valid unknown ID, an empty known-message payload and an out-of-domain payload. Tests require invalid messages to be rejected without triggering a false airbag action or damaging valid traffic.

The starvation experiment limits the master to two dequeued frames per cycle while higher-priority traffic keeps arriving. `--round-robin` rotates the software dequeue ordering after the last served ID. This is a queue scheduling experiment, not a change to CAN's physical arbitration or a full RTOS scheduler. UART and I2C routines print demonstration messages; they are not peripheral drivers.

## Verification

`make test` runs sanitized transport/application validation, six sanitized full-application scenarios, 20 CSV boundary vectors, malformed-traffic integration, 1,000 process-level regression iterations, starvation comparisons and CLI rejection/reproducibility tests.

The C tests cover null/invalid arguments, standard-ID/DLC bounds, full queues, partial draining, priority order, round-robin progress, all 256 possible payload values for each of four known IDs, deterministic loss and complete loss. The CLI tests reject 19 invalid invocations and compare three repeated seeds. CI runs the same target on Ubuntu; local results are recorded in `evidence/validation.md`. A workflow file does not itself establish a successful hosted CI run.

| Component | Responsibility |
| --- | --- |
| `src/ecu_*.c` | Sensor-to-message encoding |
| `src/protocol_can.c` | Bounded queue, software priority/fairness, deterministic loss |
| `src/vehicle_logic.c` | Message schema and payload validation |
| `src/master_node.c` | Strict CLI parsing and batch-based actions/communication checks |
| `src/dtc_logger.c` | Timestamped diagnostic file output |
| `tests/` | Unit, sanitizer, boundary, fault, repeatability and scheduling checks |

## Scope

This is software-in-the-loop verification. It does not establish physical CAN timing, bus throughput, latency guarantees, HIL behavior, actuator safety, AUTOSAR compliance or suitability for a real airbag/braking controller. No board, transceiver, oscilloscope or logic analyzer was used. There is no measured UART/I2C/CAN throughput comparison.

MIT licensed; see `LICENSE`.
