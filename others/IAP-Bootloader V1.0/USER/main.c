#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"

int main(void)
{		
	u8 key;
	u16 oldcount=0;				//老的串口接收数据值
	u16 applenth=0;				//接收到的app代码长度
	int countflag=0;   
	u8  flag=0;   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	//串口初始化为115200
	delay_init();	   	 	//延时初始化 
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
 
	printf("ELITE STM32F103 ^_^\r\n");	
	printf("Update CAN.bin file, Wait 5 seconds before executing the existing program\r\n");	
  
	while(1)
	{
	 	if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("Recive success!\r\n");
				printf("Code length:%dBytes\r\n",applenth);
			}else oldcount=USART_RX_CNT;	
			countflag = -1;
			flag = 1;
		}

		delay_ms(10);
 	 
		//五秒内没有接收到bin文件，就执行flash的程序或者SRAM的程序
		{
			if(countflag!=-1)
			{
				countflag++;
				printf("Wait %d s\r\n", countflag);	
				delay_ms(1000);
				if(countflag==5)
				{
					countflag = -1;
					if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
					{	 
						printf("Execute flash!\r\n");	  
						iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
					}
					else
					{
						printf("Flash is no execute!\r\n");
					}
				}
			}
		}
		
		key=KEY_Scan(0);
		if(flag)
		{
			flag = 0;
			delay_ms(10000);
			if(applenth)
			{
				printf("Update...\r\n");	
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//更新FLASH代码   
					printf("Update success \r\n");	
					printf("Input: \r\nkey0-Start flash \r\n");	
				}else 
				{  
					printf("Flash Update err \r\n");
				}
 			}								 
		} 
		if(key==KEY0_PRES)
		{
			delay_ms(500);
			printf("Execute flash \r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else 
			{
				printf("Flash Execute err \r\n");	   
			}									   
		}			    
	}   	   
}











