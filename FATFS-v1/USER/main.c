#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
 
/************************************************
 ALIENTEK精英STM32开发板实验36
 FATFS文件系统实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/



 int main(void)
 {	 
	 FIL fil;
	 UINT bw;
	 char buf[50];
	 
 	u32 total,free;
	u8 res=0;	    	    

	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 //	usmart_dev.init(72);		//初始化USMART		
 	LED_Init();		  			//初始化与LED连接的硬件接口
	//KEY_Init();					//初始化按键
	//LCD_Init();			   		//初始化LCD   
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池

	printf("ELITE STM32F103 ^_^\r\n");	
	printf("FATFS TEST\r\n");	
	printf("ATOM@ALIENTEK\r\n");
	printf("2020/4/1\r\n"); 

#if 0	 
	while(SD_Init())//检测不到SD卡
	{
		printf("SD Card Error!");
		delay_ms(500);					
		printf("Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
#endif
 	exfuns_init();							//为fatfs相关变量申请内存				 
  	f_mount(fs[0],"0:",1); 					//挂载SD卡 
 	res=f_mount(fs[1],"1:",1); 				//挂载FLASH.	
	if(res==0X0D)//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
		printf("Flash Disk Formatting...\r\n");	//格式化FLASH
		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//设置Flash磁盘的名字为：ALIENTEK
			printf("Flash Disk Format Finish\r\n");	//格式化完成
		}else printf("Flash Disk Format Error\r\n");	//格式化失败
		delay_ms(1000);
	}													    

	while(exf_getfree("1:",&total,&free))	//得到SD卡的总容量和剩余容量
	{
		printf("FALSH Fatfs Error!\r\n");
		delay_ms(200);
		LED0=!LED0;//DS0闪烁
	}
	
	    
 	printf("total%d\r\n",total);				//显示SD卡总容量 MB
 	printf("free%d\r\n",free);					//显示SD卡剩余容量 MB			
	#if 0
	f_open(&fil, "1:/msg.txt", FA_CREATE_ALWAYS|FA_WRITE);
	f_write(&fil, "l want the world!\r\n", 30, &bw);
	f_close(&fil);
	
		f_open(&fil, "1:/msg.txt", FA_READ);
	f_read(&fil, buf, 50, &bw);
	f_close(&fil);
	
	printf(" %s\n ", buf);
	#endif
	while(1)
	{
		delay_ms(200);		 			   
		LED0=!LED0;
	} 
}

















