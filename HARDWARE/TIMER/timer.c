#include "main.h"

static int enter_brake = 0;

void TIM1_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;									
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 
	TIM_Cmd(TIM1, ENABLE); 
}

void TIM1_UP_IRQHandler(void) 
{
	u8 canbuf[8] = {0};
	int temp = 0;
	
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); 
		LED1=!LED1; 
		IWDG_Feed();

		//判断是否收到心跳包
#if 1
		temp = Can_Receive_Msg(canbuf);
		if(temp == 8)
		{
			enter_brake++;
			if(enter_brake > 5)								//正确收到5个心跳包，才正常触发
			{
				enter_brake=5;
			}
			
			if(enter_brake == 5)
			{
				is_flag();
				brake();												//正常接收心跳包,进入急停模式
				recv_heart(canbuf);
			}
		}
		else if((temp != 8) && (enter_brake == 5))			
		{	
			enter_brake = 0;
			urgency_stop();													//发送急停命令
		}

#endif
	}
}












