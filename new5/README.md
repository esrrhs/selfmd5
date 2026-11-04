# selfmd5-v5

| Binary | Size | Full `md5sum` match |
|---|---|---|
| **selfmd5_v5** | **297** | **yes** `cc10068c9e89450201c84f848400fc66` |
| Target (beat) | 299 | — |

**297 < 299**（相对原项目 474 也小了 177）。

## Method（汇编 + 碰撞）
1. **汇编** — 只算 MD5 最后一块；ELF 头/phdr 织入代码（`e_phoff=0x38`）；`fsin` 生成 K[i]；寄存器 MD5 + 因式分解 F/g；复用 argc/argv 做 write/exit；hex 用 `div` + `test edi,edx`（argv 偶地址时稳定打出高低半字节）；`IV[3]==0` 故只存 12 字节 IV（`xor edi,edi`）。
2. **碰撞** — 暴力 `start3` 3 字节 + 页对齐 `org`，直到中间状态 `IV[3]==0`，写回 IV[0..2]，整文件自洽。

## Build / Verify
```
python3 build_v5.py          # 应打印 MATCH
# 或: nasm -f bin -o selfmd5_v5 selfmd5_v5.asm && chmod +x selfmd5_v5
wc -c selfmd5_v5             # 297
./selfmd5_v5; echo
md5sum selfmd5_v5
```

碰撞搜索（可选，已冻结进 asm）：
```
gcc -O3 -fopenmp -o search_v5 search_v5.c -lm
OMP_NUM_THREADS=8 ./search_v5 selfmd5_v5
```
