#include "main.h"

static int recv_total = 0;
static int flag = 1;
static int brake_flag = 1;								//没有发送急停

//收到心跳 统计信息（暂时用于统计包数和打印）
void recv_heart(u8 *buf)
{	
	if(flag)
	{
		recv_total++;
	
		printf(" %d %d %d %d %d %d tal%d ", calendar.w_year, 	
		calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, recv_total);
	}			
}

void urgency_stop(void)
{
	int temp = 0;
	u8 msg[8] = {0};
	
	//填充急刹车（刹车量0~255）
	msg[5] = 255 & 0xff;
	msg[6] = (255>>8) & 0xff;
	msg[7] = 2;
	
	//如果进入急停，就不再发送接收？
	if(flag && brake_flag)
	{
		brake_flag = 0;
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
		flag = 1;														
		printf("  start OK  ");
	}
}

void brake(void)
{
	brake_flag = 1;										//正常接收		
}

