#include "main.h"

int main(void)
{	
	PeriphInit();
	
	while(1)
	{
		key_brake();																										//按键1开启、0退出急停模式
		LED0=!LED0;																											//提示系统正在运行	
		delay_ms(200);
	}
}

void PeriphInit(void)
{
	delay_init();	    	 																											//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);														//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 																											//串口初始化为115200
	LED_Init();		  																													//初始化与LED连接的硬件接口
	KEY_Init();																																//按键初始化
	TIM1_Int_Init(4999,7199);	
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 4, CAN_Mode_Normal);	//CAN初始化正常模式,波特率500Kbps  
}
 
