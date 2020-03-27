#include "main.h"

int main(void)
{	
	PeriphInit();																																	//�豸��ʼ��
	
	LED0 = 0;
	USART_SendData(USART1, 'a');
	while(1)
	{
		LED0 = 1;
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
	//TIM1_Int_Init(4999,7199);																											//500ms
	TIM1_Int_Init(19999,7199);																											//2000ms
	CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 4, CAN_Mode_Normal);			//CAN��ʼ������ģʽ,������500Kbps  
	WWDG_Init(0x7f, 0x5f, WWDG_Prescaler_8);																			//��ʾϵͳ��������	LED0 ��ת
	RTC_Init();	  																																//RTC��ʼ��
}		

