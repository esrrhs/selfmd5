#!/usr/bin/env python3
"""Assemble and verify AI解法/selfmd5 (292 B)."""
import subprocess, pathlib, hashlib, resource
HERE = pathlib.Path(__file__).resolve().parent
subprocess.check_call(["nasm", "-f", "bin", "-o", str(HERE / "selfmd5"), str(HERE / "selfmd5.asm")])
(HERE / "selfmd5").chmod(0o755)
data = (HERE / "selfmd5").read_bytes()
r = subprocess.run(
    [str(HERE / "selfmd5")],
    capture_output=True,
    preexec_fn=lambda: resource.setrlimit(resource.RLIMIT_AS, (64 << 20, 64 << 20)),
)
got = r.stdout.decode()
exp = hashlib.md5(data).hexdigest()
print(f"size={len(data)}")
print(f"output={got}")
print(f"md5sum={exp}")
print("MATCH" if got == exp else "MISMATCH")
raise SystemExit(0 if got == exp else 1)
