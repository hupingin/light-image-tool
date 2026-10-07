# API 参考

所有接口定义在 `src/light_image.h` 及各个模块头文件中，统一返回 `LITStatus` 错误码。

## 公共类型

```c
typedef enum { LIT_FMT_GRAY=1, LIT_FMT_RGB=3, LIT_FMT_RGBA=4 } LITPixelFormat;

typedef struct {
    int width; int height; LITPixelFormat format; int stride; uint8_t *data;
} LITImage;

typedef enum {
    LIT_OK=0, LIT_ERR_NULL=1, LIT_ERR_MEMORY=2,
    LIT_ERR_FORMAT=3, LIT_ERR_IO=4, LIT_ERR_UNSUPPORTED=5
} LITStatus;
```

## 核心（`src/light_image.h`）

| 函数 | 说明 |
| --- | --- |
| `LITImage* lit_image_create(int w, int h, LITPixelFormat fmt)` | 创建空图像(像素清零)；尺寸非法或内存不足返回 `NULL` |
| `void lit_image_destroy(LITImage *img)` | 释放图像及其缓冲区（`NULL` 安全） |
| `int lit_image_channels(const LITImage *img)` | 通道数（等于 `format` 枚举值） |
| `int lit_image_row_bytes(const LITImage *img)` | 一行有效像素字节数（不含行尾填充） |
| `uint8_t* lit_image_pixel(LITImage *img, int x, int y)` | 指向 `(x,y)` 像素首字节；越界返回 `NULL` |
| `const char* lit_status_string(LITStatus s)` | 错误码 → 可读字符串 |

## BMP 编解码（`src/core/bmp.h`）

| 函数 | 说明 |
| --- | --- |
| `LITStatus lit_bmp_load(const char *path, LITImage **out)` | 从文件加载 BMP，支持 8 位灰度 / 24 位 / 32 位（`BI_RGB`）。成功时 `*out` 指向新建图像 |
| `LITStatus lit_bmp_save(const LITImage *img, const char *path)` | 保存为 BMP：`GRAY`→8 位（带灰阶调色板），`RGB`→24 位，`RGBA`→32 位（含 Alpha） |

## 基础算子（`src/core/ops.h`）

所有算子均**读取 `src`、生成新图像**，调用方负责 `lit_image_destroy` 返回的 `*out`。

| 函数 | 说明 |
| --- | --- |
| `LITStatus lit_op_grayscale(const LITImage *src, LITImage **out)` | 灰度化（亮度法 `0.299R+0.587G+0.114B`），输出 `GRAY` |
| `LITStatus lit_op_flip_h(const LITImage *src, LITImage **out)` | 水平翻转（左右镜像） |
| `LITStatus lit_op_flip_v(const LITImage *src, LITImage **out)` | 垂直翻转（上下镜像） |
| `LITStatus lit_op_rotate90(const LITImage *src, LITImage **out)` | 顺时针旋转 90°；新尺寸：宽=原高，高=原宽 |

## JPEG 接口（`src/jpeg/include/jpeg.h`）

> 当前**仅声明、未实现**，实际编解码待后续补全（计划纯 C、零依赖）。

| 函数 | 说明 |
| --- | --- |
| `LITStatus lit_jpeg_load(const char *path, LITImage **out)` | 从文件加载 JPEG，输出 `RGB` / `RGBA` |
| `LITStatus lit_jpeg_save(const LITImage *img, const char *path, int quality)` | 保存为 JPEG，`quality` 取值 1~100 |

## 使用示例

```c
#include "light_image.h"
#include "core/bmp.h"
#include "core/ops.h"

LITImage *img = NULL, *out = NULL;
if (lit_bmp_load("in.bmp", &img) == LIT_OK) {
    if (lit_op_grayscale(img, &out) == LIT_OK) {
        lit_bmp_save(out, "gray.bmp");
        lit_image_destroy(out);
    }
    lit_image_destroy(img);
}
```
