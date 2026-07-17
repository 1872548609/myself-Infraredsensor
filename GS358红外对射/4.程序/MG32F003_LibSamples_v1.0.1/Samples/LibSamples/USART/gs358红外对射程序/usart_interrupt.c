/**
 * @file    usart_interrupt.c
 * @brief   USART1 接收 YC02 TOF 回传帧；校验、解析后经 USART2/RS485 输出字段文本
 *
 * 数据路径：
 * TOF 模块 -> USART1(RXNE + IDLE) -> 主循环校验和解析 -> USART2/MAX485 -> 电脑串口助手
 *
 * 注意：
 * 1. 只对帧头、长度、累加和均正确的 27 字节 YC02 帧打印解析结果；
 * 2. 不再把原始 HEX 帧直接转发到 RS485；
 * 3. USART1 收帧完成后会先重新打开下一帧接收，然后才启动 RS485 文本发送。
 */

#define _USART_INTERRUPT_C_

#include "platform.h"
#include "usart_interrupt.h"

/* -------------------- 串口参数 -------------------- */
#define TOF_USART_BAUDRATE              460800UL
#define RS485_DEBUG_BAUDRATE            115200UL

/* -------------------- MAX485 方向控制 -------------------- */
/* 原理图中 PB0 同时连接 MAX485 的 DE 与 /RE：
 * PB0 = 1：允许发送、关闭接收；
 * PB0 = 0：关闭发送、允许接收。
 */
#define RS485_DIR_PORT                  GPIOB
#define RS485_DIR_PIN                   GPIO_Pin_0

/* 若希望连无效帧也打印一行状态，改成 1；默认只打印校验通过的解析帧。 */
#define TOF_RS485_PRINT_BAD_FRAME       0U

/* USART1：TOF 原始接收；USART2：RS485 解析文本发送。 */
volatile USART_RxTx_TypeDef USART_RxStruct = {0};
volatile USART_RxTx_TypeDef USART_TxStruct = {0};

/* -------------------- YC02 字段读取与帧校验 -------------------- */
static uint16_t TOF_ReadLE16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t TOF_ReadLE24(const uint8_t *p)
{
    return (uint32_t)((uint32_t)p[0] |
                      ((uint32_t)p[1] << 8) |
                      ((uint32_t)p[2] << 16));
}

static int32_t TOF_ReadLE32S(const uint8_t *p)
{
    return (int32_t)((uint32_t)p[0] |
                     ((uint32_t)p[1] << 8) |
                     ((uint32_t)p[2] << 16) |
                     ((uint32_t)p[3] << 24));
}

static uint8_t TOF_Checksum(const uint8_t *buf, uint16_t length_without_checksum)
{
    uint16_t i;
    uint32_t sum = 0U;

    for (i = 0U; i < length_without_checksum; i++)
    {
        sum += buf[i];
    }

    return (uint8_t)(sum & 0xFFU);
}

/*
 * YC02 帧格式：
 * [0..2]   = 5A 8E 17
 * [3..4]   = norm_tof       (LE16)
 * [5..6]   = norm_peak      (LE16)
 * [7..9]   = norm_noise     (LE24)
 * [10..11] = multshot       (LE16)
 * [12..13] = atten_peak     (LE16)
 * [14..16] = atten_noise    (LE24)
 * [17..18] = ref_tof        (LE16)
 * [19..22] = temperature    (LE32, 0.01 C)
 * [23..24] = cal_tof        (LE16, mm)
 * [25]     = confidence
 * [26]     = sum(buf[0..25]) low 8 bit
 */
