/*
 *  核心图像抽象实现
 *  纯 C, 零外部依赖。
 *
 *  Copyright 2023 hopeking.
 *  See ../../LICENSE for licensing terms.
 */

#include "../light_image.h"
#include <stdlib.h>
#include <string.h>

LITImage* lit_image_create(int width, int height, LITPixelFormat fmt) {
    if (width <= 0 || height <= 0) return NULL;
    int channels = (int)fmt;
    if (channels <= 0) return NULL;

    LITImage *img = (LITImage*)malloc(sizeof(LITImage));
    if (!img) return NULL;

    int row_bytes = width * channels;
    /* 行对齐到 4 字节, 既利于 SIMD, 也方便后续导出(如 BMP) */
    int stride = (row_bytes + 3) & ~3;
    size_t size = (size_t)stride * (size_t)height;

    uint8_t *data = (uint8_t*)malloc(size);
    if (!data) {
        free(img);
        return NULL;
    }
    memset(data, 0, size);

    img->width  = width;
    img->height = height;
    img->format = fmt;
    img->stride = stride;
    img->data   = data;
    return img;
}

void lit_image_destroy(LITImage *img) {
    if (!img) return;
    free(img->data);
    free(img);
}

int lit_image_channels(const LITImage *img) {
    return img ? (int)img->format : 0;
}

int lit_image_row_bytes(const LITImage *img) {
    if (!img) return 0;
    return img->width * (int)img->format;
}

uint8_t* lit_image_pixel(LITImage *img, int x, int y) {
    if (!img) return NULL;
    if (x < 0 || y < 0 || x >= img->width || y >= img->height) return NULL;
    int channels = (int)img->format;
    return img->data + (size_t)y * img->stride + (size_t)x * channels;
}

const char* lit_status_string(LITStatus s) {
    switch (s) {
        case LIT_OK:             return "success";
        case LIT_ERR_NULL:       return "null pointer argument";
        case LIT_ERR_MEMORY:     return "out of memory";
        case LIT_ERR_FORMAT:     return "unsupported or corrupt format";
        case LIT_ERR_IO:         return "file I/O error";
        case LIT_ERR_UNSUPPORTED:return "operation not supported yet";
        default:                 return "unknown error";
    }
}
