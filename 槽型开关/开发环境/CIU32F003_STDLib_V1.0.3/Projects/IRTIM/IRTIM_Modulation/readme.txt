本示例使用到的资源：
IR_OUT    --  PA7
UART1_RX  --  PA4

红外发送端与IR_OUT引脚连接，红外接收端与UART_RX引脚相连

本示例的功能流程：
1、上电配置时钟（RCH 48MHz）；
2、初始化IRTIM的IR_OUT的GPIO，初始化UART1_RX的GPIO；
3、初始化IRTIM的载波信号源TIM1，TIM1_CH1产生38KHz，占空比50%的PWM信号；
4、初始化IRTIM的调制信号源UART1，UART1输出波特率为2400的串行数据；
5、控制IRTIM载波与调制信号输出，同时UART1_RX通过红外接收头接收解调信号。
