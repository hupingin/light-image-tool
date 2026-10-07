/*
 *  light-image-tool 统一命令行入口
 *  适合"二进制进程"形态的发布(见 README 的两种发布形态)。
 *
 *  Copyright 2023 hopeking.
 *  See ../LICENSE for licensing terms.
 */

#include "light_image.h"
#include "core/bmp.h"
#include "core/ops.h"
#include <stdio.h>
#include <string.h>

static int run_info(const char *path) {
    LITImage *img = NULL;
    LITStatus s = lit_bmp_load(path, &img);
    if (s != LIT_OK) {
        fprintf(stderr, "load failed: %s\n", lit_status_string(s));
        return 1;
    }
    const char *fmt = img->format == LIT_FMT_GRAY ? "GRAY"
                    : img->format == LIT_FMT_RGB  ? "RGB"
                                                  : "RGBA";
    printf("%s: %dx%d, format=%s, stride=%d\n",
           path, img->width, img->height, fmt, img->stride);
    lit_image_destroy(img);
    return 0;
}

static int run_transform(const char *cmd, const char *in, const char *out) {
    LITImage *src = NULL, *dst = NULL;
    LITStatus s = lit_bmp_load(in, &src);
    if (s != LIT_OK) {
        fprintf(stderr, "load %s failed: %s\n", in, lit_status_string(s));
        return 1;
    }

    if      (strcmp(cmd, "to-gray") == 0) s = lit_op_grayscale(src, &dst);
    else if (strcmp(cmd, "flip-h")  == 0) s = lit_op_flip_h(src, &dst);
    else if (strcmp(cmd, "flip-v")  == 0) s = lit_op_flip_v(src, &dst);
    else if (strcmp(cmd, "rot90")   == 0) s = lit_op_rotate90(src, &dst);
    else { lit_image_destroy(src); return 1; }

    if (s != LIT_OK) {
        fprintf(stderr, "transform failed: %s\n", lit_status_string(s));
        lit_image_destroy(src);
        return 1;
    }

    s = lit_bmp_save(dst, out);
    if (s != LIT_OK) {
        fprintf(stderr, "save %s failed: %s\n", out, lit_status_string(s));
    }

    lit_image_destroy(src);
    lit_image_destroy(dst);
    return s == LIT_OK ? 0 : 1;
}

static void usage(void) {
    printf("light-image-tool v%d.%d.%d\n",
           LIT_VERSION_MAJOR, LIT_VERSION_MINOR, LIT_VERSION_PATCH);
    printf("usage:\n");
    printf("  lit info <file.bmp>\n");
    printf("  lit to-gray <in.bmp> <out.bmp>\n");
    printf("  lit flip-h  <in.bmp> <out.bmp>\n");
    printf("  lit flip-v  <in.bmp> <out.bmp>\n");
    printf("  lit rot90   <in.bmp> <out.bmp>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) { usage(); return 0; }

    const char *cmd = argv[1];

    if (strcmp(cmd, "info") == 0) {
        if (argc < 3) { usage(); return 1; }
        return run_info(argv[2]);
    }

    if (strcmp(cmd, "to-gray") == 0 ||
        strcmp(cmd, "flip-h")  == 0 ||
        strcmp(cmd, "flip-v")  == 0 ||
        strcmp(cmd, "rot90")   == 0) {
        if (argc < 4) { usage(); return 1; }
        return run_transform(cmd, argv[2], argv[3]);
    }

    usage();
    return 1;
}
