/*------------------------------------------------------------------------------
 * 文件名称 : WU_OLED_Bitmap.h
 * 所属模块 : STM32_U8g2_Wrapper - 个人封装层
 * 功能说明 : 内置位图资源声明（点赞图标、诗人、机器人、刀具、空心爱心）。
 *            位图为单色 XBMP 格式，可用 WU_OLED_U8G2_ShowBMP() 绘制。
 *            数据定义见 WU_OLED_Bitmap.c。
 *
 * 编码要求 : UTF-8 编码保存。
 *------------------------------------------------------------------------------
 */
#ifndef __WU_OLED_BITMAP_H__
#define __WU_OLED_BITMAP_H__

#include "u8g2.h"    /* 提供 U8X8_PROGMEM 与 uint8_t 类型 */

/* 点赞（Thumbs-up）图标，25 × 25 像素（100 字节 XBMP） */
extern const uint8_t u8g_logo_bits[] U8X8_PROGMEM;

/* 诗人头像图标，32 × 32 像素 */
extern const uint8_t Poet_BMP[] U8X8_PROGMEM;

/* 机器人图标，32 × 32 像素 */
extern const uint8_t Robot_BMP[] U8X8_PROGMEM;

/* 刀具图标，32 × 32 像素 */
extern const uint8_t Knife_BMP[] U8X8_PROGMEM;

/* 空心爱心图标，32 × 32 像素 */
extern const uint8_t HollowHeart_BMP[] U8X8_PROGMEM;

#endif /* __WU_OLED_BITMAP_H__ */
