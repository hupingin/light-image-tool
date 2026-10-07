#ifndef LIT_BMP_H
#define LIT_BMP_H

#include "../light_image.h"

/*
 *  BMP 编解码接口(纯 C 实现, 零外部依赖)。
 *
 *  Copyright 2023 hopeking.
 *  See ../../LICENSE for licensing terms.
 */

/* 从文件加载 BMP。支持 8 位灰度 / 24 位 / 32 位, 仅 BI_RGB(无压缩)。
   成功返回 LIT_OK, *out 指向新建图像(调用方负责 lit_image_destroy)。 */
LITStatus lit_bmp_load(const char *path, LITImage **out);

/* 将图像保存为 BMP。
   GRAY -> 8 位灰度(带灰度调色板);
   RGB  -> 24 位;
   RGBA -> 32 位(含 Alpha)。 */
LITStatus lit_bmp_save(const LITImage *img, const char *path);

#endif /* LIT_BMP_H */
