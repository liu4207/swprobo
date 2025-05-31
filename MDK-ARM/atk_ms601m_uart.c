/**
 ****************************************************************************************************
 * @file        atk_ms601m_uart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS601M模块UART接口驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "atk_ms601m_uart.h"
#include "usart.h"    // 引入你生成的 usart.h 文件，使用 huart1
#define UART_DMA_RX_BUF_SIZE 256
static uint8_t uart_dma_rx_buf[UART_DMA_RX_BUF_SIZE];  // DMA环形接收缓冲区
static uint16_t last_dma_pos = 0;
extern DMA_HandleTypeDef hdma_usart1_rx;

extern UART_HandleTypeDef huart1;
//static UART_HandleTypeDef *g_uart_handle = &huart1;  // 使用 USART1 句柄
//static UART_HandleTypeDef g_uart_handle;            /* ATK-MS601M UART */
static struct
{
    uint8_t buf[ATK_MS601M_UART_RX_FIFO_BUF_SIZE];  /* 缓冲 */
    uint16_t size;                                  /* 缓冲大小 */
    uint16_t reader;                                /* 读指针 */
    uint16_t writer;                                /* 写指针 */
} g_uart_rx_fifo;                                   /* UART接收FIFO */

/**
 * @brief       ATK-MS601M UART接收FIFO写入数据
 * @param       dat: 待写入数据
 *              len: 待写入数据的长度
 * @retval      0: 函数执行成功
 *              1: FIFO剩余空间不足
 */
uint8_t atk_ms601m_uart_rx_fifo_write(uint8_t *dat, uint16_t len)
{
    uint16_t i;
    
    /* 将数据写入FIFO
     * 并更新FIFO写入指针
     */
    for (i=0; i<len; i++)
    {
        g_uart_rx_fifo.buf[g_uart_rx_fifo.writer] = dat[i];
        g_uart_rx_fifo.writer = (g_uart_rx_fifo.writer + 1) % g_uart_rx_fifo.size;
    }
    
    return 0;
}

/**
 * @brief       ATK-MS601M UART接收FIFO读取数据
 * @param       dat: 读取数据存放位置
 *              len: 欲读取数据的长度
 * @retval      0: FIFO中无数据
 *              其他值: 实际读取的数据长度
 */
uint16_t atk_ms601m_uart_rx_fifo_read(uint8_t *dat, uint16_t len)
{
    uint16_t fifo_usage;
    uint16_t i;
    
    /* 获取FIFO已使用大小 */
    if (g_uart_rx_fifo.writer >= g_uart_rx_fifo.reader)
    {
        fifo_usage = g_uart_rx_fifo.writer - g_uart_rx_fifo.reader;
    }
    else
    {
        fifo_usage = g_uart_rx_fifo.size - g_uart_rx_fifo.reader + g_uart_rx_fifo.writer;
    }
    
    /* FIFO数据量不足 */
    if (len > fifo_usage)
    {
        len = fifo_usage;
    }
    
    /* 从FIFO读取数据
     * 并更新FIFO读取指针
     */
    for (i=0; i<len; i++)
    {
        dat[i] = g_uart_rx_fifo.buf[g_uart_rx_fifo.reader];
        g_uart_rx_fifo.reader = (g_uart_rx_fifo.reader + 1) % g_uart_rx_fifo.size;
    }
    
    return len;
}

/**
 * @brief       ATK-MS601M UART接收FIFO清空
 * @param       无
 * @retval      无
 */
void atk_ms601m_rx_fifo_flush(void)
{
    g_uart_rx_fifo.writer = g_uart_rx_fifo.reader;
}

/**
 * @brief       ATK-MS601M UART发送数据
 * @param       dat: 待发送的数据
 *              len: 待发送数据的长度
 * @retval      无
 */
void atk_ms601m_uart_send(uint8_t *dat, uint8_t len)
{
    HAL_UART_Transmit(&huart1, dat, len, HAL_MAX_DELAY);
}

