/*
 *  BMP 编解码实现
 *  纯 C, 零外部依赖。仅处理 BI_RGB(无压缩) 的 8/24/32 位 BMP。
 *
 *  Copyright 2023 hopeking.
 *  See ../../LICENSE for licensing terms.
 */

#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- 小端读写辅助 ---------- */
static uint16_t read_u16(FILE *f) {
    int b0 = fgetc(f), b1 = fgetc(f);
    if (b0 == EOF || b1 == EOF) return 0;
    return (uint16_t)((uint16_t)b0 | ((uint16_t)b1 << 8));
}
static uint32_t read_u32(FILE *f) {
    int b0 = fgetc(f), b1 = fgetc(f), b2 = fgetc(f), b3 = fgetc(f);
    if (b0 == EOF || b1 == EOF || b2 == EOF || b3 == EOF) return 0;
    return (uint32_t)b0 | ((uint32_t)b1 << 8) | ((uint32_t)b2 << 16) | ((uint32_t)b3 << 24);
}
static void write_u16(FILE *f, uint16_t v) {
    fputc(v & 0xFF, f);
    fputc((v >> 8) & 0xFF, f);
}
static void write_u32(FILE *f, uint32_t v) {
    fputc(v & 0xFF, f);
    fputc((v >> 8) & 0xFF, f);
    fputc((v >> 16) & 0xFF, f);
    fputc((v >> 24) & 0xFF, f);
}

/* ---------- 加载 ---------- */
LITStatus lit_bmp_load(const char *path, LITImage **out) {
    if (!path || !out) return LIT_ERR_NULL;
    *out = NULL;

    FILE *f = fopen(path, "rb");
    if (!f) return LIT_ERR_IO;

    /* 文件头 */
    int m0 = fgetc(f), m1 = fgetc(f);
    if (m0 != 'B' || m1 != 'M') { fclose(f); return LIT_ERR_FORMAT; }

    read_u32(f);             /* 文件大小(此处不依赖, 跳过) */
    read_u32(f);             /* 保留字段 */
    uint32_t data_offset = read_u32(f);

    /* 信息头 BITMAPINFOHEADER */
    uint32_t info_size = read_u32(f);
    if (info_size < 40) { fclose(f); return LIT_ERR_FORMAT; }

    uint32_t w = read_u32(f);
    int32_t  h_signed = (int32_t)read_u32(f);
    int top_down = (h_signed < 0);
    uint32_t h = top_down ? (uint32_t)(-h_signed) : (uint32_t)h_signed;
    if (w == 0 || h == 0) { fclose(f); return LIT_ERR_FORMAT; }

    read_u16(f);                       /* planes, 必为 1 */
    uint16_t bpp = read_u16(f);
    uint32_t compression = read_u32(f);
    if (compression != 0) { fclose(f); return LIT_ERR_FORMAT; } /* 仅支持 BI_RGB */

    /* 跳过信息头剩余字段, 定位到调色板/像素数据 */
    long info_end = 14L + (long)info_size;
    if (ftell(f) < info_end) fseek(f, info_end, SEEK_SET);

    int channels;
    LITPixelFormat fmt;
    if (bpp == 8)       { fmt = LIT_FMT_GRAY; channels = 1; }
    else if (bpp == 24) { fmt = LIT_FMT_RGB;  channels = 3; }
    else if (bpp == 32) { fmt = LIT_FMT_RGBA; channels = 4; }
    else { fclose(f); return LIT_ERR_FORMAT; }

    /* 8 位灰度调色板(标准灰阶 ramp: r=g=b=index) */
    if (bpp == 8) {
        for (int i = 0; i < 256; i++) {
            int b = fgetc(f), g = fgetc(f), r = fgetc(f);
            fgetc(f); /* 保留字节 */
            if (b == EOF || g == EOF || r == EOF) break;
        }
    }

    /* 以文件头给出的 data_offset 为准定位像素区 */
    if (data_offset > 0) fseek(f, (long)data_offset, SEEK_SET);

    LITImage *img = lit_image_create((int)w, (int)h, fmt);
    if (!img) { fclose(f); return LIT_ERR_MEMORY; }

    int src_row_bytes = ((int)w * channels + 3) & ~3;
    uint8_t *rowbuf = (uint8_t*)malloc((size_t)src_row_bytes);
    if (!rowbuf) { lit_image_destroy(img); fclose(f); return LIT_ERR_MEMORY; }

    for (uint32_t y = 0; y < h; y++) {
        if (fread(rowbuf, 1, (size_t)src_row_bytes, f) != (size_t)src_row_bytes) {
            free(rowbuf); lit_image_destroy(img); fclose(f);
            return LIT_ERR_FORMAT;
        }
        /* BMP 行从底部向上存储; top-down 位图则相反 */
        uint32_t src_y = top_down ? y : (h - 1 - y);
        uint8_t *dst = img->data + (size_t)src_y * img->stride;

        for (uint32_t x = 0; x < w; x++) {
            if (channels == 1) {
                dst[x] = (uint8_t)rowbuf[x];
            } else {
                uint8_t bl = rowbuf[x * channels + 0];
                uint8_t gn = rowbuf[x * channels + 1];
                uint8_t rd = rowbuf[x * channels + 2];
                dst[x * channels + 0] = rd;   /* 内存统一用 RGB(A) 顺序 */
                dst[x * channels + 1] = gn;
                dst[x * channels + 2] = bl;   /* BMP 磁盘顺序为 BGR */
                if (channels == 4) dst[x * channels + 3] = rowbuf[x * channels + 3];
            }
        }
    }

    free(rowbuf);
    fclose(f);
    *out = img;
    return LIT_OK;
}

