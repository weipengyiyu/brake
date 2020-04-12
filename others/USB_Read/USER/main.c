#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "memory.h"	    
#include "usb_bot.h"  
#include "ff.h"  
#include "exfuns.h"  
 
/************************************************
 ALIENTEK精英STM32开发板实验44
 USB读卡器 实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
extern u8 Max_Lun;	

void init_fatfs(void)
{
 	u32 total,free;
	u8 res=0;
	
	exfuns_init();							//?fatfs????????				 
	f_mount(fs[0],"0:",1); 					//??SD? 
 	res=f_mount(fs[1],"1:",1); 				//??FLASH.	
	if(res==0X0D)//FLASH??,FAT??????,?????FLASH
	{
		printf("Flash Disk Formatting...\r\n");	//???FLASH
		res=f_mkfs("1:",1,4096);//???FLASH,1,??;1,??????,8????1??
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//??Flash??????:ALIENTEK
			printf("Flash Disk Format Finish\r\n");	//?????
		}else printf("Flash Disk Format Error\r\n");	//?????
		delay_ms(1000);
	}													    

	while(exf_getfree("1:",&total,&free))	//??SD??????????
	{
		printf("FALSH Fatfs Error!\r\n");
		delay_ms(200);
		LED0=!LED0;//DS0??
	}
 	printf("total%d\r\n",total);				//??SD???? MB
 	printf("free%d\r\n",free);					//??SD????? MB			
}

void init(void)
{
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	LED_Init();		  			//初始化与LED连接的硬件接口

	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
}

void init_usb(void)
{
#if 0
	if(SD_Init())
	{
		Max_Lun=0;											//SD卡错误,则仅只有一个磁盘.
		printf("SD Card Error!");	//检测SD卡错误
	}
	else //SD 卡正常
	{   															  
		printf("SD Card Size:     MB");
 	    Mass_Memory_Size[1]=SDCardInfo.CardCapacity;		//得到SD卡容量（字节），当SD卡容量超过4G的时候,需要用到两个u32来表示
	    Mass_Block_Size[1] =512;							//因为我们在Init里面设置了SD卡的操作字节为512个,所以这里一定是512个字节.
	    Mass_Block_Count[1]=Mass_Memory_Size[1]/Mass_Block_Size[1];

 	}
#endif
	if(W25QXX_TYPE!=W25Q128)printf("W25Q128 Error!");	//检测SD卡错误
	else //SPI FLASH 正常
	{   															  
 	  Mass_Memory_Size[0]=1024*1024*12;	//前12M字节
	  Mass_Block_Size[0] =512;			//设置SPI FLASH的操作扇区大小为512
	  Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
		printf("SPI FLASH Size:12MB");	 
	} 
	delay_ms(1800);
	USB_Port_Set(0); 	//USB先断开
	delay_ms(700);
	USB_Port_Set(1);	//USB再次连接 
	printf("USB Connecting...");	//提示USB开始连接	 
  Data_Buffer=mymalloc(SRAMIN,BULK_MAX_PACKET_SIZE*2*4);	//为USB数据缓存区申请内存
	Bulk_Data_Buff=mymalloc(SRAMIN,BULK_MAX_PACKET_SIZE);	//申请内存
 	//USB配置
 	USB_Interrupts_Config();    
 	Set_USBClock();   
 	USB_Init();	    
	delay_ms(1800);
}

void usb_stat(void)
{
	u8 USB_STA;
	u8 Divece_STA; 
	u8 offline_cnt=0;
	u8 tct=0;
	
	while(1)
	{	
		delay_ms(1);				  
		if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   
			
			if(USB_STATUS_REG&0x01)//正在写		  
			{
				printf("USB Writing...");//提示USB正在写入数据	 
			}
			if(USB_STATUS_REG&0x02)//正在读
			{
				printf("USB Reading...");//提示USB正在读出数据  		 
			}	 										  
			if(USB_STATUS_REG&0x04)printf("USB Write Err ");//提示写入错误
 
			if(USB_STATUS_REG&0x08)printf("USB Read  Err ");//提示读出错误
  
			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==CONFIGURED)
			{
				printf("USB Connected    ");//提示USB连接已经建立
				break;
			}
			else printf("USB DisConnected ");//提示USB被拔出了
			Divece_STA=bDeviceState;
		}
		
		tct++;
		if(tct==200)
		{
			tct=0;
			LED0=!LED0;//提示系统在运行
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=CONFIGURED;
			}
			else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=UNCONNECTED;//2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG=0;
		}
	}
}

int main(void)
{	
	//normal													 																						0x8000000  0x80000			0x20000000 0x10000
	//SCB->VTOR = SRAM_BASE|0x1000;																								//0x20001000 0xC000 0x2000D000  0x3000
	SCB->VTOR = FLASH_BASE|0x10000;																							//0x8010000  0x70000			0x2000D000 0x3000
	init();
  printf("ELITE STM32F103 ^_^");	

	init_usb();
	usb_stat();
	 
	while(1)
	{	

		delay_ms(1000);
		printf(" normal ");
		LED1=!LED1;
	}
}














