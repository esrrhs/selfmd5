# selfmd5-v6

| Binary | Size | Full `md5sum` match |
|---|---|---|
| **selfmd5_v6** | **295** | **yes** `995913dd8743de76e1be940166cea2bd` |
| ../new5/selfmd5_v5 | 297 | yes |
| Target (old record) | 299 | — |

相对 v5 再小 2 字节：hex 输出改为 `shr eax,8 / jnz .nib`（省掉 `test edi,edx`），并额外碰撞约束「摘要每个字节的低半字节 ≠ 0」。

> 小白向图文（原理同 297）：[../docs/297-字节方案.md](../docs/297-字节方案.md)

## Build / Verify
```
python3 build_v6.py          # 应打印 MATCH
wc -c selfmd5_v6             # 295
./selfmd5_v6; echo
md5sum selfmd5_v6
```
