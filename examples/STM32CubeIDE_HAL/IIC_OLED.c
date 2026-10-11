/*------------------------------------------------------------------------------
 * 文件名称 : IIC_OLED.c
 * 所属模块 : STM32_U8g2_Wrapper - 移植层（STM32CubeIDE + HAL 库）
 * 功能说明 : 软件模拟 I2C 底层实现（HAL GPIO 位操作）。
 *            u8g2 通过 u8g2_gpio_and_delay_stm32() 回调本文件的
 *            IIC_OLED_SCL_HIGH/LOW 与 IIC_OLED_SDA_HIGH/LOW 驱动屏幕。
 *
 * 接线说明 : SDA / SCL 引脚由 CubeMX 配置为 GPIO_Output，
 *            并将 User Label 命名为 OLED_SDA / OLED_SCL，
 *            生成的宏（OLED_SDA_Pin 等）在 main.h 中声明。
 *
 * 编码要求 : UTF-8 编码保存。
 *------------------------------------------------------------------------------
 */
#include "IIC_OLED.h"

/**
 * @brief  软件 I2C 引脚初始化
 * @note   CubeMX 已把引脚配置为推挽输出，此处留空即可；
 *         如未使用 CubeMX 初始化引脚，可在此处补 HAL_GPIO_Init
 */
void OLED_IoInit(void)
{
    /* 由 CubeMX 的 MX_GPIO_Init() 完成，无需额外处理 */
}

/* 以下电平控制函数由 u8g2 的 GPIO 消息回调驱动 */
void IIC_OLED_SDA_HIGH(void)
{
    HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA_Pin, GPIO_PIN_SET);
}

void IIC_OLED_SDA_LOW(void)
{
    HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA_Pin, GPIO_PIN_RESET);
}

void IIC_OLED_SCL_HIGH(void)
{
    HAL_GPIO_WritePin(OLED_SCL_GPIO_Port, OLED_SCL_Pin, GPIO_PIN_SET);
}

void IIC_OLED_SCL_LOW(void)
{
    HAL_GPIO_WritePin(OLED_SCL_GPIO_Port, OLED_SCL_Pin, GPIO_PIN_RESET);
}
