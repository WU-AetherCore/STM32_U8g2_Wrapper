# Keil MDK 示例（标准外设库 SPL）

本目录是 **Keil MDK + STM32F103 标准外设库** 的移植层示例，提供软件模拟 I2C 与 u8g2 延时/GPIO 适配。

## 文件说明

| 文件 | 作用 |
| --- | --- |
| `IIC_OLED.c/h` | 软件模拟 I2C（SPL GPIO 位操作），引脚宏可修改（默认 PB8/PB9） |
| `WU_U8g2_Init.c/h` | 全局 `u8g2` 结构体、延时/GPIO 回调、`WU_u8g2Init()` 初始化、官方 Logo `draw()` |
| `main_example.c` | 使用示例：初始化 → 绘制中文/数值/图形 → 刷新 |

## 工程配置步骤

1. 使用正点原子 / 野火风格的 STM32F103 工程模板（自带 `sys.c`、`delay.c`、`usart.c`）。
2. 将本目录 4 个文件，以及仓库 `src/u8g2/`（官方库）、`src/WU_OLED/`（封装层）加入 Keil 工程编译。
3. 接线：OLED `SDA → PB8`，`SCL → PB9`（可在 `IIC_OLED.c` 顶部改引脚宏）。
4. **源码文件编码必须保存为 UTF-8**（Keil：Edit → Configuration → Encoding 选 "UTF-8 with BOM"），否则中文乱码。
5. 在 `main()` 中调用 `OLED_Example_Main()`（见 `main_example.c`）。

## 依赖提醒

- `WU_U8g2_Init.c` 使用 `Delay_ms()` / `Delay_us()`，来自工程模板的 `sys.c`（正点原子/野火模板自带）。若使用其他模板，请自行提供这两个延时函数。
- 若不需要释放 JTAG 引脚（PB3/PB4），可删除 `IIC_OLED.c` 中 `OLED_IoInit()` 的 AFIO 与 `SWJ_JTAGDisable` 两行。
