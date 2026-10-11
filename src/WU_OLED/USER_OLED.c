/*------------------------------------------------------------------------------
 * 文件名称 : USER_OLED.c
 * 所属模块 : STM32_U8g2_Wrapper - 个人封装层
 * 功能说明 : u8g2 图形库的用户封装层。
 *            1) 将官方 u8g2 API 统一封装为 WU_OLED_U8G2_ 前缀函数，
 *               上层业务代码只需要操作一个全局 u8g2 结构体；
 *            2) 提供 ShowNum / ShowFloat / Printf 等数值显示工具；
 *            3) 提供屏保、位图缩放等娱乐性功能。
 *
 * 编码要求 : 必须以 UTF-8 编码保存（Keil MDK 使用 UTF-8 with BOM）。
 *
 * 使用注意 : 全局变量 Time_ms 需要用户自行维护：
 *            在 SysTick / 定时器中断中执行 Time_ms++（或 += 周期毫秒数），
 *            屏保类函数的动态效果依赖它。
 *
 * 版本记录 : V2.0  2026-10-11  由原 WUYDD-U8g2 工程整理开源
 *            合并 Keil 与 STM32CubeIDE 两版本差异（补齐 Printf 等函数）；
 *            屏保/缩放函数统一为 WU_OLED_ 前缀命名。
 *------------------------------------------------------------------------------
 */
#include "USER_OLED.h"      /* 本模块头文件（含全局 u8g2 结构体声明） */
#include "u8g2.h"           /* 官方 u8g2 库 */
#include "stdlib.h"         /* rand() 等 */
#include "string.h"         /* 字符串操作 */
#include "stdio.h"          /* sprintf() 等 */
#include "stdarg.h"         /* va_list 可变参数支持 */
#include "math.h"           /* sinf() / ceil() 等 */

/* 毫秒计数器：动态效果的全局时钟，需由用户在定时器中断中自增 */
uint32_t Time_ms = 0;

/*------------------------------------------------------------------------------
 *                                基本函数
 *------------------------------------------------------------------------------
 */

/**
 * @brief  清空内部像素缓冲区（不会立即刷新到屏幕）
 * @note   调用本函数后需配合 WU_OLED_U8G2_SendBuffer() 刷新显示
 */
void WU_OLED_U8G2_ClearBuffer(void)
{
    u8g2_ClearBuffer(&u8g2);
}

/**
 * @brief  将内部像素缓冲区内容刷新到屏幕
 */
void WU_OLED_U8G2_SendBuffer(void)
{
    u8g2_SendBuffer(&u8g2);
}

/**
 * @brief  设置绘图颜色模式
 * @param  mode  颜色模式：
 *               0 -> 透显（黑色，不改变底色）
 *               1 -> 实显（白色，正常绘制）
 *               2 -> XOR 模式（反色，与底色异或）
 * @note   对某个对象做反色处理后，记得把模式设置回 1
 */
void WU_OLED_U8G2_SetDrawColor(uint8_t mode)
{
    u8g2_SetDrawColor(&u8g2, mode);
}

/*------------------------------------------------------------------------------
 *                              绘制相关函数
 *------------------------------------------------------------------------------
 */

/**
 * @brief  绘制点
 * @param  x  横坐标
 * @param  y  纵坐标
 * @note   超出屏幕范围的点会被跳过（边界保护）
 */
void WU_OLED_U8G2_DrawPoint(uint16_t x, uint16_t y)
{
    /* 超出屏幕 2 倍范围，直接退出（边界保护） */
    if (x > SCREEN_WIDTH * 2 || y > SCREEN_HEIGHT * 2)
        return;
    u8g2_DrawPixel(&u8g2, x, y);
}

/**
 * @brief  绘制水平线
 * @param  x  起点横坐标
 * @param  y  起点纵坐标
 * @param  w  水平线长度（像素）
 * @example WU_OLED_U8G2_DrawHLine(12, 12, 34);
 */
void WU_OLED_U8G2_DrawHLine(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w)
{
    u8g2_DrawHLine(&u8g2, x, y, w);
}

/**
 * @brief  绘制竖直线
 * @param  x    起点横坐标
 * @param  y    起点纵坐标
 * @param  len  竖直线长度（像素）
 * @example WU_OLED_U8G2_DrawVLine(12, 12, 34);
 */
