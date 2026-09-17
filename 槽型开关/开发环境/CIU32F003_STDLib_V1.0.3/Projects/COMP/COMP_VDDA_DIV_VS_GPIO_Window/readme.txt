本示例使用到的资源：
LED          --   PB1
COMP1 input  --   PB0
COMP2 input  --   PA4
COMP1 output --   PB3

本示例的功能流程：

窗口比较器的上限是阈值上限是VDDA的10/16（当VDD=3.3V时，内部电压源是2.0625V），
阈值下限由PA4决定， PA4电压必须小于VDDA的10/16同时大于0V。 PB0的初始值为GND。

1、上电配置时钟（RCH 48MHz）；
2、初始化LED灯熄灭；
3、初始化COMP1为非窗口比较器模式，正相输入是IO PB0，反相输入为VDDA的10/16，输出结果为COMP1\2输出异或;
4、初始化COMP2为窗口比较模式，正相输入COMP1的正向信号，反相输入PA4；
5、检查COMP1输出，手动切换PB0引脚输入电压，如果PB0电压大于VDDA的10/16 输出或小于PA4的输入，LED点亮，反之，LED保持熄灭。

