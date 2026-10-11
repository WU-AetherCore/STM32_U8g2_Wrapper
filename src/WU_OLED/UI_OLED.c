/*------------------------------------------------------------------------------
 * 文件名称 : UI_OLED.c
 * 所属模块 : STM32_U8g2_Wrapper - 个人封装层
 * 功能说明 : OLED 界面（UI）层实现。
 *            包含一键初始化、开机欢迎动画、模糊转场与三种缓动动画。
 *
 * 编码要求 : UTF-8 编码保存。
 *
 * 依赖说明 : 移植层需提供 OLED_IoInit() 与 WU_u8g2Init()（见本文件底部
 *            extern 声明），二者分别在 examples 的 IIC_OLED 与
 *            WU_U8g2_Init 中实现。
 *
 * 版本记录 : V2.0  2026-10-11  由原 WUYDD-U8g2 工程整理开源
 *            OLED_U8G2_Init 不再强制播放开机 Logo（如需可调用
 *            移植层 draw()），依赖关系更清晰。
 *------------------------------------------------------------------------------
 */
#include "UI_OLED.h"
#include "stdlib.h"     /* abs() 等 */
#include "string.h"
#include "stdio.h"
#include "math.h"       /* fabs() */

/*------------------------------------------------------------------------------
 * 移植层接口声明（由 examples 下的 IIC_OLED / WU_U8g2_Init 提供）
 *------------------------------------------------------------------------------
 */
extern void OLED_IoInit(void);              /* IO 初始化：配置软件 I2C 引脚 */
extern void WU_u8g2Init(u8g2_t *u8g2);      /* u8g2 初始化：Setup + InitDisplay + 清屏 */

/**
 * @brief  OLED 一键初始化
 * @note   调用顺序：IO 初始化 -> u8g2 初始化
 *         若需要播放 u8g2 官方 Logo（U/8/g/2），可在此后调用
 *         draw(&u8g2)（由移植层提供），再执行 SendBuffer + ClearBuffer
 */
void OLED_U8G2_Init(void)
{
    OLED_IoInit();              /* 1. 软件 I2C GPIO 初始化 */
    WU_u8g2Init(&u8g2);         /* 2. u8g2 结构体配置 + 屏幕初始化 + 清屏 */
}

/**
 * @brief  开机欢迎动画（HelloHOPE 进度条 + 模糊转场）
 * @note   阻塞式：内部 while(1) 循环，动画完成后 break 返回。
 *         流程：模糊转场（HugoUI） -> 进度条填充动画 -> 返回
 */
void WU_OLED_U8g2Init(void)
{
    uint8_t HugoUIEnter_flag = 1;       /* 模糊转场进行标志   */
    uint8_t HugoUIInit_flag  = 1;       /* 进度条动画进行标志 */
    float FrameBox_Width, FrameBox_Width_trg = 105;   /* 进度条宽度动画参数 */

    /* 选择中文字库（内存紧张时可换 u8g2_font_profont15_mr 等小字库） */
    WU_OLED_U8G2_SetFont(u8g2_font_wqy13_t_gb2312a);

    while (1)
    {
        WU_OLED_U8G2_ClearBuffer();

        /* 第一阶段：整屏模糊转场（“退场”特效） */
        if (HugoUIEnter_flag)
        {
            WU_OLED_U8G2_DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            if (HugoUI() == 0)              /* 转场完成 */
                HugoUIEnter_flag = 0;
            WU_OLED_U8G2_SendBuffer();
            continue;
        }

        /* 第二阶段：绘制进度条外框与提示文本 */
        if (HugoUIInit_flag == 1)
        {
            /* 进度条外框（右侧动态展开） */
            WU_OLED_U8G2_DrawRFrame(128 - (FrameBox_Width + 7), 33 - 10, FrameBox_Width, FONT_HEIGHT, 1);
            /* 居中提示文字 */
            WU_OLED_U8G2_DrawUTF8((128 - WU_OLED_U8G2_GetUTF8Width(" 请等待初始化完成 ")) / 2, 48 - 10, " 请等待初始化完成 ");
            WU_OLED_U8G2_DrawUTF8((128 - WU_OLED_U8G2_GetUTF8Width("HelloHOPE")) / 2, FONT_HEIGHT, "HelloHOPE");
            /* 进度条内部填充（XOR 反色绘制） */
            WU_OLED_U8G2_SetDrawColor(2);
            WU_OLED_U8G2_DrawRBox(FrameBox_Width / 8, 35 - 10, FrameBox_Width, FONT_HEIGHT, 0);
            WU_OLED_U8G2_SetDrawColor(1);
        }

        /* 进度条线性展开动画，完成后跳出 */
        if (WU_UI_Animation_Linear(&FrameBox_Width, &FrameBox_Width_trg, 95) == 0 && HugoUIInit_flag == 1)
        {
            HugoUIInit_flag = 0;
            break;                          /* 初始化动画结束 */
        }
        WU_OLED_U8G2_SendBuffer();
    }
}

