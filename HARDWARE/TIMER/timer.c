#include "main.h"

void TIM1_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter=4;									//2秒
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
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); 
		LED1=!LED1; 

		//判断是否收到心跳包
#if 1
		if(Can_Receive_Msg(canbuf))
		{
			recv_heart(canbuf);
		}
		else
		{
			urgency_stop();
		}
#endif
	}
}












