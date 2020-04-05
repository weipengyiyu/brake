#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"
 
 
/************************************************
 ALIENTEK��ӢSTM32������ʵ��42
 IAPʵ�� Bootloader V1.0 ���� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


int main(void)
{		
	u8 key;
	u16 oldcount=0;				//�ϵĴ��ڽ�������ֵ
	u16 applenth=0;				//���յ���app���볤��
	int countflag=0;   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	//���ڳ�ʼ��Ϊ115200
	delay_init();	   	 	//��ʱ��ʼ�� 
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
 
	printf("ELITE STM32F103 ^_^\r\n");	
	printf("update CAN.bin file, Wait 5 seconds before executing the existing program\r\n");	
  
	while(1)
	{
	 	if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",applenth);
				printf("input: \r\nkey_up-copy \r\nkey1-flash \r\nkey0-sram\r\n");	
			}else oldcount=USART_RX_CNT;	
			countflag = -1;
		}

		delay_ms(10);
 	 
		//������û�н��յ�bin�ļ�����ִ��flash�ĳ������SRAM�ĳ���
		{
			if(countflag!=-1)
			{
				countflag++;
				printf("wait %d s\r\n", countflag);	
				delay_ms(1000);
				if(countflag==5)
				{
					if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
					{	 
						printf("ִ��FLASHӦ�ó���");	  
						iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
						countflag = -1;
					}
					
					if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x20000000)//�ж��Ƿ�Ϊ0X20XXXXXX.
					{	
						printf("ִ��SRAMӦ�ó���");	
						iap_load_app(0X20001000);//SRAM��ַ
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
				printf("��ʼ���¹̼�...\r\n");	
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//����FLASH����   
					printf("�̼��������!\r\n");	
				}else 
				{  
					printf("��FLASHӦ�ó���!\r\n");
				}
 			}else 
			{
				printf("û�п��Ը��µĹ̼�!\r\n");
			}									 
		} 
		if(key==KEY1_PRES)
		{
			delay_ms(500);
			printf("��ʼִ��FLASH�û����� ");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			}else 
			{
				printf("��FLASHӦ�ó���,�޷�ִ�� ");	   
			}									   
		}
		if(key==KEY0_PRES)
		{
			delay_ms(500);
			printf("��ʼִ��SRAM�û����� ");
			if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x20000000)//�ж��Ƿ�Ϊ0X20XXXXXX.
			{	 
				iap_load_app(0X20001000);//SRAM��ַ
			}else 
			{
				printf("��SRAMӦ�ó���,�޷�ִ��!");   
			}									 	 
		}				    
	}   	   
}