void WU_OLED_U8G2_DrawVLine(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len)
{
    u8g2_DrawVLine(&u8g2, x, y, len);
}

/**
 * @brief  在两点之间绘制线段
 * @param  x0  起点横坐标
 * @param  y0  起点纵坐标
 * @param  x1  终点横坐标
 * @param  y1  终点纵坐标
 * @example WU_OLED_U8G2_DrawLine(127, 0, 0, 63);
 */
void WU_OLED_U8G2_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    /* 超出屏幕 2 倍范围，直接退出（边界保护） */
    if (x0 > SCREEN_WIDTH * 2 || y0 > SCREEN_HEIGHT * 2 ||
        x1 > SCREEN_WIDTH * 2 || y1 > SCREEN_HEIGHT * 2)
        return;
    u8g2_DrawLine(&u8g2, x0, y0, x1, y1);
}

/**
 * @brief  绘制实心矩形
 * @param  x       左上角横坐标
 * @param  y       左上角纵坐标
 * @param  width   矩形宽度
 * @param  height  矩形高度
 * @example WU_OLED_U8G2_DrawBox(30, 0, 30, 30);
 */
void WU_OLED_U8G2_DrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    u8g2_DrawBox(&u8g2, x, y, width, height);
}

/**
 * @brief  绘制空心矩形（边框）
 * @param  x       左上角横坐标
 * @param  y       左上角纵坐标
 * @param  width   矩形宽度
 * @param  height  矩形高度
 * @example WU_OLED_U8G2_DrawFrame(30, 0, 30, 30);
 */
void WU_OLED_U8G2_DrawFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    u8g2_DrawFrame(&u8g2, x, y, width, height);
}

/**
 * @brief  绘制空心圆
 * @param  x0   圆心横坐标
 * @param  y0   圆心纵坐标
 * @param  rad  半径
 * @param  opt  选择绘制圆的哪一部分，多个配置可用 | 连接：
 *              U8G2_DRAW_UPPER_RIGHT  右上圆弧
 *              U8G2_DRAW_UPPER_LEFT   左上圆弧
 *              U8G2_DRAW_LOWER_LEFT   左下圆弧
 *              U8G2_DRAW_LOWER_RIGHT  右下圆弧
 *              U8G2_DRAW_ALL          整个圆
 * @example WU_OLED_U8G2_DrawCircle(10, 35, 10, U8G2_DRAW_ALL);
 */
void WU_OLED_U8G2_DrawCircle(u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rad, uint8_t opt)
{
    u8g2_DrawCircle(&u8g2, x0, y0, rad, opt);
}

/**
 * @brief  绘制实心圆
 * @param  x0   圆心横坐标
 * @param  y0   圆心纵坐标
 * @param  rad  半径
 * @param  opt  选择绘制圆的哪一部分（取值同 DrawCircle）
 * @example WU_OLED_U8G2_DrawDisc(10, 35, 10, U8G2_DRAW_ALL);
 */
void WU_OLED_U8G2_DrawDisc(u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rad, uint8_t opt)
{
    u8g2_DrawDisc(&u8g2, x0, y0, rad, opt);
}

/**
 * @brief  绘制空心椭圆
 * @param  x0   椭圆中心横坐标
 * @param  y0   椭圆中心纵坐标
 * @param  rx   长轴半径
 * @param  ry   短轴半径
 * @param  opt  选择绘制哪一部分（取值同 DrawCircle）
 * @example WU_OLED_U8G2_DrawEllipse(20, 25, 15, 10, U8G2_DRAW_ALL);
 */
void WU_OLED_U8G2_DrawEllipse(u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rx, u8g2_uint_t ry, uint8_t opt)
{
    u8g2_DrawEllipse(&u8g2, x0, y0, rx, ry, opt);
}

/**
 * @brief  绘制实心椭圆
 * @param  x0   椭圆中心横坐标
 * @param  y0   椭圆中心纵坐标
 * @param  rx   长轴半径
 * @param  ry   短轴半径
 * @param  opt  选择绘制哪一部分（取值同 DrawCircle）
 * @example WU_OLED_U8G2_DrawFilledEllipse(20, 25, 15, 10, U8G2_DRAW_ALL);
 */
