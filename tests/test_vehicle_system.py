#!/usr/bin/env python3

import subprocess
import csv
import os
import sys

# binary is built at project root, one level up from tests/
BINARY = "../vehicle_ecu"
CSV    = os.path.join(os.path.dirname(__file__), "test_vectors.csv")

ALERTS = ["DEPLOY AIRBAG", "EMERGENCY BRAKE", "LOW FUEL WARNING", "ALARM ON"]

def run_test(row, test_num):
    cmd = [BINARY, row["impact"], row["distance"], row["fuel"], row["tamper"]]
    res = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
    output = res.stdout.upper()

    expected = row["expected"].strip().upper()

    if expected == "ALL SYSTEMS NORMAL":
        passed = not any(a in output for a in ALERTS)
    else:
        passed = expected in output

    if passed:
        print(f"  Test #{test_num:02d}  PASS  ({expected})")
    else:
        print(f"  Test #{test_num:02d}  FAIL")
        print(f"    Expected : {expected}")
        for line in res.stdout.splitlines():
            if "SYSTEM STATUS" in line.upper() or "ACTION" in line.upper():
                print(f"    Got      : {line.strip()}")
                break

    return passed


def main():
    # resolve binary path relative to project root
    script_dir = os.path.dirname(os.path.abspath(__file__))
    binary_abs = os.path.join(script_dir, "..", "vehicle_ecu")

    if not os.path.exists(binary_abs):
        print("Binary not found. Run 'make' from the project root first.")
        sys.exit(1)

    # use absolute path so script works from any working directory
    global BINARY
    BINARY = binary_abs

    passed = 0
    failed = 0

    print()
    with open(CSV, "r") as f:
        for i, row in enumerate(csv.DictReader(f), 1):
            if run_test(row, i):
                passed += 1
            else:
                failed += 1

    print()
    print("----------------------------")
    print(f"  Tests Run : {passed + failed}")
    print(f"  Passed    : {passed}")
    print(f"  Failed    : {failed}")
    print("----------------------------")
    print()

    sys.exit(0 if failed == 0 else 1)


if __name__ == "__main__":
    main()
