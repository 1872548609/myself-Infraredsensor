本示例使用到的资源：
TIM3 CH1     --   PA5
RCC  MCO     --   PA7


本示例的功能流程：
1、上电配置时钟（RCH 48MHz）；
2、配置MCO输出RCHDIV8的8分频（1MHz）；
3、MCO信号连接到TIM3的通道1；
4、配置TIM3的通道1（TI1FP1）映射到IC1上，上升沿有效时，触发捕获事件；
5、配置TIM3的通道1（TI1FP1）映射到IC2上，下降沿有效时，触发捕获事件；
6、在中断服务程序中计算MCO上的时钟占空比和频率：
   g_pwm_frequency：计算获得的PMW频率；
   g_pwm_duty：计算获得的PWM占空比。
   