static uint8_t YC02_ParseFrame(const uint8_t *buf, uint16_t length, TOF_Frame_t *out)
{
    if ((buf == 0) || (out == 0))
    {
        return 0U;
    }

    if (length != TOF_FRAME_LENGTH)
    {
        return 0U;
    }

    if ((buf[0] != 0x5AU) || (buf[1] != 0x8EU) || (buf[2] != 0x17U))
    {
        return 0U;
    }

    if (TOF_Checksum(buf, TOF_FRAME_LENGTH - 1U) != buf[TOF_FRAME_LENGTH - 1U])
    {
        return 0U;
    }

    out->norm_tof          = TOF_ReadLE16(&buf[3]);
    out->norm_peak         = TOF_ReadLE16(&buf[5]);
    out->norm_noise        = TOF_ReadLE24(&buf[7]);
    out->multshot          = TOF_ReadLE16(&buf[10]);
    out->atten_peak        = TOF_ReadLE16(&buf[12]);
    out->atten_noise       = TOF_ReadLE24(&buf[14]);
    out->ref_tof           = TOF_ReadLE16(&buf[17]);
    out->temperature_centi = TOF_ReadLE32S(&buf[19]);
    out->distance_mm       = (int16_t)TOF_ReadLE16(&buf[23]);
    out->confidence        = buf[25];

    return 1U;
}

/* -------------------- USART1：TOF -------------------- */
static void USART1_TOF_Configure(uint32_t Baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART1, ENABLE);

    /* PA12 / AF1：USART1_TX -> TOF_RXD。 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_1);
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PA3 / AF1：USART1_RX <- TOF_TXD。 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate   = Baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits   = USART_StopBits_1;
    USART_InitStruct.USART_Parity     = USART_Parity_No;
    USART_InitStruct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    USART_ITConfig(USART1, USART_IT_PE, ENABLE);
    USART_ITConfig(USART1, USART_IT_ERR, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel         = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00U;
    NVIC_InitStruct.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

static void USART1_TOF_RxIdleStart(uint16_t MaxLength)
{
    uint16_t i;

    if (MaxLength > USART_RX_BUFFER_SIZE)
    {
        MaxLength = USART_RX_BUFFER_SIZE;
    }

    for (i = 0U; i < MaxLength; i++)
    {
        USART_RxStruct.Buffer[i] = 0U;
    }

    USART_RxStruct.Length       = MaxLength;
    USART_RxStruct.CurrentCount = 0U;
    USART_RxStruct.CompleteFlag = 0U;
    USART_RxStruct.OverflowFlag = 0U;

    (void)USART_ReceiveData(USART1);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    USART_ClearFlag(USART1, USART_FLAG_RXNE);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
}

/* -------------------- USART2：RS485 -------------------- */
static void USART2_RS485_Configure(uint32_t Baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2, ENABLE);

    /* PA1 / AF2：USART2_TX -> MAX485 DI。 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PB1 / AF2：USART2_RX <- MAX485 RO。 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_2);
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PB0：MAX485 DE 与 /RE 共用方向脚，上电先设为接收。 */
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin   = RS485_DIR_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(RS485_DIR_PORT, &GPIO_InitStruct);
    GPIO_WriteBit(RS485_DIR_PORT, RS485_DIR_PIN, Bit_RESET);

    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate   = Baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits   = USART_StopBits_1;
    USART_InitStruct.USART_Parity     = USART_Parity_No;
    USART_InitStruct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStruct);

    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART2, USART_IT_TC, DISABLE);

    NVIC_InitStruct.NVIC_IRQChannel         = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x01U;
    NVIC_InitStruct.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART2, ENABLE);
}

/* 发送数据已经预先格式化到 USART_TxStruct.Buffer 中。
 * 函数只在前一条解析文本已经发送完毕后开始下一次发送，
 * 因此每条 RS485 文本都保持完整，不会与上一帧交叉混合。
 */
