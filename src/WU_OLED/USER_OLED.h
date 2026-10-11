/*------------------------------------------------------------------------------
 * 文件名称 : USER_OLED.h
 * 所属模块 : STM32_U8g2_Wrapper - 个人封装层
 * 功能说明 : u8g2 图形库的用户封装层头文件。
 *            对官方 u8g2 API 做了一层带 WU_OLED_U8G2_ 前缀的二次封装，
 *            并额外提供 Printf、数值显示、屏保、位图缩放等实用函数，
 *            让上层业务代码调用更简洁、统一。
 *
 * 编码要求 : 本文件（及所有 .c/.h 源码）必须以 UTF-8 编码保存。
 *            中文文本经 WU_OLED_U8G2_DrawUTF8() 显示时按 UTF-8 解析，
 *            配合 u8g2_font_wqy13_t_gb2312 系列中文字库使用。
 *            （Keil MDK 中请将文件编码设为 "UTF-8 with BOM"。）
 *
 * 依赖说明 : 需要移植层提供全局 u8g2 结构体实例（见下方 extern 声明），
 *            并实现 OLED_IoInit() / WU_u8g2Init() 等底层接口。
 *
 * 版本记录 : V2.0  2026-10-11  由原 WUYDD-U8g2 工程整理开源
 *            统一两平台（Keil SPL / STM32CubeIDE HAL）封装；
 *            屏保等函数统一为 WU_OLED_ 前缀命名。
 *------------------------------------------------------------------------------
 */
#ifndef __USER_OLED_H__
#define __USER_OLED_H__

#include "u8g2.h"           /* 官方 u8g2 库：类型、函数、字体声明 */

/*------------------------------------------------------------------------------
 * 全局 u8g2 结构体（在移植层 WU_U8g2_Init.c 中定义）
 * 例如：u8g2_t u8g2;
 *------------------------------------------------------------------------------
 */
extern u8g2_t u8g2;

/*------------------------------------------------------------------------------
 * 屏幕 / 字体基础参数（可按实际屏幕尺寸修改）
 *------------------------------------------------------------------------------
 */
#define SCREEN_WIDTH     128    /* 屏幕宽度（像素）        */
#define SCREEN_HEIGHT     64    /* 屏幕高度（像素）        */
#define FONT_WIDTH         8    /* 常用字体的字符宽度      */
#define FONT_HEIGHT       15    /* 常用字体的字符高度      */
#define SCROLL_BAR_WIDTH   3    /* 滚动条宽度              */

/*------------------------------------------------------------------------------
 * WU_OLED_U8G2_Printf() 的字体档位选择（FontSize 参数取值）
 *------------------------------------------------------------------------------
 */
#define DRAWSTR                    1   /* 普通字体绘制（DrawStr）         */
#define DRAWSTRX2                  2   /* 2 倍放大字体绘制（DrawStrX2）   */
#define CHINA                      3   /* 中文：wqy13 GB2312 字体（13px） */
#define CHINAX2                    4   /* 中文 2 倍放大                  */
#define CHINA_UNIFONT_T_SYMBOLS    5   /* 符号字库 unifont symbols        */
#define CHINA_UNIFONT_T_SYMBOLSX2  6   /* 符号字库 2 倍放大               */

/*------------------------------------------------------------------------------
 * 基本函数（缓冲区操作 / 颜色）
 *------------------------------------------------------------------------------
 */
void   WU_OLED_U8G2_ClearBuffer(void);                    /* 清空发送缓冲区                */
void   WU_OLED_U8G2_SendBuffer(void);                     /* 刷新发送缓冲区到屏幕          */
void   WU_OLED_U8G2_SetDrawColor(uint8_t mode);           /* 设置绘图颜色模式（0/1/2）     */

/*------------------------------------------------------------------------------
 * 绘制相关函数（点 / 线 / 面 / 图形 / 文本）
 *------------------------------------------------------------------------------
 */
void        WU_OLED_U8G2_DrawPoint(uint16_t x, uint16_t y);                    /* 绘制点          */
void        WU_OLED_U8G2_DrawHLine(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w);       /* 水平线    */
void        WU_OLED_U8G2_DrawVLine(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len);     /* 竖直线    */
void        WU_OLED_U8G2_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1); /* 两点连线  */
void        WU_OLED_U8G2_DrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height);            /* 实心矩形      */
void        WU_OLED_U8G2_DrawFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height);          /* 空心矩形      */
void        WU_OLED_U8G2_DrawCircle(u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rad, uint8_t opt);    /* 空心圆        */
void        WU_OLED_U8G2_DrawDisc(u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rad, uint8_t opt);      /* 实心圆        */
void        WU_OLED_U8G2_DrawEllipse(u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rx, u8g2_uint_t ry, uint8_t opt);      /* 空心椭圆   */
void        WU_OLED_U8G2_DrawFilledEllipse(u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rx, u8g2_uint_t ry, uint8_t opt); /* 实心椭圆 */
void        WU_OLED_U8G2_DrawRFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r); /* 空心圆角矩形 */
void        WU_OLED_U8G2_DrawRBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r);   /* 实心圆角矩形 */
void        WU_OLED_U8G2_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2); /* 实心三角形 */
void        WU_OLED_U8G2_ShowBMP(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pic);  /* 绘制位图(XBMP) */
void        WU_OLED_U8G2_DrawButtonUTF8(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t flags, u8g2_uint_t width,
                                        u8g2_uint_t padding_h, u8g2_uint_t padding_v, const char *text);      /* 给文本加外框   */
