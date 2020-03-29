#include "main.h"

#define SIZE sizeof(TEXT_Buffer)		//数组长度
#define FLASH_SAVE_ADDR  0X08070000		//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

static int recv_cnt = 0;
static int flag = 1;
static int brake_flag = 1;								//没有发送急停
static int record_flag = 0;								//记录标记位


//收到心跳 统计信息（暂时用于统计包数和打印的标记位）
void recv_heart(u8 *buf)
{	
	if(flag)
	{
		record_flag = 1;
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

void record(void)
{
	u16 TEXT_Buffer[16] = {0};
	u16 datatemp[16] = {0};
	
	if(record_flag != 0)
	{
		recv_cnt++;

		TEXT_Buffer[0] = recv_cnt;
		TEXT_Buffer[1] = ' ';
		TEXT_Buffer[2] = calendar.w_year;
		TEXT_Buffer[3] = '-';
		TEXT_Buffer[4] = calendar.w_month;
		TEXT_Buffer[5] = '-';
		TEXT_Buffer[6] = calendar.w_date;
		TEXT_Buffer[7] = '-';
		TEXT_Buffer[8] = calendar.hour;
		TEXT_Buffer[9] = ':';
		TEXT_Buffer[10] = calendar.min;
		TEXT_Buffer[11] = ':';
		TEXT_Buffer[12] = calendar.sec;

		STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)TEXT_Buffer,SIZE);
		STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,SIZE);

		printf(" %d-%d-%d-%d:%d:%d ", datatemp[2], 	
		datatemp[4], datatemp[6], datatemp[8], datatemp[10], datatemp[12]);

		printf(" cnt%d ", datatemp[0]);
		//printf(" cnt%d ", recv_cnt);
	
		record_flag = 0;
	}
	

}

