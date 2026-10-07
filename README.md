# light-image-tool

A tool of image process, smart and light.

## 介绍

这是一个用纯 C 语言编写的、处理图片的工具。

## 定位

本项目定位为一个中间件，专门用来处理图片的各方面。

## 特点

- 本项目尽量少的依赖其它项目，如果用到了其它项目的思想，请阅读其它项目相关代码，
  然后拷贝其中有用的部分，其余部分请不要拷贝。
- 内存统一采用 `RGB(A)` 顺序、行优先(row-major)紧凑存储，所有模块围绕同一套
  `LITImage` 抽象工作，便于组合与二次开发。

## 发布

本项目有两个发布：

1. 把所有的模块都拆分成 `.so` / `.dll` 的动态库，按需取用，适合大型项目集成。
2. 同时有一个汇总的程序，提供统一的功能，适合使用二进制进程。

## 目录结构

```
light-image-tool/
├── Makefile              # 构建脚本(统一二进制 / 动态库)
├── src/
│   ├── light_image.h     # 核心抽象: LITImage 结构体与公共 API
│   ├── main.c            # 统一命令行入口
│   ├── core/
│   │   ├── image.c       # 图像创建/销毁/像素访问
│   │   ├── bmp.h / bmp.c # BMP 编解码(8/24/32 位, 纯 C 实现)
│   │   └── ops.h / ops.c # 基础算子: 灰度/翻转/旋转
│   └── jpeg/
│       └── include/
│           └── jpeg.h    # JPEG 接口声明(待实现)
└── LICENSE               # MIT
```

## 构建

需要 `gcc` / `clang`（Linux、macOS）或 MinGW `gcc`（Windows）。

```bash
# 默认: 构建统一二进制 bin/lit
make

# 额外: 构建按模块拆分的动态库 lib/liblitcore / liblitbmp / liblitops
make libs

# 清理产物
make clean
```

### Windows + MSVC

已安装 Visual Studio Build Tools 时，可直接运行 `build_msvc.bat`（内部调用 `vcvars64.bat`
配置环境后用 `cl.exe` 编译出 `bin/lit.exe`），无需 Make/git-bash。

### 测试

`test/test_bmp.py` 用纯标准库生成已知 BMP，调用 `lit` 各子命令后独立解析输出校验：

```bash
python test/test_bmp.py
```

## 使用

```bash
# 查看图片信息
./bin/lit info  demo.bmp

# 转灰度
./bin/lit to-gray  in.bmp  out.bmp

# 水平 / 垂直翻转
./bin/lit flip-h  in.bmp  out.bmp
./bin/lit flip-v  in.bmp  out.bmp

# 顺时针旋转 90 度
./bin/lit rot90   in.bmp  out.bmp
```

> 当前编解码以 BMP 为主（零依赖、易验证）。JPEG 接口已在 `src/jpeg/include/jpeg.h`
> 中声明，实际编解码将作为后续模块补全。

## 作为动态库集成

`make libs` 会产出 `lib/liblitcore`、`lib/liblitbmp`、`lib/liblitops`。
上层项目可只链接需要的模块，例如仅做 BMP 处理时链接 `liblitcore` + `liblitbmp`。

## 路线图

- [x] 核心图像抽象 `LITImage`
- [x] BMP 读写（8 / 24 / 32 位）
- [x] 基础算子：灰度、水平/垂直翻转、顺时针旋转 90°
- [ ] JPEG 编解码实现
- [ ] PNG 编解码（纯 C、零依赖）
- [ ] 更多算子：缩放、裁剪、亮度/对比度调整
- [ ] 统一格式注册表，按扩展名自动选择编解码器
