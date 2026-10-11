/*------------------------------------------------------------------------------
 * 文件名称 : IIC_OLED.c
 * 所属模块 : STM32_U8g2_Wrapper - 移植层（Keil MDK + 标准外设库 SPL）
 * 功能说明 : 软件模拟 I2C 底层实现（GPIO 位操作）。
 *            u8g2 通过 u8g2_gpio_and_delay_stm32() 回调本文件的
 *            IIC_OLED_SCL_HIGH/LOW 与 IIC_OLED_SDA_HIGH/LOW 驱动屏幕。
 *
 * 接线说明 : SDA = PB8，SCL = PB9（按下表修改宏即可换引脚）：
 *                IIC_OLED_SDA_Pin        -> 数据引脚
 *                IIC_OLED_SDA_GPIO       -> 数据引脚所在 GPIO 组
 *                IIC_OLED_SDA_GPIO_CLK   -> 对应 RCC 时钟使能位
 *                （SCL 同理）
 *
 * 编码要求 : UTF-8 编码保存。
 *------------------------------------------------------------------------------
 */
#include "IIC_OLED.h"

/*==============================================================================
 * 引脚配置（可按实际接线修改）
 *==============================================================================
 */
#define IIC_OLED_SDA_Pin        GPIO_Pin_8        /* SDA -> PB8 */
#define IIC_OLED_SDA_GPIO       GPIOB             /* SDA 所在端口 */
#define IIC_OLED_SDA_GPIO_CLK   RCC_APB2Periph_GPIOB

#define IIC_OLED_SCL_Pin        GPIO_Pin_9        /* SCL -> PB9 */
#define IIC_OLED_SCL_GPIO       GPIOB             /* SCL 所在端口 */
#define IIC_OLED_SCL_GPIO_CLK   RCC_APB2Periph_GPIOB

/**
 * @brief  软件 I2C 引脚初始化（推挽输出，50MHz）
 * @note   若不需要释放 JTAG 引脚（PB3/PB4），可删除下面的
 *         AFIO 时钟使能与 SWJ_JTAGDisable 两行
 */
void OLED_IoInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    /* 开启 GPIO 时钟 */
    RCC_APB2PeriphClockCmd(IIC_OLED_SDA_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(IIC_OLED_SCL_GPIO_CLK, ENABLE);

    /* 关闭 JTAG、保留 SWD：释放 PB3/PB4 复用（与 OLED 无关，按需保留） */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /* 配置 SDA：推挽输出，50MHz */
    GPIO_InitStructure.GPIO_Pin   = IIC_OLED_SDA_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;      /* 推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IIC_OLED_SDA_GPIO, &GPIO_InitStructure);

    /* 配置 SCL：推挽输出，50MHz */
    GPIO_InitStructure.GPIO_Pin   = IIC_OLED_SCL_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;      /* 推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      /* 也可选 2MHz/10MHz */
    GPIO_Init(IIC_OLED_SCL_GPIO, &GPIO_InitStructure);
}

/* 以下电平控制函数由 u8g2 的 GPIO 消息回调驱动 */
void IIC_OLED_SDA_HIGH(void)
{
    GPIO_SetBits(IIC_OLED_SDA_GPIO, IIC_OLED_SDA_Pin);
}

void IIC_OLED_SDA_LOW(void)
{
    GPIO_ResetBits(IIC_OLED_SDA_GPIO, IIC_OLED_SDA_Pin);
}

void IIC_OLED_SCL_HIGH(void)
{
    GPIO_SetBits(IIC_OLED_SCL_GPIO, IIC_OLED_SCL_Pin);
}

void IIC_OLED_SCL_LOW(void)
{
    GPIO_ResetBits(IIC_OLED_SCL_GPIO, IIC_OLED_SCL_Pin);
}
