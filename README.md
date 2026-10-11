# STM32_U8g2_Wrapper

一个面向 **STM32 + SSD1306(128x64 OLED)** 的 **u8g2 图形库个人封装层**，由作者在实战项目中逐步封装沉淀而来，现整理开源。

> 原项目名：`WUYDD-U8g2`（Keil / STM32CubeIDE 双平台工程），V2.0 起整理为统一仓库结构并开源。

- 对官方 [u8g2](https://github.com/olikraus/u8g2) 全量 API 做了一层 `WU_OLED_U8G2_` 前缀的二次封装，**一行一个功能，调用简洁、注释齐全**
- 额外提供 **OLED 格式化打印**（`WU_OLED_U8G2_Printf`，支持中文）、**整数/浮点显示**、**屏保动效**、**位图缩放** 等实用函数
- 内置 **开机欢迎动画**（进度条 + 模糊转场）与三种缓动动画（线性/缓入/缓出）
- 同时适配 **Keil MDK（标准外设库 SPL）** 与 **STM32CubeIDE（HAL 库）** 两种工程
- 源码统一 **UTF-8 编码**，中文显示开箱即用（配合 GB2312 中文字库）

---

## 目录结构

```
STM32_U8g2_Wrapper/
├── README.md                     # 本说明文档
├── LICENSE                       # MIT 许可证
├── THIRD_PARTY_NOTICES.md        # 第三方组件声明（u8g2 / MUI）
├── src/                          # 平台无关的共享源码
│   ├── u8g2/                     # 官方 u8g2 库（含 MUI 菜单库），原样保留
│   └── WU_OLED/                  # ★ 个人封装层（本仓库核心）
│       ├── USER_OLED.c/h         #   u8g2 API 二次封装 + 实用函数
│       ├── UI_OLED.c/h           #   一键初始化 / 开机动画 / UI 动效
│       └── WU_OLED_Bitmap.c/h    #   内置位图资源（点赞/诗人/机器人/刀具/爱心）
└── examples/                     # 移植层示例（二选一）
    ├── Keil_MDK_STM32F103/       #   Keil MDK + 标准外设库（软件 I2C）
    │   ├── IIC_OLED.c/h
    │   ├── WU_U8g2_Init.c/h
    │   └── main_example.c
    └── STM32CubeIDE_HAL/         #   STM32CubeIDE + HAL 库（软件 I2C）
        ├── IIC_OLED.c/h
        ├── WU_U8g2_Init.c/h
        └── main_example.c
```

- `src/` 与 `examples/` 分层：官方库与个人封装层**只保留一份**，两个平台通过各自的移植层（I2C 底层 + 延时回调）接入。
- 屏幕默认 **SSD1306 / 128x64 / I2C**（软件模拟 I2C）。换其他屏幕只需改 `WU_U8g2_Init.c` 中的 `u8g2_Setup_*` 函数。

---

## 快速开始

### 硬件接线（I2C 软件模拟）

| OLED | STM32 | 说明 |
| --- | --- | --- |
| GND | GND | 共地 |
| VCC | 3.3V | 部分模块为 5V 供电，注意模块丝印 |
| SCL | PB9（可改） | 时钟线 |
| SDA | PB8（可改） | 数据线 |

> 软件 I2C 任意 GPIO 均可，改 `examples` 下 `IIC_OLED.c` 顶部的引脚宏即可。

### Keil MDK（标准外设库）

1. 使用正点原子 / 野火风格 STM32F103 工程模板（自带 `sys.c`/`delay.c`）。
2. 把 `src/u8g2/`、`src/WU_OLED/`、`examples/Keil_MDK_STM32F103/` 下文件加入工程编译。
3. **所有 .c/.h 文件编码设为 UTF-8（with BOM）**，避免中文乱码。
4. 在 `main()` 调用示例入口：

```c
#include "UI_OLED.h"

int main(void)
{
    delay_init();           // 工程模板延时初始化
    OLED_U8G2_Init();       // 一键初始化：IO + u8g2 + 清屏
    // draw(&u8g2);         // 可选：u8g2 官方 Logo 自检画面
    WU_OLED_U8g2Init();     // 可选：开机欢迎动画

    WU_OLED_U8G2_ClearBuffer();
    WU_OLED_U8G2_SetFont(u8g2_font_wqy13_t_gb2312a);
    WU_OLED_U8G2_DrawUTF8(0, 20, "你好，u8g2！");
    WU_OLED_U8G2_SendBuffer();

    while (1) {}
}
```

### STM32CubeIDE（HAL 库）

1. CubeMX 中把 OLED `SDA`/`SCL` 引脚配为 `GPIO_Output`，User Label 命名为 `OLED_SDA`/`OLED_SCL`。
2. 把 `src/u8g2/`、`src/WU_OLED/`、`examples/STM32CubeIDE_HAL/` 下文件加入工程。
3. 在 `main()` 中 `SystemClock_Config()` 之后调用同样的初始化序列（见 `examples/STM32CubeIDE_HAL/main_example.c`）。

---

## 常用 API 速查

### 基础操作

| 函数 | 说明 |
| --- | --- |
| `OLED_U8G2_Init()` | 一键初始化（IO + u8g2 + 清屏） |
| `WU_OLED_U8G2_ClearBuffer()` | 清空绘制缓冲区 |
| `WU_OLED_U8G2_SendBuffer()` | 刷新缓冲区到屏幕 |
| `WU_OLED_U8G2_SetDrawColor(mode)` | 绘图颜色：0 透显 / 1 实显 / 2 反色(XOR) |
| `WU_OLED_U8G2_SetFont(font)` | 选择字库（`u8g2_font_*`） |

### 图形绘制

| 函数 | 说明 |
| --- | --- |
| `WU_OLED_U8G2_DrawPoint / DrawHLine / DrawVLine / DrawLine` | 点 / 水平线 / 竖直线 / 线段 |
| `WU_OLED_U8G2_DrawBox / DrawFrame` | 实心 / 空心矩形 |
| `WU_OLED_U8G2_DrawRBox / DrawRFrame` | 实心 / 空心圆角矩形 |
| `WU_OLED_U8G2_DrawCircle / DrawDisc` | 空心 / 实心圆 |
| `WU_OLED_U8G2_DrawEllipse / DrawFilledEllipse` | 空心 / 实心椭圆 |
| `WU_OLED_U8G2_DrawTriangle` | 实心三角形 |
| `WU_OLED_U8G2_ShowBMP(x, y, w, h, pic)` | 绘制单色位图（XBMP） |
| `WU_OLED_U8G2_DrawButtonUTF8(...)` | 给文本加外框（按钮效果） |

### 文本显示

| 函数 | 说明 |
| --- | --- |
| `WU_OLED_U8G2_DrawStr(x, y, s)` | 绘制 ASCII 字符串 |
| `WU_OLED_U8G2_DrawUTF8(x, y, s)` | 绘制 UTF-8 字符串（**中文用这个**） |
| `WU_OLED_U8G2_GetUTF8Width(s)` | 获取字符串像素宽度（居中排版常用） |
| `WU_OLED_U8G2_ShowNum(x, y, num)` | 显示整数 |
| `WU_OLED_U8G2_ShowFloat(x, y, f, intLen, fraLen)` | 显示浮点数（指定整数/小数位数） |
| `WU_OLED_U8G2_Printf(x, y, fontSize, fmt, ...)` | **printf 风格打印**（支持中文档位） |

### 其他实用函数

| 函数 | 说明 |
| --- | --- |
| `WU_OLED_U8G2_SetOledInverseColor(flag)` | OLED 整体反色 |
| `WU_OLED_U8G2_SetDisplayRotation(cb)` | 屏幕旋转（R0~R3 / 镜像） |
| `WU_OLED_U8G2_SetClipWindow(...)` | 裁剪显示区域 |
| `WU_OLED_U8G2_SetFontDirection(dir)` | 字体书写方向（0~3） |
| `WU_OLED_U8G2_GetBufferPtr()` | 获取缓冲区指针（做特效用） |
| `WU_OLED_RandomLines()` | 屏保：随机线条 |
| `WU_OLED_Noise()` | 屏保：模拟噪点 |
| `WU_OLED_DrawSlowBitmapResize(...)` | 位图缩放绘制 |

> 完整声明见 `src/WU_OLED/USER_OLED.h`，每个函数都有参数/返回值/示例注释。

---

## 中文显示指南（重要）

1. 使用 **GB2312 字符集中文字库**：`u8g2_font_wqy13_t_gb2312a`（13px，推荐）等，`src/u8g2/u8g2.h` 中可查到全部 `wqy` 字体。
2. 中文字符串必须通过 `WU_OLED_U8G2_DrawUTF8()` / `WU_OLED_U8G2_Printf()` 绘制。
3. **源码文件必须以 UTF-8 编码保存**（u8g2 的 UTF8 解析器按 UTF-8 解码）。
   - Keil MDK：`Edit → Configuration → Editor → Encoding` 选 **UTF-8 with BOM**；
   - STM32CubeIDE：默认 UTF-8，勿手动改回 GBK。
4. 内存紧张时可改用更小的字库（如 `u8g2_font_wqy12_t_gb2312`），或只编译需要的字体文件以减少 Flash 占用（注意 `u8g2_fonts.c` 较大，可自行裁剪）。

---

## 常见问题（FAQ）

**Q1：屏幕不亮 / 无显示？**
- 检查接线（VCC/GND/SCL/SDA）与供电；检查 `IIC_OLED.c` 引脚宏是否与实际接线一致。
- Keil 版确认 `Delay_ms/Delay_us` 已由工程模板提供；CubeIDE 版确认 `HAL_Delay` 可用（HAL 已初始化）。
- 换屏幕型号时确认 `WU_U8g2_Init.c` 中 `u8g2_Setup_*` 与屏幕驱动一致。

**Q2：中文显示乱码 / 方块？**
- 99% 是**源码文件编码不是 UTF-8**（Keil 默认 GBK）。把文件另存为 UTF-8（with BOM）再编译。
- 确认使用了中文字库（`u8g2_font_wqy*_t_gb2312*`），并调用 `DrawUTF8` 而非 `DrawStr`。

**Q3：画面上下/左右颠倒？**
- 改 `WU_U8g2_Init.c` 中 `WU_u8g2Init()` 的旋转参数：`U8G2_R1/R2/R3` 或 `U8G2_MIRROR*`。

**Q4：`u8g2_fonts.c` 太大（31MB 源码），Flash 不够？**
- 该文件包含全量字体。裁剪方法：删除 `u8g2_fonts.c` 中不需要的字体数组，并同步删除 `u8g2.h` 中对应 `extern` 声明；或换用 `tools/font` 自行生成精简字库。

**Q5：`Time_ms` 是干嘛的？**
- 屏保动效的全局时钟。在 SysTick/定时器中断里 `Time_ms++`（或加周期毫秒数）即可；不使用屏保可忽略。

---

## 变更记录

| 版本 | 日期 | 说明 |
| --- | --- | --- |
| V2.0 | 2026-10-11 | 由原 `WUYDD-U8g2` 整理开源：合并 Keil 与 CubeIDE 双版本封装差异（补齐 `Printf` 等函数）；官方 u8g2 核心去重为一份；屏保/位图缩放函数统一 `WU_OLED_` 前缀命名；位图资源独立成模块；全量补充函数注释；统一 UTF-8 编码；新增 README/示例/许可证 |

> ⚠️ 与原工程不兼容的函数改名：`OLED_RandomLines → WU_OLED_RandomLines`、`OLED_Noise → WU_OLED_Noise`、`OLED_DrawIntensiveComputing → WU_OLED_DrawIntensiveComputing`、`Oled_DrawSlowBitmapResize → WU_OLED_DrawSlowBitmapResize`。

## 许可证

- 本仓库封装层代码（`src/WU_OLED/`、`examples/`）采用 [MIT License](LICENSE)。
- 内置官方 u8g2 库版权归其作者所有（MIT），详见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

## 致谢

- [olikraus/u8g2](https://github.com/olikraus/u8g2)：优秀的单色图形库，本项目的基础。
- 文泉驿点阵字库（wqy）：中文点阵字体来源。
- 位图缩放片段改编自 [Arduboy2](https://github.com/MLXXXp/Arduboy2)。

欢迎 Star、Fork、提 Issue 或 PR！
