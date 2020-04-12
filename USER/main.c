#include "main.h"

extern u8 Max_Lun;
void init(void)
{
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�

	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
}
int main(void)
{	
	FIL fil;
	UINT bw;
	//normal													 																						0x8000000  0x80000			0x20000000 0x10000
	//SCB->VTOR = SRAM_BASE|0x1000;																								//0x20001000 0xC000 0x2000D000  0x3000
	//SCB->VTOR = FLASH_BASE|0x10000;																							//0x8010000  0x70000			0x2000D000 0x3000
	//PeriphInit();																																	//�豸��ʼ��
	init();
	printf("\r\n    brake start!   \r\n");
	
	init_usb();
	init_fatfs();
	
	f_open(&fil, "1:/msg.txt", FA_CREATE_ALWAYS|FA_WRITE);
	f_write(&fil, "l want the world", 16, &bw);
	f_write(&fil, "hello world", 11, &bw);
	f_write(&fil, "l want the world", 16, &bw);
	f_close(&fil);
	
	while(1)
	{
		key_brake();																																//����1������0�˳���ͣģʽ
	}
}

void PeriphInit(void)
{
	delay_init();	    	 																													//��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);																//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 																													//���ڳ�ʼ��Ϊ115200
	LED_Init();		  																															//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();																																		//������ʼ��
	TIM1_Int_Init(19999,7199);																										//2000ms ��ʾϵͳ��������	LED0 ��ת
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 4, CAN_Mode_Normal);			//CAN��ʼ������ģʽ,������500Kbps  
	IWDG_Init(4, 1250);																														//2s
	RTC_Init();	  																																//RTC��ʼ��
	W25QXX_Init();																																//��ʼ��SPI FLASH��IO��
	my_mem_init(SRAMIN);																													//��ʼ���ڲ��ڴ��
}		

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
 	printf("total%d\r\n",total);				
 	printf("free%d\r\n",free);				
}

void init_usb(void)
{
	u8 USB_STA;
	u8 Divece_STA; 
	u8 offline_cnt=0;
	u8 tct=0;
	
	if(SD_Init())
	{
		Max_Lun=0;											
		printf("SD Card Error!");
	}
	else //SD
	{   															  
		printf("SD Card Size:     MB");
 	  Mass_Memory_Size[1]=SDCardInfo.CardCapacity;		
	  Mass_Block_Size[1] =512;							
	  Mass_Block_Count[1]=Mass_Memory_Size[1]/Mass_Block_Size[1];
 	}
	if(W25QXX_TYPE!=W25Q128)printf("W25Q128 Error!");	
	else //SPI FLASH 
	{   															  
 	  Mass_Memory_Size[0]=1024*1024*12;	
	  Mass_Block_Size[0] =512;			
	  Mass_Block_Count[0]=Mass_Memory_Size[0]/Mass_Block_Size[0];
		printf("SPI FLASH Size:12MB");	 
	} 
	delay_ms(1800);
	USB_Port_Set(0); 	
	delay_ms(700);
	USB_Port_Set(1);	
	printf("USB Connecting...");	
  Data_Buffer=mymalloc(SRAMIN,BULK_MAX_PACKET_SIZE*2*4);	
	Bulk_Data_Buff=mymalloc(SRAMIN,BULK_MAX_PACKET_SIZE);	
 	//USB??
 	USB_Interrupts_Config();    
 	Set_USBClock();   
 	USB_Init();	    
	delay_ms(1800);
	while(1)
	{	
		delay_ms(1);				  
		if(USB_STA!=USB_STATUS_REG)
		{	 						   
			
			if(USB_STATUS_REG&0x01)
			{
				printf("USB Writing...");
			}
			if(USB_STATUS_REG&0x02)
			{
				printf("USB Reading...");
			}	 										  
			if(USB_STATUS_REG&0x04)printf("USB Write Err ");
 
			if(USB_STATUS_REG&0x08)printf("USB Read  Err ");
  
			USB_STA=USB_STATUS_REG;
		}
		
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==CONFIGURED)
			{
				printf("USB Connected    ");
				break;
			}
			else printf("USB DisConnected ");
			Divece_STA=bDeviceState;
		}
		
		tct++;
		if(tct==200)
		{
			tct=0;
			LED0=!LED0;
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;
				bDeviceState=CONFIGURED;
			}
			else
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=UNCONNECTED;
			}
			USB_STATUS_REG=0;
		}
	}
}
