本示例使用的硬件资源：
UART1 TX      --   PA3 
UART2 TX      --   PA5

UART通信参数配置:
  -- 波特率:   115200
  -- 字符长度：8bits
  -- 停止位：  1bit
  -- 奇偶校验：无
  -- 硬件流控：无
  
本示例的功能流程：
1、上电配置系统时钟为RCH（48MHz）；
2、初始化UART1和UART2的 GPIO，使用复用开漏模式，使能上拉电阻；
3、使能UART1和UART2的单线半双工模式；
3、UART1发送8个字节UART2接收，然后UART2发送UART1接收；
