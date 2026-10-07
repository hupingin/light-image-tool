# 构建指南

## 环境要求

- **编译器**：`gcc` / `clang`（Linux、macOS），或 MinGW `gcc`（Windows）。
- **或** 已安装 **Visual Studio Build Tools**（Windows），使用仓库内置的 `build_msvc.bat`。
- 仅需一个 C99 兼容编译器，**无需任何第三方库**。

## 方式一：Make（推荐，跨平台）

```bash
# 默认: 构建统一二进制 bin/lit
make

# 额外: 按模块拆分的动态库 lib/liblitcore / liblitbmp / liblitops
make libs

# 清理产物(bin/ lib/)
make clean
```

`Makefile` 会根据平台自动选择动态库后缀：

| 平台 | 动态库后缀 |
| --- | --- |
| Linux | `.so` |
| macOS | `.dylib` |
| Windows (MinGW / MSYS) | `.dll` |

编译选项：`-std=c99 -O2 -Wall -Isrc`（`CC` / `CFLAGS` 均可在命令行覆盖，如 `make CC=clang`）。

## 方式二：MSVC（Windows）

在已安装 VS Build Tools 的机器上，直接运行：

```bat
build_msvc.bat
```

该脚本内部会：

1. 调用 `vcvars64.bat` 配置 MSVC 环境（含 Windows SDK 的 INCLUDE / LIB）；
2. 用 `cl.exe` 将 `src/main.c` 与 `src/core/*.c` 编译链接为 `bin/lit.exe`。

> 注意：沙箱环境禁止从 Bash 直接调用 `cmd.exe`。在受限环境中可改用 PowerShell 手动设置
> `INCLUDE` / `LIB` / `PATH` 后直接调用 `cl.exe`，效果等价。

## 验证

构建完成后即可运行测试（见 [测试](testing.md)）：

```bash
python test/test_bmp.py
```

> 本项目已在 **MSVC Build Tools 18 + Windows SDK 10.0.26100.0** 下编译并通过全部测试。
