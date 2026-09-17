本示例使用到的资源：
LED          --   PB1
COMP1 input  --   PB0

本示例的功能流程：
COMP比较器正相输入端PB0，初始状态连接到GND。

1、上电配置时钟（RCH 48MHz）；
2、初始化LED,初始状态是熄灭；
3、初始化COMP1，正相输入是IO PB0，反相输入VBGR，非窗口比较器模式；
4、手动切换PB0引脚电压，如果电压大于VBGR（0.8V），LED亮，反之，LED灭。

