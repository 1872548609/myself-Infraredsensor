本示例使用到的资源：
LED         --   PB1


本示例的功能流程：
1、上电配置时钟、LPTIM及LED灯；
2、LED灯默认为关闭状态；
3、等待3s；
4、配置Flash唤醒时间为0us，配置LPTIM中断挂起方式唤醒，之后进入Deepstop模； 
     等待LPTIM计数溢出，唤醒Deepstop后，清除溢出标志，清除中断挂起标志；
     程序在SRAM中运行，由分散加载文件实现，MDK工程由PMU_Flash_NoWait_Wakeup.sct文件加载；
5、点亮LED灯，并恢复系统时钟配置（程序在Flash中运行）。