void WU_OLED_U8G2_DrawFilledEllipse(u8g2_uint_t x0, u8g2_uint_t y0, u8g2_uint_t rx, u8g2_uint_t ry, uint8_t opt)
{
    u8g2_DrawFilledEllipse(&u8g2, x0, y0, rx, ry, opt);
}

/**
 * @brief  绘制空心圆角矩形
 * @param  x       左上角横坐标
 * @param  y       左上角纵坐标
 * @param  width   矩形宽度
 * @param  height  矩形高度
 * @param  r       圆角半径
 * @example WU_OLED_U8G2_DrawRFrame(30, 0, 30, 30, 12);
 */
void WU_OLED_U8G2_DrawRFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r)
{
    u8g2_DrawRFrame(&u8g2, x, y, width, height, r);
}

/**
 * @brief  绘制实心圆角矩形
 * @param  x       左上角横坐标
 * @param  y       左上角纵坐标
 * @param  width   矩形宽度
 * @param  height  矩形高度
 * @param  r       圆角半径
 * @example WU_OLED_U8G2_DrawRBox(30, 0, 30, 30, 12);
 */
void WU_OLED_U8G2_DrawRBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r)
{
    u8g2_DrawRBox(&u8g2, x, y, width, height, r);
}

/**
 * @brief  绘制实心三角形
 * @param  x0  顶点 1 横坐标
 * @param  y0  顶点 1 纵坐标
 * @param  x1  顶点 2 横坐标
 * @param  y1  顶点 2 纵坐标
 * @param  x2  顶点 3 横坐标
 * @param  y2  顶点 3 纵坐标
 * @example WU_OLED_U8G2_DrawTriangle(20, 5, 27, 50, 5, 32);
 */
void WU_OLED_U8G2_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    u8g2_DrawTriangle(&u8g2, x0, y0, x1, y1, x2, y2);
}

/**
 * @brief  绘制位图（XBMP 格式，单色 1bit/像素）
 * @param  x       左上角横坐标
 * @param  y       左上角纵坐标
 * @param  width   位图宽度（像素）
 * @param  height  位图高度（像素）
 * @param  pic     位图数据指针（可用 PCtoLCD / 图片取模工具生成）
 * @example WU_OLED_U8G2_ShowBMP(30, 20, 25, 25, u8g_logo_bits);   // 内置点赞图标
 * @note   内置位图资源见 WU_OLED_Bitmap.c/h（点赞 25x25、其余图标 32x32）
 */
void WU_OLED_U8G2_ShowBMP(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pic)
{
    u8g2_DrawXBMP(&u8g2, x, y, width, height, pic);
}

/**
 * @brief  给文本加外框（按钮效果）
 * @param  x         横坐标（左下角基准）
 * @param  y         纵坐标（左下角基准）
 * @param  flags     边框配置，多个可用 | 连接：
 *                   U8G2_BTN_BW0/BW1/BW2/BW3  边框宽度 0~3 像素
 *                   U8G2_BTN_SHADOW0/1/2      阴影及间隙
 *                   U8G2_BTN_INV              文本反色
 *                   U8G2_BTN_HCENTER          文本在框内水平居中
 *                   U8G2_BTN_XFRAME           外圈再绘制 1 像素边框
 * @param  width     边框宽度，0 或负值表示自适应文本宽度
 * @param  padding_h 文本与边框的水平间距
 * @param  padding_v 文本与边框的垂直间距
 * @param  text      要显示的文本
 * @note   该函数会将字体模式设置为透明模式（1）
 * @example
 *   WU_OLED_U8G2_SetFont(u8g2_font_helvR08_tr);
 *   WU_OLED_U8G2_DrawButtonUTF8(62, 20, U8G2_BTN_SHADOW1|U8G2_BTN_HCENTER|U8G2_BTN_BW2, 0, 2, 2, "WU-Btn");
 */
void WU_OLED_U8G2_DrawButtonUTF8(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t flags,
                                 u8g2_uint_t width, u8g2_uint_t padding_h, u8g2_uint_t padding_v, const char *text)
{
    u8g2_DrawButtonUTF8(&u8g2, x, y, flags, width, padding_h, padding_v, text);
}

