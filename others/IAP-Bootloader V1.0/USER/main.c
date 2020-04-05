#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"
 
 
/************************************************
 ALIENTEK精英STM32开发板实验42
 IAP实验 Bootloader V1.0 代码 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


int main(void)
{		
	u8 key;
	u16 oldcount=0;				//老的串口接收数据值
	u16 applenth=0;				//接收到的app代码长度
	int countflag=0;   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	//串口初始化为115200
	delay_init();	   	 	//延时初始化 
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
 
	printf("ELITE STM32F103 ^_^\r\n");	
	printf("update CAN.bin file, Wait 5 seconds before executing the existing program\r\n");	
  
	while(1)
	{
	 	if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("用户程序接收完成!\r\n");
				printf("代码长度:%dBytes\r\n",applenth);
				printf("input: \r\nkey_up-copy \r\nkey1-flash \r\nkey0-sram\r\n");	
			}else oldcount=USART_RX_CNT;	
			countflag = -1;
		}

		delay_ms(10);
 	 
		//五秒内没有接收到bin文件，就执行flash的程序或者SRAM的程序
		{
			if(countflag!=-1)
			{
				countflag++;
				printf("wait %d s\r\n", countflag);	
				delay_ms(1000);
				if(countflag==5)
				{
					if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
					{	 
						printf("执行FLASH应用程序");	  
						iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
						countflag = -1;
					}
					
					if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x20000000)//判断是否为0X20XXXXXX.
					{	
						printf("执行SRAM应用程序");	
						iap_load_app(0X20001000);//SRAM地址
						countflag = -1;
					}
				}
			}
		}
		
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			delay_ms(500);
			if(applenth)
			{
				printf("开始更新固件...\r\n");	
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//更新FLASH代码   
					printf("固件更新完成!\r\n");	
				}else 
				{  
					printf("非FLASH应用程序!\r\n");
				}
 			}else 
			{
				printf("没有可以更新的固件!\r\n");
			}									 
		} 
		if(key==KEY1_PRES)
		{
			delay_ms(500);
			printf("开始执行FLASH用户代码 ");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else 
			{
				printf("非FLASH应用程序,无法执行 ");	   
			}									   
		}
		if(key==KEY0_PRES)
		{
			delay_ms(500);
			printf("开始执行SRAM用户代码 ");
			if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x20000000)//判断是否为0X20XXXXXX.
			{	 
				iap_load_app(0X20001000);//SRAM地址
			}else 
			{
				printf("非SRAM应用程序,无法执行!");   
			}									 	 
		}				    
	}   	   
}











