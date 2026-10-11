/*------------------------------------------------------------------------------
 * 文件名称 : WU_U8g2_Init.c
 * 所属模块 : STM32_U8g2_Wrapper - 移植层（Keil MDK + 标准外设库 SPL）
 * 功能说明 : u8g2 移植适配实现。
 *            1) 定义全局 u8g2 结构体；
 *            2) 实现延时 / GPIO 回调 u8g2_gpio_and_delay_stm32()；
 *            3) 提供官方 Logo 演示画面 draw()；
 *            4) 提供统一初始化入口 WU_u8g2Init()。
 *
 * 编码要求 : UTF-8 编码保存。
 *
 * 依赖说明 : 需要 sys.h / sys.c 提供 Delay_ms 与 Delay_us
 *            （正点原子、野火等 STM32F103 工程模板自带）。
 *------------------------------------------------------------------------------
 */
#include "WU_U8g2_Init.h"
#include "u8g2.h"
#include "sys.h"            /* 延时函数：Delay_ms() / Delay_us() */

/* 全局 u8g2 结构体：整个工程（含封装层、UI 层）共用的显示上下文 */
u8g2_t u8g2;

/**
 * @brief  u8g2 底层回调：延时与软件 I2C 电平控制
 * @param  u8x8     u8x8 结构体（本移植未使用）
 * @param  msg      u8g2 消息类型（延时 / GPIO）
 * @param  arg_int  消息参数（延时时为毫秒数；GPIO 时为电平 0/1）
 * @param  arg_ptr  附加指针参数（未使用）
 * @return 1 = 消息处理成功；0 = 未处理的消息
 * @note   该回调必须被 u8g2_Setup_* 系列函数作为参数传入
 */
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg,
                                  U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_DELAY_MILLI:      /* 毫秒延时，arg_int 为毫秒数 */
            Delay_ms(arg_int);
            break;

        case U8X8_MSG_DELAY_10MICRO:    /* 10 微秒延时 */
            Delay_us(10);
            break;

        case U8X8_MSG_DELAY_100NANO:    /* 100 纳秒延时（近似） */
            __NOP();
            break;

        case U8X8_MSG_GPIO_I2C_CLOCK:   /* I2C 时钟线 SCL 电平 */
            if (arg_int)
                IIC_OLED_SCL_HIGH();
            else
                IIC_OLED_SCL_LOW();
            break;

        case U8X8_MSG_GPIO_I2C_DATA:    /* I2C 数据线 SDA 电平 */
            if (arg_int)
                IIC_OLED_SDA_HIGH();
            else
                IIC_OLED_SDA_LOW();
            break;

        default:
            return 0;                   /* 未实现的消息，返回 0 表示错误 */
    }
    return 1;                           /* 消息处理成功 */
}

/**
 * @brief  u8g2 官方 Logo 演示画面（U / 8 / g / 2）
 * @param  u8g2  u8g2 结构体指针
 * @note   绘制后内部已 SendBuffer 并延时 1 秒；
 *         可用作开机自检画面（可选调用）
 */
void draw(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2);
    u8g2_SetFontMode(u8g2, 1);
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 0, 20, "U");

    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21, 8, "8");

    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51, 30, "g");
    u8g2_DrawStr(u8g2, 67, 30, "\xb2");

    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);

    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1, 54, "github.com/olikraus/u8g2");

    u8g2_SendBuffer(u8g2);
    Delay_ms(1000);
}

/**
 * @brief  u8g2 初始化
 * @param  u8g2  u8g2 结构体指针
 * @note   流程：
 *         1) u8g2_Setup_ssd1306_i2c_128x64_noname_f：绑定屏幕驱动、
 *            旋转方向、软件 I2C 字节发送函数与本回调；
 *         2) u8g2_InitDisplay：按所选芯片执行初始化（完成后屏幕关闭）；
 *         3) u8g2_SetPowerSave(0)：开启显示；
 *         4) u8g2_ClearBuffer：清空缓冲。
 * @note   旋转方向参数（第二参数）：
 *         U8G2_R0 / U8G2_R1 / U8G2_R2 / U8G2_R3  分别对应 0°/90°/180°/270°
 *         U8G2_MIRROR           水平镜像
 *         U8G2_MIRROR_VERTICAL  垂直镜像
 */
void WU_u8g2Init(u8g2_t *u8g2)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8g2_gpio_and_delay_stm32);
    u8g2_InitDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0);     /* 0 = 开显示，1 = 关显示 */
    u8g2_ClearBuffer(u8g2);
}
