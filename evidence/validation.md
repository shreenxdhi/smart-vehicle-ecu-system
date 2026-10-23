# Recorded validation

A clean local `make test CC=clang` passed with strict compiler warnings. Compiler, host and tested application hash are recorded in `environment.json`; raw results are in `test-results.log`.

| Check | Result |
| --- | --- |
| Sanitized C core | Transport bounds, capacity, priority, partial receive, fairness, deterministic faults and all 1,024 ID/payload combinations passed |
| Sanitized full application | 6/6 scenarios |
| CSV boundary vectors | 20/20 |
| Malformed traffic | Invalid DLC, unknown ID, empty known payload and out-of-domain payload handled; no false airbag action |
| Process-level repeated regression | 1,000/1,000 iterations |
| Starvation and software fairness experiments | Both passed |
| Invalid CLI input | 19/19 rejected before simulation |
| Repeated fault seeds | Three seeds reproduced identical output |
| Total communication loss | All four batch-level loss diagnostics observed |

No sanitizer finding occurred in exercised paths. Host C static analysis completed without diagnostics on master_node.c, protocol_can.c, vehicle_logic.c and dtc_logger.c. Ubuntu CI is configured; no completed hosted run is claimed.

This is software-in-the-loop verification. Message loss is injected at a software queue, and communication presence is evaluated per batch. It does not measure physical CAN arbitration/timing, CRC, bus-off recovery, transceiver behavior, heartbeat deadlines, hardware-in-the-loop behavior, actuator safety or production diagnostic compliance. The 1,000 iterations are repeated process-level scenarios, not measured bus load or hardware endurance.
