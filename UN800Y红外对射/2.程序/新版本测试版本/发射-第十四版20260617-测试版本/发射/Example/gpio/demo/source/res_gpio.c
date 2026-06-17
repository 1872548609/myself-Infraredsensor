#include "res_gpio.h"




void res_gpio_test(void)
{
	delay1ms(4000);
	
	ESTCR &= ~(1<<3);									//P0_2外部复位功能禁止
	gpio_init(P0_2);                  //P0_2初始化
	gpio_dir_set(P0_2, GPIO_DIR_OUT); //配置P0_2为输出
	
	while(1)
	{
		gpio_io_set(P0_2, GPIO_HIGH);						
		delay1ms(1000);
		
		gpio_io_set(P0_2, GPIO_LOW); 
		delay1ms(1000);
	}
}










