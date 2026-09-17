本示例使用到的资源：
LED           --   PB1
COMP1 input   --   PB0

本示例的功能流程：

COMP1比较器输入端PB0初始状态连接到GND。

1、上电配置时钟（RCH 48MHz）；
2、初始化LED灯熄灭；
3、初始化COMP1,正相输入是IO PB0，反相输入VBGR；
4、初始化EXTI，配置EXTI通道是COMP1，中断唤醒，上升沿触发;
5、延时3s后，进入DeepStop模式；
6、手动将PB0引脚电压由GND切换至大于VBGR（0.8V），产生上升沿唤醒DeepStop；
7、唤醒后，LED闪烁；