/**
 * @brief  绘制字形（单字符，按编码索引）
 * @param  x         左下角横坐标
 * @param  y         左下角纵坐标
 * @param  encoding  字形编码（Unicode，如 0x2603 雪人符号）
 * @return 字符宽度
 * @example
 *   WU_OLED_U8G2_SetFont(u8g2_font_unifont_t_symbols); // 选择符号字库
 *   WU_OLED_U8G2_DrawGlyph(70, 45, 0x2603);
 */
u8g2_uint_t WU_OLED_U8G2_DrawGlyph(u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding)
{
    return u8g2_DrawGlyph(&u8g2, x, y, encoding);
}

/**
 * @brief  绘制字形 X2（2 倍放大）
 * @param  x         左下角横坐标
 * @param  y         左下角纵坐标
 * @param  encoding  字形编码（Unicode）
 * @return 字符宽度
 * @example
 *   WU_OLED_U8G2_SetFont(u8g2_font_unifont_t_symbols);
 *   WU_OLED_U8G2_DrawGlyphX2(70, 45, 0x2603);
 */
u8g2_uint_t WU_OLED_U8G2_DrawGlyphX2(u8g2_uint_t x, u8g2_uint_t y, uint16_t encoding)
{
    return u8g2_DrawGlyphX2(&u8g2, x, y, encoding);
}

/**
 * @brief  绘制字符串（ASCII）
 * @param  x  左下角起点横坐标
 * @param  y  左下角起点纵坐标
 * @param  s  字符串文本
 * @return 字符串像素长度
 * @example
 *   WU_OLED_U8G2_SetFont(u8g2_font_ncenB14_tr);
 *   WU_OLED_U8G2_DrawStr(0, 30, "Hello World!");
 */
u8g2_uint_t WU_OLED_U8G2_DrawStr(u8g2_uint_t x, u8g2_uint_t y, const char *s)
{
    return u8g2_DrawStr(&u8g2, x, y, s);
}

/**
 * @brief  绘制字符串 X2（2 倍放大，ASCII）
 * @param  x  左下角起点横坐标
 * @param  y  左下角起点纵坐标
 * @param  s  字符串文本
 * @return 字符串像素长度
 */
u8g2_uint_t WU_OLED_U8G2_DrawStrX2(u8g2_uint_t x, u8g2_uint_t y, const char *s)
{
    return u8g2_DrawStrX2(&u8g2, x, y, s);
}

/**
 * @brief  绘制 UTF-8 编码字符串（显示中文请用本函数）
 * @param  x  左下角起点横坐标
 * @param  y  左下角起点纵坐标
 * @param  s  UTF-8 编码文本（源码文件需以 UTF-8 保存）
 * @return 字符串像素长度
 * @note   需配合中文字库使用，例如 u8g2_font_wqy13_t_gb2312a；
 *         纯 ASCII 文本也可用本函数绘制
 * @example
 *   WU_OLED_U8G2_SetFont(u8g2_font_wqy13_t_gb2312a);
 *   WU_OLED_U8G2_DrawUTF8(5, 20, "你好，世界！");
 */
u8g2_uint_t WU_OLED_U8G2_DrawUTF8(u8g2_uint_t x, u8g2_uint_t y, const char *s)
{
    return u8g2_DrawUTF8(&u8g2, x, y, s);
}

/**
 * @brief  绘制 UTF-8 编码字符串 X2（2 倍放大）
 * @param  x  左下角起点横坐标
 * @param  y  左下角起点纵坐标
 * @param  s  UTF-8 编码文本
 * @return 字符串像素长度
 */
u8g2_uint_t WU_OLED_U8G2_DrawUTF8X2(u8g2_uint_t x, u8g2_uint_t y, const char *s)
{
    return u8g2_DrawUTF8X2(&u8g2, x, y, s);
}

/*------------------------------------------------------------------------------
 *                              显示配置相关函数
 *------------------------------------------------------------------------------
 */

/**
 * @brief  设置位图透明模式
 * @param  mode  1 = 透明模式（不写入背景色），0 = 实心模式（默认）
 */
