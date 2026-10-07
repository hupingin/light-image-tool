/*
 *  A declare for Jpeg
 *  Written by hopeking <hupingjin@163.com>
 *
 *  Declare interface of Jpeg.
 *
 *  Copyright 2023 hopeking.
 *
 *  See ../../LICENSE  for licensing terms.
 */

#ifndef LIT_JPEG_H
#define LIT_JPEG_H

#include "../../light_image.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  JPEG 编解码接口声明(尚未实现)。
 *
 *  设计目标: 纯 C、零外部依赖, 在模块内部实现一套精简的 JPEG 读写,
 *  不引入 libjpeg 等第三方库(遵循本项目"尽量少依赖"的原则)。
 *
 *  当前阶段仅给出接口契约, 实际编解码待后续补全。
 *  CLI 与上层调用方可直接 include 本头文件并按契约对接。
 */

/* 从文件加载 JPEG, 输出 LITImage(RGB / RGBA)。
   成功返回 LIT_OK, *out 指向新建图像(调用方负责 lit_image_destroy)。 */
LITStatus lit_jpeg_load(const char *path, LITImage **out);

/* 将 LITImage 保存为 JPEG。quality 取值 1~100(越大质量越高、体积越大)。 */
LITStatus lit_jpeg_save(const LITImage *img, const char *path, int quality);

#ifdef __cplusplus
}
#endif

#endif /* LIT_JPEG_H */
