# MuCompilerGenerator

**MuCompilerGenerator(MuCplGen)** is a Header-Only dynamic compiler generator based on C++ 17.

Why MuCplGen:

- header-only
- self-contained (without any dependencies)
- type-safe semantic action (debug-friendly)
- cross-platform

## Scanner

Regular Expression (Regex) based.



## Syntax-Directed Parser

Context Free Grammar (CFG) based.

| Parser     | Usage                    |                    |
| ---------- | ------------------------ | ------------------ |
| SLR        | `SLRParser<UserToken,T>` | :heavy_check_mark: |
| LR1        | `LR1Parser<UserToken,T>` | :heavy_check_mark: |
| BaseParser |                          | :x:                |