static void USART2_StartTx(const uint8_t *Buffer, uint16_t Length)
{
    uint16_t i;

    if ((Buffer == 0) || (Length == 0U))
    {
        return;
    }

    if (Length > USART_RX_BUFFER_SIZE)
    {
        Length = USART_RX_BUFFER_SIZE;
    }

    while (USART_TxStruct.CompleteFlag == 0U)
    {
        /* USART1 已经在解析前重新启动；等待上条 RS485 文本发完期间，TOF 仍然可以接收下一帧。 */
    }

    for (i = 0U; i < Length; i++)
    {
        USART_TxStruct.Buffer[i] = Buffer[i];
    }

    USART_TxStruct.Length       = Length;
    USART_TxStruct.CurrentCount = 0U;
    USART_TxStruct.CompleteFlag = 0U;

    /* 清除可能遗留的 TC，再切到发送状态并启动 TXE 中断。 */
    USART_ClearITPendingBit(USART2, USART_IT_TC);
    GPIO_WriteBit(RS485_DIR_PORT, RS485_DIR_PIN, Bit_SET);
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

/* -------------------- 解析文本格式化 -------------------- */
static void Text_AppendChar(uint8_t *Text, uint16_t *Length, uint8_t ch)
{
    if (*Length < USART_RX_BUFFER_SIZE)
    {
        Text[*Length] = ch;
        (*Length)++;
    }
}

static void Text_AppendString(uint8_t *Text, uint16_t *Length, const char *str)
{
    if (str == 0)
    {
        return;
    }

    while (*str != '\0')
    {
        Text_AppendChar(Text, Length, (uint8_t)*str);
        str++;
    }
}

static void Text_AppendU32(uint8_t *Text, uint16_t *Length, uint32_t value)
{
    uint8_t reverse[10];
    uint8_t digit_count = 0U;

    if (value == 0U)
    {
        Text_AppendChar(Text, Length, '0');
        return;
    }

    while ((value != 0U) && (digit_count < (uint8_t)sizeof(reverse)))
    {
        reverse[digit_count] = (uint8_t)('0' + (value % 10U));
        digit_count++;
        value /= 10U;
    }

    while (digit_count != 0U)
    {
        digit_count--;
        Text_AppendChar(Text, Length, reverse[digit_count]);
    }
}

static void Text_AppendU16(uint8_t *Text, uint16_t *Length, uint16_t value)
{
    Text_AppendU32(Text, Length, (uint32_t)value);
}

static void Text_AppendS16(uint8_t *Text, uint16_t *Length, int16_t value)
{
    int32_t signed_value = (int32_t)value;
    uint32_t magnitude;

    if (signed_value < 0)
    {
        Text_AppendChar(Text, Length, '-');
        magnitude = (uint32_t)(-signed_value);
    }
    else
    {
        magnitude = (uint32_t)signed_value;
    }

    Text_AppendU32(Text, Length, magnitude);
}

static void Text_AppendTemperature(uint8_t *Text, uint16_t *Length, int32_t temperature_centi)
{
    uint32_t magnitude;
    uint32_t integer_part;
    uint32_t fractional_part;

    if (temperature_centi < 0)
    {
        Text_AppendChar(Text, Length, '-');
        /* 避免 -INT32_MIN 的溢出。 */
        magnitude = (uint32_t)(-(temperature_centi + 1)) + 1U;
    }
    else
    {
        magnitude = (uint32_t)temperature_centi;
    }

    integer_part    = magnitude / 100U;
    fractional_part = magnitude % 100U;

    Text_AppendU32(Text, Length, integer_part);
    Text_AppendChar(Text, Length, '.');
    Text_AppendChar(Text, Length, (uint8_t)('0' + (fractional_part / 10U)));
    Text_AppendChar(Text, Length, (uint8_t)('0' + (fractional_part % 10U)));
}

/*
 * 只打印已经通过协议校验的解析字段。
 * 输出示例：
 * norm_tof    = 10551
 * norm_peak   = 28041
 * ...
 * confidence  = 100
 */
static void USART2_PrintTofParsedFrame(const TOF_Frame_t *Frame)
{
    uint8_t  Text[USART_RX_BUFFER_SIZE];
    uint16_t out = 0U;

    if (Frame == 0)
    {
        return;
    }

    Text_AppendString(Text, &out, "\r\nnorm_tof    = ");
    Text_AppendU16(Text, &out, Frame->norm_tof);

    Text_AppendString(Text, &out, "\r\nnorm_peak   = ");
    Text_AppendU16(Text, &out, Frame->norm_peak);

    Text_AppendString(Text, &out, "\r\nnorm_noise  = ");
    Text_AppendU32(Text, &out, Frame->norm_noise);

    Text_AppendString(Text, &out, "\r\nmultshot    = ");
    Text_AppendU16(Text, &out, Frame->multshot);

    Text_AppendString(Text, &out, "\r\natten_peak  = ");
    Text_AppendU16(Text, &out, Frame->atten_peak);

    Text_AppendString(Text, &out, "\r\natten_noise = ");
    Text_AppendU32(Text, &out, Frame->atten_noise);

    Text_AppendString(Text, &out, "\r\nref_tof     = ");
    Text_AppendU16(Text, &out, Frame->ref_tof);

    Text_AppendString(Text, &out, "\r\ntemperature = ");
    Text_AppendTemperature(Text, &out, Frame->temperature_centi);
    Text_AppendString(Text, &out, " C");

    Text_AppendString(Text, &out, "\r\ncal_tof     = ");
    Text_AppendS16(Text, &out, Frame->distance_mm);
    Text_AppendString(Text, &out, " mm");

    Text_AppendString(Text, &out, "\r\nconfidence  = ");
    Text_AppendU16(Text, &out, Frame->confidence);
    Text_AppendString(Text, &out, "\r\n");

    USART2_StartTx(Text, out);
}

#if TOF_RS485_PRINT_BAD_FRAME
static void USART2_PrintBadFrame(uint16_t Length, uint8_t Overflow)
{
    uint8_t  Text[64];
    uint16_t out = 0U;

    Text_AppendString(Text, &out, "\r\nTOF frame invalid, len=");
    Text_AppendU16(Text, &out, Length);
    Text_AppendString(Text, &out, ", overflow=");
    Text_AppendU16(Text, &out, Overflow);
    Text_AppendString(Text, &out, "\r\n");

    USART2_StartTx(Text, out);
}
#endif

/* -------------------- 应用主循环 -------------------- */
void USART_Interrupt_Sample(void)
{
    uint8_t     FrameCopy[TOF_FRAME_MAX_LENGTH];
    uint16_t    FrameLength;
    uint16_t    i;
    uint8_t     Overflow;
    TOF_Frame_t ParsedFrame;

    USART_RxStruct.CompleteFlag = 0U;
    USART_TxStruct.CompleteFlag = 1U;

    USART1_TOF_Configure(TOF_USART_BAUDRATE);
    USART2_RS485_Configure(RS485_DEBUG_BAUDRATE);
    USART1_TOF_RxIdleStart(TOF_FRAME_MAX_LENGTH);

    while (1)
    {
        if (USART_RxStruct.CompleteFlag != 0U)
        {
            /* USART1 IDLE 中断已关闭 RXNE/IDLE，因此此时缓冲区不会被 ISR 改写。 */
            FrameLength = USART_RxStruct.CurrentCount;
            Overflow    = USART_RxStruct.OverflowFlag;

            if (FrameLength > TOF_FRAME_MAX_LENGTH)
            {
                FrameLength = TOF_FRAME_MAX_LENGTH;
                Overflow = 1U;
            }

            for (i = 0U; i < FrameLength; i++)
            {
                FrameCopy[i] = USART_RxStruct.Buffer[i];
            }

            /* 先重新开始下一帧 TOF 接收，再做解析与 RS485 输出。 */
            USART1_TOF_RxIdleStart(TOF_FRAME_MAX_LENGTH);

            if ((Overflow == 0U) &&
                (YC02_ParseFrame(FrameCopy, FrameLength, &ParsedFrame) != 0U))
            {
                /* 只有解析成功的帧，才按字段回传到 RS485。 */
                USART2_PrintTofParsedFrame(&ParsedFrame);
            }
#if TOF_RS485_PRINT_BAD_FRAME
            else
            {
                USART2_PrintBadFrame(FrameLength, Overflow);
            }
#endif
        }
    }
}