void WU_OLED_U8G2_SetBitmapMode(uint8_t mode)
{
    u8g2_SetBitmapMode(&u8g2, mode);
}

/**
 * @brief  启用/禁用显示内容硬件 180 度翻转
 * @param  mode  1 = 翻转，0 = 正常
 * @note   部分屏幕支持内部帧缓冲翻转。切换后请先清屏再重新绘制，
 *        否则原有内容显示结果不确定
 */
void WU_OLED_U8G2_SetFlipMode(uint8_t mode)
{
    u8g2_SetFlipMode(&u8g2, mode);
}

/**
 * @brief  获取当前字体基准线以上的高度（ascent）
 * @param  u8g  u8g2 结构体指针（一般传 &u8g2）
 * @return 基准线以上高度（像素）
 */
int8_t WU_OLED_U8G2_GetAscent(u8g2_t *u8g)
{
    return u8g2_GetAscent(u8g);
}

/**
 * @brief  获取当前字体基准线以下的高度（descent）
 * @param  u8g2  u8g2 结构体指针（一般传 &u8g2）
 * @return 基准线以下高度（像素）
 */
int8_t WU_OLED_U8G2_GetDescent(u8g2_t *u8g2)
{
    return u8g2_GetDescent(u8g2);
}

/**
 * @brief  获取显示器高度
 * @return 屏幕高度（像素）
 */
u8g2_uint_t WU_OLED_U8G2_GetDisplayHeight(void)
{
    return u8g2_GetDisplayHeight(&u8g2);
}

/**
 * @brief  获取显示器宽度
 * @return 屏幕宽度（像素）
 */
u8g2_uint_t WU_OLED_U8G2_GetDisplayWidth(void)
{
    return u8g2_GetDisplayWidth(&u8g2);
}

/**
 * @brief  获取当前字体最大字符高度
 * @return 最大字符高度（像素）
 */
u8g2_uint_t WU_OLED_U8G2_GetMaxCharHeight(void)
{
    return u8g2_GetMaxCharHeight(&u8g2);
}

/**
 * @brief  获取当前字体最大字符宽度
 * @return 最大字符宽度（像素）
 */
u8g2_uint_t WU_OLED_U8G2_GetMaxCharWidth(void)
{
    return u8g2_GetMaxCharWidth(&u8g2);
}

/**
 * @brief  获取字符串像素宽度（ASCII）
 * @param  s  字符串文本
 * @return 字符串像素宽度
 */
u8g2_uint_t WU_OLED_U8G2_GetStrWidth(const char *s)
{
    return u8g2_GetStrWidth(&u8g2, s);
}

/**
 * @brief  获取 UTF-8 字符串像素宽度（中文居中等场景常用）
 * @param  str  UTF-8 编码文本
 * @return 字符串像素宽度
 * @example
 *   x = (128 - WU_OLED_U8G2_GetUTF8Width("居中文本")) / 2;  // 水平居中
 */
u8g2_uint_t WU_OLED_U8G2_GetUTF8Width(const char *str)
{
    return u8g2_GetUTF8Width(&u8g2, str);
}

/**
 * @brief  设置自动清除缓冲区
 * @param  mode  0 = 关闭自动清屏，1 = 开启（默认）
 * @return 缓冲区 Tile 宽度
 * @note   开启后每次 SendBuffer 前自动清除缓冲区；关闭则需手动 ClearBuffer
 */
uint8_t WU_OLED_U8G2_SetAutoPageClear(uint8_t mode)
{
    return u8g2_SetAutoPageClear(&u8g2, mode);
}

/**
 * @brief  限制所有图形输出到指定裁剪窗口
 * @param  clip_x0  可见区域左边缘
 * @param  clip_y0  可见区域上边缘
 * @param  clip_x1  可见区域右边缘 +1
 * @param  clip_y1  可见区域下边缘 +1
 */
void WU_OLED_U8G2_SetClipWindow(u8g2_uint_t clip_x0, u8g2_uint_t clip_y0,
                                u8g2_uint_t clip_x1, u8g2_uint_t clip_y1)
{
    u8g2_SetClipWindow(&u8g2, clip_x0, clip_y0, clip_x1, clip_y1);
}

