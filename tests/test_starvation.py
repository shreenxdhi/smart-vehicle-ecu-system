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

    print("--- Running Priority Starvation Test ---")
    
    # Run test without round-robin
    cmd_starved = [binary_abs, "90", "50", "8", "0", "--starve-test"]
    res = subprocess.run(cmd_starved, capture_output=True, text=True)
    
    output = res.stdout.upper()
    
    # We expect Airbag to deploy, but Fuel to NEVER deploy because it's starved out
    if "LOW FUEL WARNING" in output:
        print("FAIL: The strict-priority test failed to demonstrate starvation! Fuel was processed.")
        sys.exit(1)
        
    print("PASS: Fuel ECU was successfully starved by high-priority Airbag spam (Strict Priority).")

    print("\n--- Running Round-Robin Fallback Test ---")
    
    # Run test WITH round-robin
    cmd_fixed = [binary_abs, "90", "50", "8", "0", "--starve-test", "--round-robin"]
    res = subprocess.run(cmd_fixed, capture_output=True, text=True)
    
    output = res.stdout.upper()
    
    # We expect BOTH Airbag and Fuel to deploy because the scheduler guarantees fairness
    if "LOW FUEL WARNING" not in output:
        print("FAIL: Round-Robin Fallback failed to prevent starvation! Fuel is still missing.")
        print("Output:\n", output)
        sys.exit(1)
        
    print("PASS: Fuel ECU was successfully processed! Round-Robin scheduler prevented starvation.")
    
    print("\nStarvation Testing: PASSED.")
    sys.exit(0)

if __name__ == "__main__":
    main()
