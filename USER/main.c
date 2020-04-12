#include "main.h"

int main(void)
{	
	//normal													 																						0x8000000  0x80000			0x20000000 0x10000
	//SCB->VTOR = SRAM_BASE|0x1000;																								//0x20001000 0xC000 0x2000D000  0x3000
	SCB->VTOR = FLASH_BASE|0x10000;																							//0x8010000  0x70000			0x2000D000 0x3000
	PeriphInit();																																	//设备初始化
	
	while(1)
	{
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
	TIM1_Int_Init(19999,7199);																										//2000ms 提示系统正在运行	LED0 翻转
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 4, CAN_Mode_Normal);			//CAN初始化正常模式,波特率500Kbps  
	IWDG_Init(4, 1250);																														//独立看门狗2s
	RTC_Init();	  																																//RTC初始化
	W25QXX_Init();																																//初始化SPI FLASH的IO口
	my_mem_init(SRAMIN);																													//初始化内部内存池
	printf("\r\n    brake start!   \r\n");
	//init_fatfs();																																	//文件系统初始化
}		

#if 0
void init_fatfs(void)
{
 	u32 total,free;
	u8 res=0;
	
	exfuns_init();							
	f_mount(fs[0],"0:",1); 					
 	res=f_mount(fs[1],"1:",1); 				
	if(res==0X0D)
	{
		printf("Flash Disk Formatting...\r\n");	
		res=f_mkfs("1:",1,4096);
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	
			printf("Flash Disk Format Finish\r\n");	
		}
		else printf("Flash Disk Format Error\r\n");	
		delay_ms(1000);
	}													    

	while(exf_getfree("1:",&total,&free))
	{
		printf("FALSH Fatfs Error!\r\n");
		delay_ms(200);
		LED0=!LED0;
	}
 	printf("flash total%d\r\n",total);				
 	printf("flash free %d\r\n",free);				
}
#endif
