#!/usr/bin/env python3

import subprocess
import os
import sys

# binary is built at project root, one level up from tests/
BINARY = "../vehicle_ecu"
ITERATIONS = 1000

EXPECTED_ALERTS = [
    "*** ACTION: DEPLOY AIRBAG ***",
    "*** ACTION: EMERGENCY BRAKE ***",
    "*** ACTION: ALARM ON ***",
    "*** ACTION: LOW FUEL WARNING ***"
]

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    binary_abs = os.path.join(script_dir, "..", "vehicle_ecu")

    if not os.path.exists(binary_abs):
        print("Binary not found. Run 'make' from the project root first.")
        sys.exit(1)

    print(f"--- Running Stress/Regression Test ({ITERATIONS} cycles) ---")
    
    # Inputs that trigger all 4 alerts: impact=90, dist=10, fuel=8, tamper=1
    cmd = [binary_abs, "90", "10", "8", "1"]
    
    for i in range(1, ITERATIONS + 1):
        try:
            res = subprocess.run(cmd, capture_output=True, text=True, timeout=1)
        except subprocess.TimeoutExpired:
            print(f"\nFAIL: Process timed out on iteration {i}.")
            sys.exit(1)

        if res.returncode != 0:
            print(f"\nFAIL: Process crashed with return code {res.returncode} on iteration {i}.")
            sys.exit(1)

        output = res.stdout.upper()

        # Check that ALL expected alerts were triggered
        for alert in EXPECTED_ALERTS:
            if alert not in output:
                print(f"\nFAIL: Missing alert '{alert}' on iteration {i}.")
                print("--- Output ---\n", res.stdout)
                sys.exit(1)

        # Check that system status reports exactly 4 alerts
        if "4 ALERT(S) ACTIVE" not in output:
            print(f"\nFAIL: System status incorrect on iteration {i}.")
            print("--- Output ---\n", res.stdout)
            sys.exit(1)

        # Progress bar (print a dot every 50 iterations)
        if i % 50 == 0:
            sys.stdout.write('.')
            sys.stdout.flush()

    print("\n\nStress Test: PASSED. Zero crashes or dropped frames across 1000 simultaneous trigger events.")
    sys.exit(0)

if __name__ == "__main__":
    main()