u8g2_uint_t WU_OLED_U8G2_DrawGlyph(u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding);     /* 绘制字形       */
u8g2_uint_t WU_OLED_U8G2_DrawGlyphX2(u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding);   /* 绘制字形 X2    */
u8g2_uint_t WU_OLED_U8G2_DrawStr(u8g2_uint_t x, u8g2_uint_t y, const char *s);           /* 绘制字符串     */
u8g2_uint_t WU_OLED_U8G2_DrawStrX2(u8g2_uint_t x, u8g2_uint_t y, const char *s);         /* 绘制字符串 X2  */
u8g2_uint_t WU_OLED_U8G2_DrawUTF8(u8g2_uint_t x, u8g2_uint_t y, const char *s);          /* 绘制 UTF-8 字符串（中文用） */
u8g2_uint_t WU_OLED_U8G2_DrawUTF8X2(u8g2_uint_t x, u8g2_uint_t y, const char *s);        /* 绘制 UTF-8 字符串 X2 */

/*------------------------------------------------------------------------------
 * 显示配置相关函数
 *------------------------------------------------------------------------------
 */
void        WU_OLED_U8G2_SetOledInverseColor(uint8_t flag);      /* OLED 整体反色（SSD1306 A6/A7 命令）  */
void        WU_OLED_U8G2_SetBitmapMode(uint8_t mode);            /* 位图透明模式开关                    */
void        WU_OLED_U8G2_SetClipWindow(u8g2_uint_t clip_x0, u8g2_uint_t clip_y0,
                                       u8g2_uint_t clip_x1, u8g2_uint_t clip_y1);  /* 裁剪显示区域      */
void        WU_OLED_U8G2_SetDisplayRotation(const u8g2_cb_t *u8g2_cb);            /* 更改显示旋转      */
void        WU_OLED_U8G2_SetFontDirection(uint8_t dir);          /* 设置字体书写方向   */
void        WU_OLED_U8G2_SetFlipMode(uint8_t mode);              /* 硬件 180 度翻转    */
void        WU_OLED_U8G2_SetFont(const uint8_t *font);           /* 选择字库            */
int8_t      WU_OLED_U8G2_GetAscent(u8g2_t *u8g);                 /* 基准线以上高度      */
int8_t      WU_OLED_U8G2_GetDescent(u8g2_t *u8g2);               /* 基准线以下高度      */
u8g2_uint_t WU_OLED_U8G2_GetDisplayHeight(void);                 /* 屏幕高度            */
u8g2_uint_t WU_OLED_U8G2_GetDisplayWidth(void);                  /* 屏幕宽度            */
u8g2_uint_t WU_OLED_U8G2_GetMaxCharHeight(void);                 /* 当前字体最大字符高度 */
u8g2_uint_t WU_OLED_U8G2_GetMaxCharWidth(void);                  /* 当前字体最大字符宽度 */
u8g2_uint_t WU_OLED_U8G2_GetStrWidth(const char *s);             /* 字符串像素宽度       */
u8g2_uint_t WU_OLED_U8G2_GetUTF8Width(const char *str);          /* UTF-8 字符串像素宽度 */
uint8_t     WU_OLED_U8G2_SetAutoPageClear(uint8_t mode);         /* 自动清屏开关         */
uint8_t    *WU_OLED_U8G2_GetBufferPtr(void);                     /* 获取缓冲区地址       */
uint16_t    WU_OLED_U8G2_GetBufferTileHeight(void);              /* 缓冲区 Tile 高度    */
uint16_t    WU_OLED_U8G2_GetBufferTileWidth(void);               /* 缓冲区 Tile 宽度    */
uint8_t     WU_OLED_U8G2_GetBufferCurrTileRow(void);             /* 当前 Tile 行        */
void        WU_OLED_U8G2_SetBufferCurrTileRow(uint8_t row);      /* 设置当前 Tile 行    */

/*------------------------------------------------------------------------------
 * 用户自定义实用函数
 *------------------------------------------------------------------------------
 */
void WU_OLED_U8G2_ShowNum(u8g2_uint_t x, u8g2_uint_t y, short dat);             /* 显示整数            */
void WU_OLED_U8G2_ShowFloat(u8g2_uint_t x, u8g2_uint_t y, float dat,
                            u8g2_uint_t IntLength, u8g2_uint_t FraLength);       /* 显示浮点数          */
void WU_OLED_U8G2_Printf(uint8_t X, uint8_t Y, uint8_t FontSize,
                         const char *format, ...);                               /* OLED 格式化打印     */
void WU_OLED_RandomLines(void);                                                  /* 屏保：随机线条      */
void WU_OLED_Noise(void);                                                        /* 屏保：模拟噪点      */
void WU_OLED_DrawIntensiveComputing(void);                                       /* 屏保：密集运算噪点  */
void WU_OLED_DrawSlowBitmapResize(int x, int y, const uint8_t *bitmap,
                                  int w1, int h1, int w2, int h2);               /* 位图缩放绘制        */

#endif /* __USER_OLED_H__ */
