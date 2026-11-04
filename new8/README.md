# selfmd5-v8

| Binary | Size | Full `md5sum` match | ≤64MB AS |
|---|---|---|---|
| **selfmd5_v8** | **292** | **yes** `652a79162c69154de82f66c53b87f588` | **yes** |
| ../new6/selfmd5_v6 | 295 | yes | no (~82MB memsz) |

Same architecture as new6 (ELF weave + last-block MD5 + IV[3]==0 + low-nibble hex). Micro-opts:

1. **Loop head in `p_paddr`** — drop `xor edi,edi`; Linux x86-64 ELF entry has `rdi=0` (= D).
2. **Free trampoline in `p_memsz`** — `EB xx` rebound so `jnz` stays short while loop head sits at `0x54`.
3. **Page-tail `memsz=filesz` style** — trampoline encodes memsz≈62KiB (≤64MB); padding uses the mapped page.
4. **No `pop` before `exit`** — exit status 1.

## Build / Verify
```
python3 build_v8.py          # MATCH under RLIMIT_AS=64MB
wc -c selfmd5_v8             # 292
./selfmd5_v8; echo
md5sum selfmd5_v8
```
