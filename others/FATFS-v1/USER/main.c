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
 ALIENTEK��ӢSTM32������ʵ��36
 FATFS�ļ�ϵͳʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/



 int main(void)
 {	 
	 FIL fil;
	 UINT bw;
	 char buf[50];
	 
 	u32 total,free;
	u8 res=0;	    	    

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 //	usmart_dev.init(72);		//��ʼ��USMART		
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	//KEY_Init();					//��ʼ������
	//LCD_Init();			   		//��ʼ��LCD   
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��

	printf("ELITE STM32F103 ^_^\r\n");	
	printf("FATFS TEST\r\n");	
	printf("ATOM@ALIENTEK\r\n");
	printf("2020/4/1\r\n"); 

#if 0	 
	while(SD_Init())//��ⲻ��SD��
	{
		printf("SD Card Error!");
		delay_ms(500);					
		printf("Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
#endif
 	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1); 					//����SD�� 
 	res=f_mount(fs[1],"1:",1); 				//����FLASH.	
	if(res==0X0D)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		printf("Flash Disk Formatting...\r\n");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//����Flash���̵�����Ϊ��ALIENTEK
			printf("Flash Disk Format Finish\r\n");	//��ʽ�����
		}else printf("Flash Disk Format Error\r\n");	//��ʽ��ʧ��
		delay_ms(1000);
	}													    

	while(exf_getfree("1:",&total,&free))	//�õ�SD������������ʣ������
	{
		printf("FALSH Fatfs Error!\r\n");
		delay_ms(200);
		LED0=!LED0;//DS0��˸
	}
	
	    
 	printf("total%d\r\n",total);				//��ʾSD�������� MB
 	printf("free%d\r\n",free);					//��ʾSD��ʣ������ MB			
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

















