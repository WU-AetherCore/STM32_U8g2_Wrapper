/*------------------------------------------------------------------------------
 * 文件名称 : WU_U8g2_Init.h
 * 所属模块 : STM32_U8g2_Wrapper - 移植层（STM32CubeIDE + HAL 库）
 * 功能说明 : u8g2 移植适配头文件。
 *            定义全局 u8g2 结构体，声明 GPIO/延时回调与初始化函数。
 *
 * 编码要求 : UTF-8 编码保存。
 *
 * 依赖说明 : 延时使用 HAL_Delay()（HAL 库自带，1ms 精度）。
 *------------------------------------------------------------------------------
 */
#ifndef __WU_U8G2_INIT_H__
#define __WU_U8G2_INIT_H__

#include "u8g2.h"           /* 官方 u8g2 库 */
#include "IIC_OLED.h"       /* 底层软件 I2C（HAL 版） */

/* 全局 u8g2 结构体（在 WU_U8g2_Init.c 中定义，全工程共用） */
extern u8g2_t u8g2;

/**
 * @brief  u8g2 底层回调：GPIO 与延时消息处理
 * @note   由 u8g2 内部调用，实现延时与 SCL/SDA 电平控制
 */
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg,
                                  U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);

/**
 * @brief  u8g2 官方 Logo 演示画面（U / 8 / g / 2）
 * @note   可作为开机自检画面，绘制后内部已 SendBuffer 并延时 1 秒
 */
void draw(u8g2_t *u8g2);

/**
 * @brief  u8g2 初始化（Setup + InitDisplay + 开机 + 清屏）
 * @param  u8g2  u8g2 结构体指针
 */
void WU_u8g2Init(u8g2_t *u8g2);

#endif /* __WU_U8G2_INIT_H__ */
