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
	u16 oldcount=0;				//�ϵĴ��ڽ�������ֵ
	u16 applenth=0;				//���յ���app���볤��
	int countflag=0;   
	u8  flag=0;   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	//���ڳ�ʼ��Ϊ115200
	delay_init();	   	 	//��ʱ��ʼ�� 
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
 
	printf("ELITE STM32F103 ^_^\r\n");	
	printf("Update CAN.bin file, Wait 5 seconds before executing the existing program\r\n");	
  
	while(1)
	{
	 	if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
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
 	 
		//������û�н��յ�bin�ļ�����ִ��flash�ĳ������SRAM�ĳ���
		{
			if(countflag!=-1)
			{
				countflag++;
				printf("Wait %d s\r\n", countflag);	
				delay_ms(1000);
				if(countflag==5)
				{
					countflag = -1;
					if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
					{	 
						printf("Execute flash!\r\n");	  
						iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
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
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//����FLASH����   
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
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			}else 
			{
				printf("Flash Execute err \r\n");	   
			}									   
		}			    
	}   	   
}