/**
 * @brief  更改显示旋转方向
 * @param  u8g2_cb  旋转参数：
 *                  U8G2_R0  不旋转，横向
 *                  U8G2_R1  顺时针旋转 90°
 *                  U8G2_R2  顺时针旋转 180°
 *                  U8G2_R3  顺时针旋转 270°
 *                  U8G2_MIRROR          水平镜像
 *                  U8G2_MIRROR_VERTICAL 垂直镜像
 * @note   调用后需要重新绘制全部内容
 */
void WU_OLED_U8G2_SetDisplayRotation(const u8g2_cb_t *u8g2_cb)
{
    u8g2_SetDisplayRotation(&u8g2, u8g2_cb);
}

/**
 * @brief  设置字体书写方向
 * @param  dir  0 = 0°  从左到右
 *              1 = 90° 从上到下
 *              2 = 180° 从右到左
 *              3 = 270° 从下到上
 */
void WU_OLED_U8G2_SetFontDirection(uint8_t dir)
{
    u8g2_SetFontDirection(&u8g2, dir);
}

/**
 * @brief  获取内部像素缓冲区地址（用于直接操作缓冲区，如实现特效）
 * @return 缓冲区首地址指针
 */
uint8_t *WU_OLED_U8G2_GetBufferPtr(void)
{
    return u8g2_GetBufferPtr(&u8g2);
}

/**
 * @brief  获取缓冲区 Tile 高度（1 Tile = 8 像素行）
 * @return Tile 高度
 */
uint16_t WU_OLED_U8G2_GetBufferTileHeight(void)
{
    return u8g2_GetBufferTileHeight(&u8g2);
}

/**
 * @brief  获取缓冲区 Tile 宽度（1 Tile = 8 像素行）
 * @return Tile 宽度
 */
uint16_t WU_OLED_U8G2_GetBufferTileWidth(void)
{
    return u8g2_GetBufferTileWidth(&u8g2);
}

/**
 * @brief  获取缓冲区当前 Tile 行（页面位置，1 Tile = 8 像素行）
 * @return 当前 Tile 行号
 */
uint8_t WU_OLED_U8G2_GetBufferCurrTileRow(void)
{
    return u8g2_GetBufferCurrTileRow(&u8g2);
}

/**
 * @brief  设置缓冲区当前 Tile 行
 * @param  row  Tile 行号（像素位置 = row × 8）
 */
void WU_OLED_U8G2_SetBufferCurrTileRow(uint8_t row)
{
    u8g2_SetBufferCurrTileRow(&u8g2, row);
}

/**
 * @brief  OLED 整体反色开关（SSD1306 A6/A7 命令）
 * @param  flag  1 = 反白显示（0xA7），0 = 正常显示（0xA6）
 * @note   底层依赖 u8g2_SendF()，该函数在 u8g2 内部实现
 */
void WU_OLED_U8G2_SetOledInverseColor(uint8_t flag)
{
    if (flag)
    {
        u8g2_SendF(&u8g2, "c", 0xA7);   /* 反白 */
    }
    else
    {
        u8g2_SendF(&u8g2, "c", 0xA6);   /* 反黑（正常） */
    }
}

/**
 * @brief  选择字库（字体）
 * @param  font  字体定义指针，如：
 *               u8g2_font_ncenB14_tr           英文粗体 14px
 *               u8g2_font_wqy13_t_gb2312a      中文 13px（GB2312 字符集）
 *               u8g2_font_unifont_t_symbols    符号字库
 * @note   常用字体在 u8g2.h 中有声明；u8x8 字体不可用于 u8g2 模式
 */
void WU_OLED_U8G2_SetFont(const uint8_t *font)
{
    u8g2_SetFont(&u8g2, font);
}

/*------------------------------------------------------------------------------
 *                              用户自定义实用函数
 *------------------------------------------------------------------------------
 */

/**
 * @brief  显示整数（short 型）
 * @param  x    横坐标（左下角基准）
 * @param  y    纵坐标（左下角基准）
 * @param  dat  要显示的整数数据
 * @example WU_OLED_U8G2_ShowNum(0, 40, 23);
 * @note   使用当前字体绘制
 */
