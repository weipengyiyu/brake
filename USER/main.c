#include "main.h"

int main(void)
{	
	//SCB->VTOR = SRAM_BASE|0x1000;								//0x20001000 0xC000 0x2000D000  0x3000
	SCB->VTOR = FLASH_BASE|0x10000;								//0x8010000  0x70000
	PeriphInit();																																	//�豸��ʼ��
	start();
	
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
}		

void start(void)
{
	USART_SendData(USART1, 's');
	delay_us(100);
	USART_SendData(USART1, 't');
	delay_us(100);
	USART_SendData(USART1, 'a');
	delay_us(100);
	USART_SendData(USART1, 'r');
	delay_us(100);
	USART_SendData(USART1, 't');
	delay_us(100);
	USART_SendData(USART1, ' ');
}
