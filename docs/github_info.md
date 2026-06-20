# GitHub Profile Assets

Reference material for publishing this project to GitHub.

---

## Repository Description

> Embedded C project simulating a smart vehicle ECU network using CAN communication. Features Airbag, Obstacle Detection, Fuel Monitoring, and Anti-Theft ECUs with CAN arbitration and Python-based automated testing.

*(Maximum 350 characters — paste this into the GitHub repository description field)*

---

## GitHub Topics

Add these in the repository settings under **Topics**:

```
embedded-c
automotive
can-bus
ecu
embedded-systems
python-testing
gcc
automotive-software
c-programming
simulation
```

---

## Resume Bullet Points

- Developed a multi-ECU automotive simulation in Embedded C, implementing software-based CAN communication between Airbag, Obstacle Detection, Fuel, and Anti-Theft ECUs.
- Implemented CAN frame handling, priority-based arbitration, and Hardware-In-the-Loop (HIL) style bus error simulation with random frame dropping.
- Designed a centralized master node featuring real-time diagnostic logging (DTCs), "Degraded Mode" for comm loss, and a custom RTOS-style scheduler with Round-Robin fallback to prevent low-priority ECU starvation.
- Built a robust Python test automation suite covering boundary scenarios, CAN fault injection, Priority Starvation proofs, and 1000-cycle regression stress testing.

---

## LinkedIn Project Description

**Smart Vehicle ECU System** — Personal Project

Built a software simulation of a multi-ECU automotive network in Embedded C to get practical experience with automotive communication protocols. The project models four ECUs (Airbag, Obstacle Detection, Fuel Monitoring, Anti-Theft) each broadcasting CAN frames on a shared bus. A master control node receives the frames, applies priority-based arbitration by sorting on CAN message ID, and triggers the appropriate vehicle action.

I also implemented comparison stubs for UART and I2C to understand why CAN is the standard for vehicle-level ECU communication. The project includes a Python test suite with 20 test cases that validate every ECU threshold and boundary condition automatically.

**Skills demonstrated:** Embedded C, CAN Bus, Automotive Software, Software Testing, GCC, Makefile

---

## Notes for Repository Setup

1. Set the repository to **Public**.
2. Add the description from above under Settings → General → Description.
3. Add the topics listed above under the gear icon on the main repository page.
4. Take a terminal screenshot of `make run` output and add it to `docs/` — link it in the Screenshots section of the README.
5. Take a screenshot of `make test` output showing `Passed: 20 / Failed: 0` and add it similarly.
6. The binary (`vehicle_ecu`) and object files (`*.o`) are listed in `.gitignore` — make sure they are not committed.
