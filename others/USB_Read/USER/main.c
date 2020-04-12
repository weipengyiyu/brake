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
 ALIENTEK��ӢSTM32������ʵ��44
 USB������ ʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
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
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�

	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
}

void init_usb(void)
{
#if 0
	if(SD_Init())
	{
		Max_Lun=0;											//SD������,���ֻ��һ������.
		printf("SD Card Error!");	//���SD������
	}
	else //SD ������
	{   															  
		printf("SD Card Size:     MB");
 	    Mass_Memory_Size[1]=SDCardInfo.CardCapacity;		//�õ�SD���������ֽڣ�����SD����������4G��ʱ��,��Ҫ�õ�����u32����ʾ
	    Mass_Block_Size[1] =512;							//��Ϊ������Init����������SD���Ĳ����ֽ�Ϊ512��,��������һ����512���ֽ�.
	    Mass_Block_Count[1]=Mass_Memory_Size[1]/Mass_Block_Size[1];

 	}
#endif
	if(W25QXX_TYPE!=W25Q128)printf("W25Q128 Error!");	//���SD������
	else //SPI FLASH ����
	{   															  
 	  Mass_Memory_Size[0]=1024*1024*12;	//ǰ12M�ֽ�
	  Mass_Block_Size[0] =512;			//����SPI FLASH�Ĳ���������СΪ512
	  Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
		printf("SPI FLASH Size:12MB");	 
	} 
	delay_ms(1800);
	USB_Port_Set(0); 	//USB�ȶϿ�
	delay_ms(700);
	USB_Port_Set(1);	//USB�ٴ����� 
	printf("USB Connecting...");	//��ʾUSB��ʼ����	 
  Data_Buffer=mymalloc(SRAMIN,BULK_MAX_PACKET_SIZE*2*4);	//ΪUSB���ݻ����������ڴ�
	Bulk_Data_Buff=mymalloc(SRAMIN,BULK_MAX_PACKET_SIZE);	//�����ڴ�
 	//USB����
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
		if(USB_STA!=USB_STATUS_REG)//״̬�ı��� 
		{	 						   
			
			if(USB_STATUS_REG&0x01)//����д		  
			{
				printf("USB Writing...");//��ʾUSB����д������	 
			}
			if(USB_STATUS_REG&0x02)//���ڶ�
			{
				printf("USB Reading...");//��ʾUSB���ڶ�������  		 
			}	 										  
			if(USB_STATUS_REG&0x04)printf("USB Write Err ");//��ʾд�����
 
			if(USB_STATUS_REG&0x08)printf("USB Read  Err ");//��ʾ��������
  
			USB_STA=USB_STATUS_REG;//��¼����״̬
		}
		
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==CONFIGURED)
			{
				printf("USB Connected    ");//��ʾUSB�����Ѿ�����
				break;
			}
			else printf("USB DisConnected ");//��ʾUSB���γ���
			Divece_STA=bDeviceState;
		}
		
		tct++;
		if(tct==200)
		{
			tct=0;
			LED0=!LED0;//��ʾϵͳ������
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB������,�����offline������
				bDeviceState=CONFIGURED;
			}
			else//û�еõ���ѯ 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=UNCONNECTED;//2s��û�յ����߱��,����USB���γ���
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














