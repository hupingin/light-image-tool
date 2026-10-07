# 架构设计

light-image-tool 的定位是一个**纯 C 图像中间件**：所有功能围绕一套统一的内存图像抽象
`LITImage` 展开，编解码与算子都只与该抽象打交道，便于组合与二次开发。

## 核心抽象：`LITImage`

定义于 `src/light_image.h`：

```c
typedef enum {
    LIT_FMT_GRAY = 1,  /* 1 通道: 灰度 */
    LIT_FMT_RGB  = 3,  /* 3 通道: R,G,B */
    LIT_FMT_RGBA = 4   /* 4 通道: R,G,B,A */
} LITPixelFormat;

typedef struct {
    int            width;    /* 像素宽 */
    int            height;   /* 像素高 */
    LITPixelFormat format;   /* 像素格式(同时等价于每像素通道数) */
    int            stride;   /* 每行字节数(含行尾填充, 已 4 字节对齐) */
    uint8_t       *data;     /* 像素缓冲区, 长度 = stride * height */
} LITImage;
```

设计要点：

- **行优先(row-major)**：像素按 `(y, x)` 顺序紧凑存放，第 `y` 行起始地址为 `data + y*stride`。
- **4 字节行对齐**：`stride = (width * channels + 3) & ~3`，既利于后续 SIMD，也方便直接导出(如 BMP)。
- **内存统一 RGB(A) 顺序**：无论磁盘格式如何(BMP 为 BGR)，载入内存后一律为 R,G,B(,A)。
- **通道数即枚举值**：`LIT_FMT_RGB == 3`，因此 `img->format` 可直接当作通道数参与寻址。

## 模块划分

```
src/
├── light_image.h          # 核心类型与公共 API(被所有模块 include)
├── main.c                 # 统一命令行入口(对应"二进制进程"发布形态)
├── core/
│   ├── image.c            # LITImage 的创建/销毁/像素访问/错误码
│   ├── bmp.c / bmp.h      # BMP 编解码(8 / 24 / 32 位, 纯 C)
│   └── ops.c / ops.h      # 基础算子: 灰度 / 翻转 / 旋转
└── jpeg/
    └── include/
        └── jpeg.h         # JPEG 接口声明(待实现)
```

约定：

- **编解码模块**（bmp/jpeg）负责"磁盘格式 ⇄ `LITImage` "。
- **算子模块**（ops）只读取 `src`、生成**新**图像，不修改入参。
- 上层调用方持有 `LITImage*`，用完后调用 `lit_image_destroy` 释放。

## 两种发布形态

对应 README 中描述的两条发布路线：

1. **统一二进制** `bin/lit`：把所有模块编进一个可执行文件，适合直接当进程调用。
2. **按模块拆分动态库** `lib/liblitcore` / `liblitbmp` / `liblitops`
   （`.so` / `.dll` / `.dylib`）：上层项目按需链接，例如只做 BMP 处理时仅依赖
   `liblitcore` + `liblitbmp`，适合大型项目集成。

## 设计原则

- **尽量少依赖**：不引入 libjpeg / libpng 等第三方库；JPEG、PNG 计划以纯 C 在模块内部实现。
- **拷贝有用片段**：若借鉴其它项目思想，只摘录可复用的部分，不整体搬入。
- **错误码统一**：所有接口返回 `LITStatus`，调用方可据此分支处理。
