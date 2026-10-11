# STM32CubeIDE 示例（HAL 库）

本目录是 **STM32CubeIDE + HAL 库** 的移植层示例，提供软件模拟 I2C 与 u8g2 延时/GPIO 适配。

## 文件说明

| 文件 | 作用 |
| --- | --- |
| `IIC_OLED.c/h` | 软件模拟 I2C（HAL GPIO 位操作），引脚宏来自 CubeMX 生成的 `main.h` |
| `WU_U8g2_Init.c/h` | 全局 `u8g2` 结构体、延时/GPIO 回调、`WU_u8g2Init()` 初始化、官方 Logo `draw()` |
| `main_example.c` | 使用示例：初始化 → 绘制中文/数值/图形 → 刷新 |

## 工程配置步骤

1. 在 STM32CubeMX 中新建工程，把 OLED 的 `SDA` / `SCL` 引脚配置为 `GPIO_Output`，**User Label 命名为 `OLED_SDA` / `OLED_SCL`**（这样 `main.h` 会自动生成 `OLED_SDA_Pin`、`OLED_SDA_GPIO_Port` 等宏）。不叫这个名字也可以，改 `IIC_OLED.c` 里的引用即可。
2. 把仓库 `src/u8g2/`（官方库）与 `src/WU_OLED/`（封装层）复制进工程，连同本目录源文件一起加入编译。
3. 接线：OLED `SDA` / `SCL` 接到 CubeMX 配置的引脚（示例用 PB8/PB9）。
4. **源码文件编码必须保存为 UTF-8**（CubeIDE 默认即 UTF-8，注意不要手动改成 GBK），否则中文乱码。
5. 在 `main()` 中 `SystemClock_Config()` 之后调用 `OLED_Example_Main()`（见 `main_example.c`）。

## 依赖提醒

- 延时使用 HAL 自带的 `HAL_Delay()`，无需额外移植。
- 初始化方向参数为 `U8G2_MIRROR_VERTICAL`（垂直镜像，原工程调试参数），如需横屏/翻转可在 `WU_U8g2_Init.c` 的 `WU_u8g2Init()` 中改为 `U8G2_R0` ~ `U8G2_R3`。
