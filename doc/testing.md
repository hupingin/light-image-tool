# 测试

测试脚本位于 `test/test_bmp.py`，用 **Python 标准库**（无需任何第三方包）实现，
充当被测 C 程序的「独立裁判」。

## 运行

```bash
python test/test_bmp.py
```

输出示例：

```
  PASS  info rc
  PASS  info dims
  PASS  info fmt
  PASS  info stride
  PASS  to-gray values
  PASS  flip-h values
  PASS  flip-v values
  PASS  rot90 dims
  PASS  rot90 values
  PASS  gray info fmt
  PASS  gray flip-h values
  PASS  rgba info fmt
  PASS  rgba flip-h values

结果: 全部通过
```

## 它测了什么

脚本本身生成带**已知像素值**的 BMP，再调用 `bin/lit.exe` 各子命令，最后把输出 BMP
独立解析回来，与预期变换逐像素比对：

- `info`：RGB / GRAY / RGBA 的维度、格式、stride 是否正确；
- `to-gray`：灰度值是否符合亮度公式 `int(0.299R+0.587G+0.114B)`；
- `flip-h` / `flip-v`：翻转后像素位置是否正确；
- `rot90`：旋转后尺寸与每个像素是否正确；
- **8 位灰度**与 **32 位 RGBA** 的「载入 → 变换 → 保存 → 再载入」往返一致性。

生成的临时文件放在 `test/out/`（已被 `.gitignore` 忽略）。

## 如何扩展

新增用例只需：

1. 用脚本里的 `write_bmp_24/8/32` 生成输入；
2. 调用 `run([...])` 触发 `lit` 子命令；
3. 用 `read_bmp` 读回输出，与手工计算的 `exp_*` 对比，并通过 `check(name, cond)` 断言。

由于 BMP 读写由脚本独立实现（与被测 C 代码无关），能真实检验 `lit` 的行为是否正确。