/*------------------------------------------------------------------------------
 *                              动画函数
 *------------------------------------------------------------------------------
 */

/**
 * @brief  线性滑动动画（匀速逼近目标值）
 * @param  a      当前值指针（每帧按比例向目标靠拢）
 * @param  a_trg  目标值指针
 * @param  n      运动速度系数（越大越慢）
 * @return 1 = 动画进行中；0 = 已到达目标值
 */
uint8_t WU_UI_Animation_Linear(float *a, float *a_trg, uint8_t n)
{
    if (*a == *a_trg)
        return 0;
    else
    {
        if (fabs(*a - *a_trg) < 0.15f)      /* 接近目标则直接吸附 */
            *a = *a_trg;
        else
            *a += (*a_trg - *a) / (n / 10.0f);
    }
    return 1;
}

/**
 * @brief  缓出动画（开始快、结尾慢，适合“滑入停止”场景）
 * @param  a      当前值指针
 * @param  a_trg  目标值指针
 * @param  n      运动速度系数（越大越慢）
 * @return 1 = 动画进行中；0 = 已到达目标值
 */
uint8_t HugoUI_Animation_EasyOut(float *a, float *a_trg, uint16_t n)
{
    if (*a == *a_trg)
        return 0;

    float cz = fabs(*a - *a_trg);

    if (cz <= 1)                    /* 距离小于 1px，直接吸附 */
        *a = *a_trg;
    else
    {
        if (cz < 10)                /* 越接近目标，速度越慢 */
            n = n * cz * 0.1f;
        if (n < 10)
            n = 10;                 /* 限制最小步数，避免卡顿 */
        *a += (*a_trg - *a) / (n * 0.1f);
    }
    return 1;
}

/**
 * @brief  缓入动画（开始慢、结尾快，适合“启动加速”场景）
 * @param  a      当前值指针
 * @param  a_trg  目标值指针
 * @param  n      运动速度系数（越大越慢）
 * @return 1 = 动画进行中；0 = 已到达目标值
 */
uint8_t HugoUI_Animation_EasyIn(float *a, float *a_trg, uint16_t n)
{
    if (*a == *a_trg)
        return 0;

    float cz = fabs(*a - *a_trg);
    if (cz <= 1)                    /* 距离小于 1px，直接吸附 */
        *a = *a_trg;
    else if (cz > 20)               /* 距离越远，起步越快 */
        n = n * 3;
    else if (cz > 15)
        n = n * 2;
    else if (cz > 5)
        n = n * 1;

    if (*a != *a_trg)
        *a += (*a_trg - *a) / (n * 0.1f);
    else
        return 0;
    return 1;
}

/**
 * @brief  UI 模糊转场效果（直接操作帧缓冲区实现逐级模糊退场）
 * @note   原理：把缓冲区中偶数/奇数位字节逐级按位清零，
 *        造成画面“溶解”消失的观感；共约 21 帧。
 * @return 1 = 转场未完成；0 = 转场已完成
 */
uint8_t HugoUI(void)
{
    int len = 8 * WU_OLED_U8G2_GetBufferTileHeight() * WU_OLED_U8G2_GetBufferTileWidth();
    uint8_t *p = WU_OLED_U8G2_GetBufferPtr();
    uint8_t return_flag = 0;
    static uint8_t Blur_Effect_temp = 0;    /* 模糊转场帧计数 */

    if (Blur_Effect_temp >= 6)      /* 第 6 帧起：清掉偶数位像素 */
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 == 0)
                p[i] = p[i] & (0x55);
        }
    }
    if (Blur_Effect_temp >= 13)     /* 第 13 帧起：清掉奇数位像素 */
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 != 0)
                p[i] = p[i] & (0xaa);
        }
    }
    if (Blur_Effect_temp >= 17)     /* 第 17 帧起：偶数位像素全部清零 */
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 == 0)
                p[i] = p[i] & (0x00);
        }
    }
    if (Blur_Effect_temp >= 20)     /* 第 20 帧起：奇数位像素全部清零（画面全灭） */
    {
        for (int i = 0; i < len; i++)
        {
            if (i % 2 != 0)
                p[i] = p[i] & (0x00);
        }
    }
    Blur_Effect_temp += 1;
    if (Blur_Effect_temp > 21)      /* 达到结束帧，复位并返回完成 */
    {
        return_flag = 1;
        Blur_Effect_temp = 0;
    }
    return return_flag ? 0 : 1;
}
