/*------------------------------------------------------------------------------
 * 文件名称 : IIC_OLED.h
 * 所属模块 : STM32_U8g2_Wrapper - 移植层（Keil MDK + 标准外设库 SPL）
 * 功能说明 : 软件模拟 I2C 底层头文件。
 *            为 u8g2 提供 SCL/SDA 引脚电平控制接口。
 *
 * 编码要求 : UTF-8 编码保存。
 *------------------------------------------------------------------------------
 */
#ifndef __IIC_OLED_H__
#define __IIC_OLED_H__

#include "stm32f10x.h"      /* 标准外设库寄存器定义 */

/* IO 初始化：配置软件 I2C 引脚为推挽输出 */
void OLED_IoInit(void);

/* SCL/SDA 电平控制（由 u8g2 的 gpio 回调调用） */
void IIC_OLED_SDA_HIGH(void);
void IIC_OLED_SDA_LOW(void);
void IIC_OLED_SCL_HIGH(void);
void IIC_OLED_SCL_LOW(void);

#endif /* __IIC_OLED_H__ */
