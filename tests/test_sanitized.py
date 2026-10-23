from pathlib import Path
import os
import subprocess
import tempfile

binary = str(Path(__file__).resolve().parents[1] / "build/vehicle-sanitized")
scenarios = [("90", "10", "8", "1"), ("50", "50", "50", "0", "--fault-inject"),
             ("90", "50", "8", "0", "--starve-test"),
             ("90", "50", "8", "0", "--starve-test", "--round-robin"),
             ("90", "10", "8", "1", "--error-rate", "1"),
             ("50", "50", "50", "0", "--error-rate", "0.5", "--seed", "123")]
with tempfile.TemporaryDirectory() as folder:
    for args in scenarios:
        result = subprocess.run([binary, *args], cwd=folder, text=True, capture_output=True,
                                timeout=5, env={**os.environ, "UBSAN_OPTIONS": "halt_on_error=1"})
        assert result.returncode == 0 and not result.stderr, (args, result.stderr)
print(f"PASS {len(scenarios)} full-application scenarios under ASan/UBSan")
