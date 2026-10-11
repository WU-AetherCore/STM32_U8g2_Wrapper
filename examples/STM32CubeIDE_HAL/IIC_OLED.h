/*------------------------------------------------------------------------------
 * 文件名称 : IIC_OLED.h
 * 所属模块 : STM32_U8g2_Wrapper - 移植层（STM32CubeIDE + HAL 库）
 * 功能说明 : 软件模拟 I2C 底层头文件。
 *            为 u8g2 提供 SCL/SDA 引脚电平控制接口。
 *
 * 编码要求 : UTF-8 编码保存。
 *
 * 接线说明 : 引脚宏来自 CubeMX 生成的 main.h：
 *            OLED_SDA_GPIO_Port / OLED_SDA_Pin / OLED_SCL_GPIO_Port / OLED_SCL_Pin
 *            请在 CubeMX 中把 OLED 引脚 User Label 命名为 OLED_SDA、OLED_SCL，
 *            或将下方实现改为手动 #define 引脚。
 *------------------------------------------------------------------------------
 */
#ifndef __IIC_OLED_H__
#define __IIC_OLED_H__

#include "main.h"        /* CubeMX 生成：引脚宏与 HAL 头文件 */

/* IO 初始化（CubeMX 已配置 GPIO 为推挽输出，通常为空实现） */
void OLED_IoInit(void);

/* SCL/SDA 电平控制（由 u8g2 的 gpio 回调调用） */
void IIC_OLED_SDA_HIGH(void);
void IIC_OLED_SDA_LOW(void);
void IIC_OLED_SCL_HIGH(void);
void IIC_OLED_SCL_LOW(void);

#endif /* __IIC_OLED_H__ */
