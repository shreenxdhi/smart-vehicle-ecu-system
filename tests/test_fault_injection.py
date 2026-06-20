#!/usr/bin/env python3

import subprocess
import os
import sys

BINARY = "../vehicle_ecu"

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    binary_abs = os.path.join(script_dir, "..", "vehicle_ecu")

    if not os.path.exists(binary_abs):
        print("Binary not found. Run 'make' from the project root first.")
        sys.exit(1)

    print("--- Running Fault Injection Test ---")
    
    # --fault-inject enables fault injection
    cmd = [binary_abs, "50", "50", "50", "0", "--fault-inject"]
    
    try:
        res = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
    except subprocess.TimeoutExpired:
        print("FAIL: Process timed out (possible infinite loop due to malformed frame).")
        sys.exit(1)
        
    output = res.stdout.upper()
    returncode = res.returncode

    print("--- Output from ECU Simulation ---")
    print(res.stdout.strip())
    print("----------------------------------\n")

    if returncode != 0:
        print(f"FAIL: Process crashed with return code {returncode}.")
        sys.exit(1)

    # Check that the malformed frame was detected but ignored/handled safely
    if "UNKNOWN CAN ID" in output and "7FF" in output:
        print("PASS: Master node safely handled unknown CAN ID without crashing.")
    else:
        print("FAIL: Master node output did not report unknown CAN ID.")
        sys.exit(1)

    # Ensure system still reports normal status for the other 4 ECUs
    if "ALL SYSTEMS NORMAL" in output:
        print("PASS: Valid ECU data was still processed correctly.")
    else:
        print("FAIL: Valid ECU data was not processed correctly due to fault.")
        sys.exit(1)
        
    print("\nFault Injection Test: PASSED.")
    sys.exit(0)

if __name__ == "__main__":
    main()
