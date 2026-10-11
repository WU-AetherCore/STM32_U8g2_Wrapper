/*------------------------------------------------------------------------------
 * 文件名称 : UI_OLED.h
 * 所属模块 : STM32_U8g2_Wrapper - 个人封装层
 * 功能说明 : OLED 界面（UI）层头文件。
 *            提供整体初始化入口、开机动画与常用 UI 动效函数。
 *
 * 编码要求 : UTF-8 编码保存。
 *
 * 依赖说明 : 依赖移植层提供的两个接口（由 examples 下的 IIC_OLED /
 *            WU_U8g2_Init 实现）：
 *              void       OLED_IoInit(void);        // IO 初始化
 *              void       WU_u8g2Init(u8g2_t*);    // u8g2 初始化
 *------------------------------------------------------------------------------
 */
#ifndef __UI_OLED_H__
#define __UI_OLED_H__

#include "USER_OLED.h"      /* 个人封装层（含 u8g2.h、全局 u8g2 结构体） */

/**
 * @brief  OLED 一键初始化
 * @note   依次完成：IO 初始化 -> u8g2 初始化（含清屏）
 */
void OLED_U8G2_Init(void);

/**
 * @brief  开机欢迎动画（HelloHOPE 进度条 + 模糊转场）
 * @note   阻塞式执行，直到动画完成返回
 */
void WU_OLED_U8g2Init(void);

/**
 * @brief  UI 模糊转场效果（直接操作显示缓冲区实现“模糊退场”）
 * @return 1 = 动画未完成，0 = 动画已完成
 */
uint8_t HugoUI(void);

/**
 * @brief  线性滑动动画
 * @param  a      当前值指针（会被逐步修改）
 * @param  a_trg  目标值指针
 * @param  n      运动速度（越大越慢）
 * @return 1 = 动画进行中，0 = 已到达目标
 */
uint8_t WU_UI_Animation_Linear(float *a, float *a_trg, uint8_t n);

/**
 * @brief  缓出动画（缓慢结束）
 * @param  a      当前值指针
 * @param  a_trg  目标值指针
 * @param  n      运动速度（越大越慢）
 * @return 1 = 动画进行中，0 = 已到达目标
 */
uint8_t HugoUI_Animation_EasyOut(float *a, float *a_trg, uint16_t n);

/**
 * @brief  缓入动画（缓慢进入）
 * @param  a      当前值指针
 * @param  a_trg  目标值指针
 * @param  n      运动速度（越大越慢）
 * @return 1 = 动画进行中，0 = 已到达目标
 */
uint8_t HugoUI_Animation_EasyIn(float *a, float *a_trg, uint16_t n);

#endif /* __UI_OLED_H__ */
