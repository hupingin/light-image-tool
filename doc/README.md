# 文档

本目录收录 light-image-tool 的项目文档。

| 文档 | 内容 |
| --- | --- |
| [架构设计](architecture.md) | 核心抽象 `LITImage`、模块划分与数据布局 |
| [构建指南](build.md) | 用 Make(gcc/clang) 或 MSVC 编译本项目 |
| [API 参考](api.md) | 核心、BMP、算子、JPEG 接口的函数契约 |
| [命令行用法](cli.md) | `bin/lit` 各子命令说明与示例 |
| [格式支持](formats.md) | 已支持 / 计划支持的图像格式与细节 |
| [测试](testing.md) | 如何运行与扩展测试 |

> 本项目为纯 C 实现，尽量少的外部依赖；用到其它项目的思想时只拷贝有用片段。