void WU_OLED_U8G2_ShowNum(u8g2_uint_t x, u8g2_uint_t y, short dat)
{
    char buff[10];
    sprintf(buff, "%d", dat);
    WU_OLED_U8G2_DrawStr(x, y, buff);
}

/**
 * @brief  OLED 格式化打印（printf 风格，支持中文）
 * @param  X         横坐标（左下角基准）
 * @param  Y         纵坐标（左下角基准）
 * @param  FontSize  字体档位，取值：
 *                   DRAWSTR                 普通字体
 *                   DRAWSTRX2               2 倍放大
 *                   CHINA                   中文 13px（wqy gb2312a）
 *                   CHINAX2                 中文 2 倍放大
 *                   CHINA_UNIFONT_T_SYMBOLS 符号字库
 *                   CHINA_UNIFONT_T_SYMBOLSX2 符号字库 2 倍
 * @param  format    格式化字符串（ASCII 可见字符）
 * @param  ...       可变参数
 * @example WU_OLED_U8G2_Printf(0, 30, CHINA, "温度:%d℃", Temperature);
 * @note   内部缓冲区 100 字节，超长字符串可能截断或溢出，请自行按需调整
 */
void WU_OLED_U8G2_Printf(uint8_t X, uint8_t Y, uint8_t FontSize, const char *format, ...)
{
    char String[100];               /* 格式化结果缓冲区 */
    va_list arg;                    /* 可变参数列表 */
    va_start(arg, format);          /* 从 format 之后开始取参数 */
    vsprintf(String, format, arg);  /* 格式化到字符数组 */
    va_end(arg);                    /* 结束可变参数处理 */

    switch (FontSize)
    {
        case DRAWSTR:                       /* 普通字体 */
            WU_OLED_U8G2_DrawStr(X, Y, String);
            break;
        case DRAWSTRX2:                     /* 2 倍放大 */
            WU_OLED_U8G2_DrawStrX2(X, Y, String);
            break;
        case CHINA:                         /* 中文 13px */
            WU_OLED_U8G2_SetFont(u8g2_font_wqy13_t_gb2312a);
            WU_OLED_U8G2_DrawUTF8(X, Y, String);
            break;
        case CHINAX2:                       /* 中文 2 倍 */
            WU_OLED_U8G2_SetFont(u8g2_font_wqy13_t_gb2312a);
            WU_OLED_U8G2_DrawUTF8X2(X, Y, String);
            break;
        case CHINA_UNIFONT_T_SYMBOLS:       /* 符号字库 */
            WU_OLED_U8G2_SetFont(u8g2_font_unifont_t_symbols);
            WU_OLED_U8G2_DrawUTF8(X, Y, String);
            break;
        case CHINA_UNIFONT_T_SYMBOLSX2:     /* 符号字库 2 倍 */
            WU_OLED_U8G2_SetFont(u8g2_font_unifont_t_symbols);
            WU_OLED_U8G2_DrawUTF8X2(X, Y, String);
            break;
        default:
            break;
    }
}

/**
 * @brief  显示浮点数（可指定整数/小数位数）
 * @param  x         横坐标（左下角基准）
 * @param  y         纵坐标（左下角基准）
 * @param  dat       要显示的浮点数据
 * @param  IntLength 小数点前的整数位数
 * @param  FraLength 小数点后的小数位数
 * @example WU_OLED_U8G2_ShowFloat(0, 60, 296.65, 3, 2);  // 显示 296.65
 * @note   使用当前字体绘制
 */
void WU_OLED_U8G2_ShowFloat(u8g2_uint_t x, u8g2_uint_t y, float dat,
                            u8g2_uint_t IntLength, u8g2_uint_t FraLength)
{
    char formatControl[16];     /* 动态生成的格式控制串，如 "%6.2f" */
    char NumForShow[32];        /* 格式化后的数字文本 */

    /* %m.nf 中的 m 指整个浮点数的打印宽度（含小数点与小数位） */
    IntLength += FraLength + 1;
    sprintf(formatControl, "%s%d%c%d%c", "%", IntLength, '.', FraLength, 'f');
    sprintf(NumForShow, formatControl, dat);

    WU_OLED_U8G2_DrawStr(x, y, NumForShow);
}

