#include "main.h"

int main(void)
{	
	PeriphInit();																																	//设备初始化
	
	LED0 = 0;
	USART_SendData(USART1, 'a');
	while(1)
	{
		LED0 = 1;
		key_brake();																																//按键1开启、0退出急停模式
	}
}

void PeriphInit(void)
{
	delay_init();	    	 																													//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 																													//串口初始化为115200
	LED_Init();		  																															//初始化与LED连接的硬件接口
	KEY_Init();																																		//按键初始化
	//TIM1_Int_Init(4999,7199);																											//500ms
	TIM1_Int_Init(19999,7199);																											//2000ms
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 4, CAN_Mode_Normal);			//CAN初始化正常模式,波特率500Kbps  
	WWDG_Init(0x7f, 0x5f, WWDG_Prescaler_8);																			//提示系统正在运行	LED0 翻转
	RTC_Init();	  																																//RTC初始化
}		

