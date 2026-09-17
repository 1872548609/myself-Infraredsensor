本示例使用到的资源：
SPI1_NSS           --    PB1(GPIO配置为输出推挽)
SPI1_SCK           --    PB0
SPI1_MOSI          --    PA0
SPI1_MISO          --    PA1


本示例的功能流程：
1、上电配置系统时钟为RCH（48MHz）；
2、初始化SPI的GPIO管脚；
3、SPI1初始化为主机模式，通信速率为PCLK/32（1.5MHZ），
   通信模式为模式2，数据位为MSB模式，使能SPI；
4、SPI1作为主机拉低NSS片选从机，开始以轮询方式数据
   收发的流程，收发数据完成后，拉高NSS片选信号，结束通信。
