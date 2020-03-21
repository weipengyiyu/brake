#include "main.h"

static int recv_total = 0;
static int flag = 1;

//收到心跳 统计信息（暂时用于统计包数和打印）
void recv_heart(u8 *buf)
{	
	//int i;
	recv_total++;
	
	//向串口发送消息
#if 0
	for(i=0;i<8;i++)
	{
		USART_SendData(USART1, buf[i]);		//向串口1发送数据
	}
#endif
	
	printf("%d ", recv_total);					//这是阻塞发送？串口发送的字符显示没事调试好
}

void urgency_stop(void)
{
	int temp = 0;
	u8 msg[8] = {0};
	
	//填充急刹车（刹车量0~255）
	msg[5] = 255 & 0xff;
	msg[6] = (255>>8) & 0xff;
	msg[7] = 2;
	
	if(flag)
	{
		//返回值:0,成功;其他,失败;
		temp = Can_Send_Msg(msg,8);
		if(temp)
		{
			printf("  stop Failed  temp=%d  ", temp);		
		}
		else
		{
			printf("  stop OK  temp=%d  ", temp);
		}
	}
}

void key_brake(void)
{
	u8 key;
	key=KEY_Scan(0);
	if(key == KEY0_PRES)										//KEY0_PRES按下
	{
		flag = 0;															//保证退出急停模式，不再进入urgency_stop
		
		printf("  exit OK    ");
	}
	else if(key == KEY1_PRES)										//KEY1_PRES按下
	{
		flag = 1;															//
		
		printf("  start OK  ");
	}
}



