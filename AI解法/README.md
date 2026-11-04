# AI解法

| Binary | Size | Full `md5sum` match | ≤64MB AS |
|---|---|---|---|
| **selfmd5** | **292** | **yes** `652a79162c69154de82f66c53b87f588` | **yes** |

**由纯 AI 独立完成**（设计、汇编、碰撞搜索与验证）。  
路线：汇编织入 ELF + 只算最后一块 MD5 + IV[3]==0 / 低半字节非零碰撞。

小白向图文说明 → [../docs/AI解法.md](../docs/AI解法.md)

## Build / Verify
```
python3 build.py             # 应打印 MATCH（RLIMIT_AS=64MB）
wc -c selfmd5                # 292
./selfmd5; echo
md5sum selfmd5
```

可选重搜碰撞（改布局后才需要）：
```
gcc -O3 -fopenmp -o search search.c -lm
./search selfmd5
```
