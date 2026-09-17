本示例使用到的资源：
无


本示例的功能流程：
1、上电配置时钟（RCH 48MHz）；
2、启动TIM1计数，当发生上溢事件时，进入中断；

注意：
1、TIM1的时钟 = PCLK = HCLK = HCLK = SystemCoreClock = 48MHz；
2、如果TIM1的计数时钟为10kHz，则TIM1的预分频参数为：
   prescal = (SystemCoreClock /10 kHz) - 1 = 0x12BF
   TIM1的自动重载值为：（10000 - 1）= 0x270F
3、TIM1的更新事件频率为1Hz，即1s产生一次更新事件。