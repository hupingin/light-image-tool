/*
 *  基础图像处理算子实现
 *  纯 C, 零外部依赖。
 *
 *  Copyright 2023 hopeking.
 *  See ../../LICENSE for licensing terms.
 */

#include "ops.h"
#include <stdlib.h>
#include <string.h>

LITStatus lit_op_grayscale(const LITImage *src, LITImage **out) {
    if (!src || !out) return LIT_ERR_NULL;
    *out = NULL;

    if (src->format == LIT_FMT_GRAY) {
        /* 已是灰度: 直接克隆 */
        LITImage *g = lit_image_create(src->width, src->height, LIT_FMT_GRAY);
        if (!g) return LIT_ERR_MEMORY;
        memcpy(g->data, src->data, (size_t)g->stride * g->height);
        *out = g;
        return LIT_OK;
    }

    int ch = (int)src->format;
    LITImage *g = lit_image_create(src->width, src->height, LIT_FMT_GRAY);
    if (!g) return LIT_ERR_MEMORY;

    for (int y = 0; y < src->height; y++) {
        const uint8_t *s = src->data + (size_t)y * src->stride;
        uint8_t *d = g->data + (size_t)y * g->stride;
        for (int x = 0; x < src->width; x++) {
            uint8_t r = s[x * ch + 0];
            uint8_t gn = s[x * ch + 1];
            uint8_t b = s[x * ch + 2];
            int lum = (int)(0.299 * r + 0.587 * gn + 0.114 * b);
            d[x] = (uint8_t)(lum > 255 ? 255 : lum);
        }
    }
    *out = g;
    return LIT_OK;
}

LITStatus lit_op_flip_h(const LITImage *src, LITImage **out) {
    if (!src || !out) return LIT_ERR_NULL;
    *out = NULL;

    int ch = (int)src->format;
    LITImage *d = lit_image_create(src->width, src->height, src->format);
    if (!d) return LIT_ERR_MEMORY;

    for (int y = 0; y < src->height; y++) {
        const uint8_t *s = src->data + (size_t)y * src->stride;
        uint8_t *dd = d->data + (size_t)y * d->stride;
        for (int x = 0; x < src->width; x++) {
            int sx = src->width - 1 - x;
            memcpy(dd + x * ch, s + sx * ch, (size_t)ch);
        }
    }
    *out = d;
    return LIT_OK;
}

LITStatus lit_op_flip_v(const LITImage *src, LITImage **out) {
    if (!src || !out) return LIT_ERR_NULL;
    *out = NULL;

    LITImage *d = lit_image_create(src->width, src->height, src->format);
    if (!d) return LIT_ERR_MEMORY;

    for (int y = 0; y < src->height; y++) {
        const uint8_t *s = src->data + (size_t)y * src->stride;
        uint8_t *dd = d->data + (size_t)(src->height - 1 - y) * d->stride;
        memcpy(dd, s, (size_t)src->stride);
    }
    *out = d;
    return LIT_OK;
}

LITStatus lit_op_rotate90(const LITImage *src, LITImage **out) {
    if (!src || !out) return LIT_ERR_NULL;
    *out = NULL;

    int ch = (int)src->format;
    int W = src->width, H = src->height;
    /* 顺时针 90 度: 新宽=原高, 新高=原宽 */
    LITImage *d = lit_image_create(H, W, src->format);
    if (!d) return LIT_ERR_MEMORY;

    for (int dy = 0; dy < W; dy++) {
        for (int dx = 0; dx < H; dx++) {
            int sx = dy;
            int sy = H - 1 - dx;
            const uint8_t *s = src->data + (size_t)sy * src->stride + (size_t)sx * ch;
            uint8_t *dd = d->data + (size_t)dy * d->stride + (size_t)dx * ch;
            memcpy(dd, s, (size_t)ch);
        }
    }
    *out = d;
    return LIT_OK;
}
