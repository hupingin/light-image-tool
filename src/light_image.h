#ifndef LIGHT_IMAGE_H
#define LIGHT_IMAGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 版本号 ---- */
#define LIT_VERSION_MAJOR 0
#define LIT_VERSION_MINOR 1
#define LIT_VERSION_PATCH 0

/* ---- 像素格式 ----
   用"每像素通道数"直接表示, 便于扩展。 */
typedef enum {
    LIT_FMT_GRAY = 1,  /* 1 通道: 灰度 */
    LIT_FMT_RGB  = 3,  /* 3 通道: R,G,B */
    LIT_FMT_RGBA = 4   /* 4 通道: R,G,B,A */
} LITPixelFormat;

/* ---- 图像句柄 ----
   像素数据按"行优先(row-major)"紧凑存放, 每行按 4 字节对齐(见 stride)。 */
typedef struct {
    int            width;    /* 像素宽 */
    int            height;   /* 像素高 */
    LITPixelFormat format;   /* 像素格式 */
    int            stride;   /* 每行字节数(含行尾填充) */
    uint8_t       *data;     /* 像素缓冲区, 长度 = stride * height */
} LITImage;

/* ---- 返回码 ---- */
typedef enum {
    LIT_OK = 0,
    LIT_ERR_NULL = 1,         /* 空指针参数 */
    LIT_ERR_MEMORY = 2,       /* 内存分配失败 */
    LIT_ERR_FORMAT = 3,       /* 不支持/损坏的文件格式 */
    LIT_ERR_IO = 4,           /* 文件读写错误 */
    LIT_ERR_UNSUPPORTED = 5   /* 暂未实现的操作 */
} LITStatus;

/* 创建一张空图像(像素清零)。尺寸非法或内存不足时返回 NULL。 */
LITImage* lit_image_create(int width, int height, LITPixelFormat fmt);

/* 释放图像及其像素缓冲区(NULL 安全)。 */
void lit_image_destroy(LITImage *img);

/* 通道数(等于 format 枚举值)。 */
int lit_image_channels(const LITImage *img);

/* 一行"有效像素"的字节数(不含行尾填充)。 */
int lit_image_row_bytes(const LITImage *img);

/* 返回指向 (x,y) 像素首字节的指针; 越界返回 NULL。 */
uint8_t* lit_image_pixel(LITImage *img, int x, int y);

/* 把错误码转成易读字符串。 */
const char* lit_status_string(LITStatus s);

#ifdef __cplusplus
}
#endif

#endif /* LIGHT_IMAGE_H */
