/*------------------------------------------------------------------------------
 * 文件名称 : main_example.c
 * 所属模块 : STM32_U8g2_Wrapper - 示例（STM32CubeIDE + HAL 库）
 * 功能说明 : OLED 使用示例。展示：初始化 -> 清屏 -> 绘制文本/图形 -> 刷新。
 *
 * 工程要求 : 在 STM32CubeMX 工程中：
 *            1) 把 OLED 的 SDA / SCL 引脚配置为 GPIO_Output，
 *               User Label 命名为 OLED_SDA / OLED_SCL；
 *            2) 将 src 目录（u8g2 + WU_OLED）与本目录源文件加入工程；
 *            3) 本文件内容放入 main() 中（SystemClock_Config 之后）。
 *
 * 编码要求 : 本文件必须以 UTF-8 编码保存，否则中文字符串会乱码。
 *------------------------------------------------------------------------------
 */
#include "main.h"           /* CubeMX 生成：时钟、GPIO、HAL 头文件 */
#include "UI_OLED.h"        /* 封装层 + UI 层（自动包含 USER_OLED.h / u8g2.h） */

/* 全局毫秒计数：供屏保动效使用（可选，放在 SysTick 中断中自增） */
extern uint32_t Time_ms;

/**
 * @brief  主函数（在 main() 中调用，需在 SystemClock_Config 之后）
 * @note   示例代码，可按需修改
 */
void OLED_Example_Main(void)
{
    OLED_U8G2_Init();                   /* 一键初始化：IO + u8g2 + 清屏 */

    /* （可选）播放 u8g2 官方 Logo 1 秒 */
    /* draw(&u8g2); */

    /* 开机欢迎动画（阻塞约 1 秒） */
    WU_OLED_U8g2Init();

    /* 日常使用流程：清屏 -> 绘制 -> 刷新 */
    while (1)
    {
        WU_OLED_U8G2_ClearBuffer();

        /* 中文显示：使用 GB2312 中文字库 + DrawUTF8（源码必须为 UTF-8） */
        WU_OLED_U8G2_SetFont(u8g2_font_wqy13_t_gb2312a);
        WU_OLED_U8G2_DrawUTF8((128 - WU_OLED_U8G2_GetUTF8Width("你好，STM32_U8g2_Wrapper")) / 2, 20, "你好，STM32_U8g2_Wrapper");

        /* 数值显示 */
        WU_OLED_U8G2_SetFont(u8g2_font_ncenB14_tr);
        WU_OLED_U8G2_ShowNum(0, 40, 2026);

        /* 绘制图形：实心圆 + 空心圆角矩形 */
        WU_OLED_U8G2_DrawDisc(80, 44, 12, U8G2_DRAW_ALL);
        WU_OLED_U8G2_DrawRFrame(0, 32, 60, 24, 4);

        WU_OLED_U8G2_SendBuffer();      /* 刷新到屏幕 */

        HAL_Delay(500);                 /* 每 500ms 刷新一帧 */
    }
}
