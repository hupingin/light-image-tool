# 项目长期笔记: light-image-tool

纯 C 图像中间件, MIT 协议(作者 hopeking)。原则: 尽量少依赖, 用到别处思想时只拷贝有用片段。

- **两种发布形态**: ① 按模块拆分动态库(.so/.dll); ② 统一二进制进程(`bin/lit`)。
- **核心抽象**: `LITImage`(`src/light_image.h`), 行优先、每行 4 字节对齐、像素统一 RGB(A) 顺序。
- **已落地**: 核心抽象、BMP 编解码(8/24/32 位, 纯 C)、基础算子(灰度/翻转/旋转)、CLI。
- **待实现**: JPEG 编解码(接口已在 `src/jpeg/include/jpeg.h` 声明)、PNG、更多算子、格式自动注册。
- **构建**: `make` = 统一二进制; `make libs` = 动态库。需 gcc/clang/MinGW。
- 本机环境无 C 编译器, 改动后需在装有编译器的机器上验证。
