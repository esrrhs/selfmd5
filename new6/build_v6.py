#!/usr/bin/env python3
"""Assemble and verify selfmd5-v6."""
import subprocess, pathlib, hashlib
HERE = pathlib.Path(__file__).resolve().parent
subprocess.check_call(["nasm","-f","bin","-o",str(HERE/"selfmd5_v6"), str(HERE/"selfmd5_v6.asm")])
(HERE/"selfmd5_v6").chmod(0o755)
data=(HERE/"selfmd5_v6").read_bytes()
got=subprocess.check_output([str(HERE/"selfmd5_v6")]).decode()
exp=hashlib.md5(data).hexdigest()
print(f"size={len(data)}")
print(f"output={got}")
print(f"md5sum={exp}")
print("MATCH" if got==exp else "MISMATCH")
raise SystemExit(0 if got==exp else 1)
