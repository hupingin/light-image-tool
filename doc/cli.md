# 命令行用法

统一二进制 `bin/lit`（由 `make` 或 `build_msvc.bat` 产出）提供以下子命令。

## 通用

```bash
lit info <file.bmp>                  # 查看图片信息
lit to-gray <in.bmp> <out.bmp>       # 转为灰度
lit flip-h  <in.bmp> <out.bmp>       # 水平翻转
lit flip-v  <in.bmp> <out.bmp>       # 垂直翻转
lit rot90   <in.bmp> <out.bmp>       # 顺时针旋转 90°
```

不带参数运行会打印用法与版本号。

## `info`

输出图片的尺寸、像素格式与每行字节数（stride）。

```bash
$ lit info demo.bmp
demo.bmp: 4x3, format=RGB, stride=12
```

- `format` 取值：`GRAY` / `RGB` / `RGBA`
- `stride` = 每行字节数（含 4 字节对齐填充）

## `to-gray`

将彩色图转为灰度图（亮度法），输出 8 位灰度 BMP。

```bash
$ lit to-gray in.bmp gray.bmp
```

## `flip-h` / `flip-v`

水平（左右）/ 垂直（上下）镜像。输入与输出格式一致。

```bash
$ lit flip-h in.bmp mirrored.bmp
```

## `rot90`

顺时针旋转 90°。输出尺寸为「宽=原高，高=原宽」。

```bash
$ lit rot90 in.bmp rotated.bmp
```

## 退出码

- `0`：成功
- `1`：参数缺失或处理失败（具体原因打印到 stderr）

## 示例流程

```bash
lit info   photo.bmp
lit to-gray photo.bmp photo_gray.bmp
lit flip-h  photo_gray.bmp photo_gray_h.bmp
lit rot90   photo_gray_h.bmp photo_gray_h_90.bmp
```
