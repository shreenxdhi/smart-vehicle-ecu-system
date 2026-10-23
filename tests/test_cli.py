from pathlib import Path
import subprocess

binary = str(Path(__file__).resolve().parents[1] / "vehicle_ecu")
base = ["50", "50", "50", "0"]
cases = [[], ["a", "50", "50", "0"], ["101", "50", "50", "0"],
         ["-1", "50", "50", "0"], ["50", "256", "50", "0"],
         ["50", "50", "101", "0"], ["50", "50", "50", "2"],
         ["5x", "50", "50", "0"], ["99999999999999999999999", "50", "50", "0"]]
for flag in (["--unknown"], ["--error-rate"], ["--seed"], ["--seed", "-1"],
             ["--error-rate", "nan"], ["--error-rate", "inf"], ["--error-rate", "-0.1"],
             ["--error-rate", "1.1"], ["--error-rate", "abc"], ["--error-rate", ""]):
    cases.append(base + flag)
for args in cases:
    result = subprocess.run([binary, *args], capture_output=True, text=True, timeout=3)
    assert result.returncode != 0, args
    assert "Starting ECU simulation" not in result.stdout, args
for seed in ("0", "1", "4294967295"):
    args = [binary, *base, "--error-rate", "0.5", "--seed", seed]
    first = subprocess.run(args, capture_output=True, text=True, check=True).stdout
    second = subprocess.run(args, capture_output=True, text=True, check=True).stdout
    assert first == second
result = subprocess.run([binary, *base, "--error-rate", "1"], capture_output=True, text=True, check=True)
assert result.stdout.count("Comm Lost") == 4
print(f"PASS CLI: {len(cases)} invalid inputs, three reproducible seeds, total communication loss")