///**
// * @brief       ATK-MS601M UART初始化
// * @param       baudrate: UART通讯波特率
// * @retval      无
// */
void atk_ms601m_uart_init()
{
//    g_uart_handle.Instance          = ATK_MS601M_UART_INTERFACE;    /* ATK-MS601M UART */
//    g_uart_handle.Init.BaudRate     = baudrate;                     /* 波特率 */
//    g_uart_handle.Init.WordLength   = UART_WORDLENGTH_8B;           /* 数据位 */
//    g_uart_handle.Init.StopBits     = UART_STOPBITS_1;              /* 停止位 */
//    g_uart_handle.Init.Parity       = UART_PARITY_NONE;             /* 校验位 */
//    g_uart_handle.Init.Mode         = UART_MODE_TX_RX;              /* 收发模式 */
//    g_uart_handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;          /* 无硬件流控 */
//    g_uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;         /* 过采样 */
//    HAL_UART_Init(&g_uart_handle);                                  /* 使能ATK-MS601M UART
//                                                                     * HAL_UART_Init()会调用函数HAL_UART_MspInit()
//                                                                     * 该函数定义在文件usart.c中
//                                                                     */
    g_uart_rx_fifo.size = ATK_MS601M_UART_RX_FIFO_BUF_SIZE;         /* UART接收FIFO缓冲大小 */
    g_uart_rx_fifo.reader = 0;                                      /* UART接收FIFO读指针 */
    g_uart_rx_fifo.writer = 0;                                      /* UART接收FIFO写指针 */
	
//	    // 这里不用再初始化 UART，MX_USART1_UART_Init() 已经初始化好了
//    // 只需要开启 USART1 的中断接收即可
//    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	    // 启动DMA环形接收
    HAL_UART_Receive_DMA(&huart1, uart_dma_rx_buf, UART_DMA_RX_BUF_SIZE);

    // 使能空闲中断（用来检测数据帧边界）
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

/**
 * @brief       ATK-MS601M UART中断回调函数
 * @param       无
 * @retval      无
 */
void ATK_MS601M_UART_IRQHandler(void)
{
    uint8_t tmp;
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
	{
    __HAL_UART_CLEAR_IDLEFLAG(&huart1); // 清除 IDLE 标志

    uint16_t dma_pos = UART_DMA_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); // 当前DMA接收指针
    uint16_t data_len;

    if (dma_pos >= last_dma_pos)
    {
        data_len = dma_pos - last_dma_pos;
        atk_ms601m_uart_rx_fifo_write(&uart_dma_rx_buf[last_dma_pos], data_len);
    }
    else
    {
        // 发生了DMA缓冲区回绕
        atk_ms601m_uart_rx_fifo_write(&uart_dma_rx_buf[last_dma_pos], UART_DMA_RX_BUF_SIZE - last_dma_pos);
        atk_ms601m_uart_rx_fifo_write(&uart_dma_rx_buf[0], dma_pos);
    }

    last_dma_pos = dma_pos;
	}

    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE) != RESET)    /* UART接收过载错误中断 */
    {
        __HAL_UART_CLEAR_OREFLAG(&huart1);                       /* 清除接收过载错误中断标志 */
        (void)huart1.Instance->SR;                               /* 先读SR寄存器，再读DR寄存器 */
        (void)huart1.Instance->DR;
    }
    
   // if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_RXNE) != RESET)   /* UART接收中断 */
    //{
       // HAL_UART_Receive(&g_uart_handle, &tmp, 1, HAL_MAX_DELAY);       /* UART接收数据 */
				if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
					{
							tmp = (uint8_t)(huart1.Instance->DR & 0xFF);  // 直接读 DR 寄存器
							atk_ms601m_uart_rx_fifo_write(&tmp, 1);
					}
      //  atk_ms601m_uart_rx_fifo_write(&tmp, 1);                         /* 接收到的数据，写入UART接收FIFO */
   // }
}
