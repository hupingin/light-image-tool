#ifndef LIT_OPS_H
#define LIT_OPS_H

#include "../light_image.h"

/*
 *  基础图像处理算子
 *  所有算子均生成"新图像", 不修改输入(src 保持只读)。
 *
 *  Copyright 2023 hopeking.
 *  See ../../LICENSE for licensing terms.
 */

/* 灰度化(亮度法: 0.299R + 0.587G + 0.114B), 输出 GRAY 图像。 */
LITStatus lit_op_grayscale(const LITImage *src, LITImage **out);

/* 水平翻转(左右镜像)。 */
LITStatus lit_op_flip_h(const LITImage *src, LITImage **out);

/* 垂直翻转(上下镜像)。 */
LITStatus lit_op_flip_v(const LITImage *src, LITImage **out);

/* 顺时针旋转 90 度。新尺寸: 宽=原高, 高=原宽。 */
LITStatus lit_op_rotate90(const LITImage *src, LITImage **out);

#endif /* LIT_OPS_H */
