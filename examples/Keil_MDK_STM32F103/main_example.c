/*------------------------------------------------------------------------------
 * 文件名称 : main_example.c
 * 所属模块 : STM32_U8g2_Wrapper - 示例（Keil MDK + 标准外设库 SPL）
 * 功能说明 : OLED 使用示例。展示：初始化 -> 清屏 -> 绘制文本/图形 -> 刷新。
 *
 * 工程要求 : 本示例假设使用 STM32F103 + 标准外设库工程模板
 *            （正点原子 / 野火风格，自带 sys.c / delay.c / usart.c）。
 *            只需把本文件与 src 目录加入工程编译即可。
 *
 * 编码要求 : 本文件必须以 UTF-8 编码保存（Keil 中选 "UTF-8 with BOM"），
 *            否则中文字符串会乱码。
 *
 * 接线     : OLED SDA -> PB8，SCL -> PB9（见 IIC_OLED.c 引脚宏）
 *------------------------------------------------------------------------------
 */
#include "sys.h"            /* 工程模板：系统时钟等 */
#include "delay.h"          /* 工程模板：延时 */
#include "usart.h"          /* 工程模板：串口（可选） */
#include "UI_OLED.h"        /* 封装层 + UI 层（自动包含 USER_OLED.h / u8g2.h） */

/* 全局毫秒计数：供屏保动效使用（可选，放在 SysTick 中断中自增） */
extern uint32_t Time_ms;

/**
 * @brief  主函数
 * @note   在工程模板的 main() 中调用本函数即可
 */
void OLED_Example_Main(void)
{
    delay_init();                       /* 模板：延时初始化（提供 Delay_ms/Delay_us） */

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

        Delay_ms(500);                  /* 每 500ms 刷新一帧 */
    }
}