/**
 * @brief  屏保效果：随机线条
 * @note   需要外部周期性调用（例如每 50ms 一次），
 *         依赖全局时钟 Time_ms（可选）与 rand()
 */
void WU_OLED_RandomLines(void)
{
    static uint8_t wu_Lines[4];
    for (uint8_t a = 0; a < 4; a++)
    {
        wu_Lines[a] += rand() % 2 - 1;
        if (wu_Lines[a] > 128)
            wu_Lines[a] -= 128;
        for (uint8_t b = 0; b < rand() % 3 + 3; b++)
        {
            WU_OLED_U8G2_DrawHLine(0, wu_Lines[a] + rand() % 20 - 10, 128);   /* 水平线 */
            WU_OLED_U8G2_DrawVLine(wu_Lines[a] + rand() % 20 - 10, 0, 64);    /* 垂直线 */
        }
    }
}

/**
 * @brief  屏保效果：模拟噪点
 * @note   噪点数量随时间呈正弦变化；依赖 Time_ms 与 rand()
 */
void WU_OLED_Noise(void)
{
    float wu_noise;

    wu_noise = sinf(Time_ms / 4000.0f);

    for (int i = 0; i < wu_noise * 256 + 256; i++)
        WU_OLED_U8G2_DrawPoint(rand() % 128, rand() % 64);
}

/**
 * @brief  屏保效果：浮点密集运算噪点（烧脑跑分风）
 * @note   与 WU_OLED_Noise() 类似，但更强调“密集计算”；
 *         内部含被注释的蜂鸣器音乐示例，按需启用
 */
void WU_OLED_DrawIntensiveComputing(void)
{
    float calculate;

    calculate = sinf(Time_ms / 4000.0f);

    /* 模拟噪点 */
    for (int i = 0; i < calculate * 256 + 256; i++)
        WU_OLED_U8G2_DrawPoint(rand() % 128, rand() % 64);

    /* 可选：随噪点节奏响蜂鸣器 */
    // Beeper_Set_Musical_Tone(1500 + calculate * 400);
    // Beeper_Set_Musical_Tone(1500 + calculate * 400 + rand() % 64 - 32 - (((Time_ms / 1000) % 2 == 1) ? 440 : 0));
}

/**
 * @brief  位图缩放绘制（慢速，代码片段改编自 Arduboy2）
 * @param  x      目标区域左上角横坐标
 * @param  y      目标区域左上角纵坐标
 * @param  bitmap 原始位图数据（XBMP 格式）
 * @param  w1     原始位图宽度
 * @param  h1     原始位图高度
 * @param  w2     目标宽度（缩放后）
 * @param  h2     目标高度（缩放后）
 * @note   使用逐像素方块填充实现缩放，速度较慢，适合静态画面；
 *         绘制颜色受当前绘图颜色影响（XOR 模式 2 时不擦背景）
 */
void WU_OLED_DrawSlowBitmapResize(int x, int y, const uint8_t *bitmap,
                                  int w1, int h1, int w2, int h2)
{
    uint8_t color = u8g2_GetDrawColor(&u8g2);
    float mw = (float)w2 / w1;      /* 水平缩放比例 */
    float mh = (float)h2 / h1;      /* 垂直缩放比例 */
    uint8_t cmw = ceil(mw);         /* 向上取整，保证覆盖 */
    uint8_t cmh = ceil(mh);
    int xi, yi, byteWidth = (w1 + 7) / 8;   /* 原始位图每行字节数 */

    for (yi = 0; yi < h1; yi++)
    {
        for (xi = 0; xi < w1; xi++)
        {
            /* 判断该像素是否为“前景”点（1 位有效） */
            if (*(uint8_t *)(bitmap + yi * byteWidth + xi / 8) & (1 << (xi & 7)))
            {
                u8g2_DrawBox(&u8g2, x + xi * mw, y + yi * mh, cmw, cmh);
            }
            /* 非 XOR 模式下，把背景点清成黑色，保证缩放不留残影 */
            else if (color != 2)
            {
                u8g2_SetDrawColor(&u8g2, 0);
                u8g2_DrawBox(&u8g2, x + xi * mw, y + yi * mh, cmw, cmh);
                u8g2_SetDrawColor(&u8g2, color);
            }
        }
    }
}
