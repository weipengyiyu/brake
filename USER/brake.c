#include "main.h"


//要写入到STM32 FLASH的字符串数组
const u8 TEXT_Buffer[]={"l want the world!"};
#define SIZE sizeof(TEXT_Buffer)		//数组长度
#define FLASH_SAVE_ADDR  0X08070000		//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

static int recv_cnt = 0;
static int flag = 1;
static int brake_flag = 1;								//没有发送急停

//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)TEXT_Buffer,SIZE);
//STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);

//收到心跳 统计信息（暂时用于统计包数和打印）
void recv_heart(u8 *buf)
{	

	if(flag)
	{
		recv_cnt++;

		//USART_SendData(USART1, USART_RX_BUF[7]);
		
		//printf(" %d-%d-%d-%d:%d:%d ", calendar.w_year, 	
		//calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);

		//printf(" cnt%d ", recv_cnt);

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
		flag = 0;
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
	else if(key == KEY1_PRES)										//KEY1_PRES按下，开启急停模式
	{
		flag = 1;														
		printf("  start OK  ");
	}
}

void brake(void)
{
	brake_flag = 1;										//正常接收		
}

int is_brake(void)
{
	if(brake_flag == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void is_flag(void)
{
		flag = 1;
}