/* ---------- 保存 ---------- */
LITStatus lit_bmp_save(const LITImage *img, const char *path) {
    if (!img || !path) return LIT_ERR_NULL;

    int channels = (int)img->format;
    uint16_t bpp;
    if (channels == 1)      bpp = 8;
    else if (channels == 3) bpp = 24;
    else if (channels == 4) bpp = 32;
    else return LIT_ERR_FORMAT;

    int w = img->width, h = img->height;
    int row_bytes = w * channels;
    int stride = (row_bytes + 3) & ~3;
    uint32_t palette_size = (bpp == 8) ? 256u * 4u : 0u;
    uint32_t header_size = 14u + 40u + palette_size;
    uint32_t pixel_size = (uint32_t)stride * (uint32_t)h;
    uint32_t filesize = header_size + pixel_size;

    FILE *f = fopen(path, "wb");
    if (!f) return LIT_ERR_IO;

    /* 文件头 */
    fputc('B', f); fputc('M', f);
    write_u32(f, filesize);
    write_u32(f, 0);                  /* 保留 */
    write_u32(f, header_size);        /* 像素数据偏移 */

    /* 信息头 */
    write_u32(f, 40);                 /* BITMAPINFOHEADER 大小 */
    write_u32(f, (uint32_t)w);
    write_u32(f, (uint32_t)h);
    write_u16(f, 1);                  /* planes */
    write_u16(f, bpp);
    write_u32(f, 0);                  /* BI_RGB */
    write_u32(f, pixel_size);
    write_u32(f, 2835);               /* 约 72 DPI */
    write_u32(f, 2835);
    write_u32(f, (bpp == 8) ? 256u : 0u); /* 调色板颜色数 */
    write_u32(f, 0);                  /* 重要颜色数 */

    /* 8 位灰度调色板 */
    if (bpp == 8) {
        for (int i = 0; i < 256; i++) {
            fputc(i, f); fputc(i, f); fputc(i, f); fputc(0, f);
        }
    }

    /* 像素数据: 自底向上存储 */
    uint8_t *pad = (uint8_t*)calloc(1, 4);
    for (int y = h - 1; y >= 0; y--) {
        const uint8_t *src = img->data + (size_t)y * img->stride;
        for (int x = 0; x < w; x++) {
            if (channels == 1) {
                fputc(src[x], f);
            } else {
                uint8_t rd = src[x * channels + 0];
                uint8_t gn = src[x * channels + 1];
                uint8_t bl = src[x * channels + 2];
                fputc(bl, f); fputc(gn, f); fputc(rd, f); /* 转 BGR */
                if (channels == 4) fputc(src[x * channels + 3], f);
            }
        }
        int pad_bytes = stride - row_bytes;
        if (pad_bytes > 0) fwrite(pad, 1, (size_t)pad_bytes, f);
    }
    free(pad);

    if (ferror(f)) { fclose(f); return LIT_ERR_IO; }
    fclose(f);
    return LIT_OK;
}
