/*
 * 请在 app.c 中，用下面函数整体替换原来的 uart0_rec_pro()。
 *
 * 原程序的问题：
 * uart0_rx_buf 只有 32 字节，但 rx_count 一直递增且没有边界判断。
 * 接收第 33 个字节后会写出数组边界，可能破坏相邻 RAM、函数指针或状态变量，
 * 这是当前程序最明确的“跑飞”风险。
 */
void uart0_rec_pro(void)
{
    uint8_t rx_byte;

    rx_byte = uart0_recv_byte();

    /* 作为循环缓冲区使用，保证任何情况下都不会越界写 RAM */
    if (rx_count >= sizeof(uart0_rx_buf))
    {
        rx_count = 0;
    }

    uart0_rx_buf[rx_count] = rx_byte;
    rx_count++;

    rx_flag = 1;
}
