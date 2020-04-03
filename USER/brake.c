#include "main.h"

#define SIZE sizeof(TEXT_Buffer)		//数组长度
//#define FLASH_SAVE_ADDR  0X08070000		//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
#define SIZECHAR  19
static long int recv_cnt = 0;
static int flag = 1;
static int brake_flag = 1;								//没有发送急停

//收到心跳 统计信息（暂时用于统计包数和打印的标记位）
void recv_heart(u8 *buf)
{	
	u8 TEXT_Buffer[20] = {0};
	u8 datatemp[20] = {0};
	
	if(flag)
	{
		TEXT_Buffer[0] = recv_cnt&0xff;
		TEXT_Buffer[1] = (recv_cnt>>8)&0xff;
		TEXT_Buffer[2] = (recv_cnt>>16)&0xff;
		TEXT_Buffer[3] = (recv_cnt>>24)&0xff;
		TEXT_Buffer[4] = calendar.w_year&0xff;
		TEXT_Buffer[5] = (calendar.w_year>>8)&0xff;
		TEXT_Buffer[6] = calendar.w_month;
		TEXT_Buffer[7] = calendar.w_date;
		TEXT_Buffer[8] = calendar.hour;
		TEXT_Buffer[9] = calendar.min;
		TEXT_Buffer[10] = calendar.sec;
		TEXT_Buffer[11] = buf[0];
		TEXT_Buffer[12] = buf[1];
		TEXT_Buffer[13] = buf[2];
		TEXT_Buffer[14] = buf[3];
		TEXT_Buffer[15] = buf[4];
		TEXT_Buffer[16] = buf[5];
		TEXT_Buffer[17] = buf[6];
		TEXT_Buffer[18] = buf[7];
		
		W25QXX_Write((u8*)TEXT_Buffer,recv_cnt*SIZE,SIZECHAR);
		W25QXX_Read((u8*)datatemp,recv_cnt*SIZE,SIZECHAR);
		printf(" %d-%d-%d-%d:%d:%d ", datatemp[4]|(datatemp[5]<<8), 	
		datatemp[6], datatemp[7], datatemp[8], datatemp[9], datatemp[10]);

		printf(" %x%x%x%x%x%x%x%x ", datatemp[11], datatemp[12], datatemp[13], datatemp[14],
			datatemp[15], datatemp[16], datatemp[17], datatemp[18]);
		
		if(recv_cnt < 256)
		{
			printf(" cnt%d \r\n", datatemp[0]);
		}
		else if(recv_cnt < 65535)
		{
			printf(" cnt%d \r\n", datatemp[0]|(datatemp[1]<<8));
		}
		else
		{
			printf(" cnt%d \r\n", datatemp[0]|(datatemp[1]<<8)|(datatemp[2]<<16));
		}

		recv_cnt++;
	}			
}

void urgency_stop(void)
{
	int temp = 0;
	u8 msg[8] = {0};
	
	//填充急刹车（刹车量0~255）
	/**电机控制帧
	*0位：0表示接触器断开；1表示接触器闭合
	*1位：0表示扭矩；1表示转速
	*2位：0表示转动；1表示停止
	*3位：0/1方向
	*4位：0表示正转；1表示反转
	*5位：0表示其它；1表示油门转矩控制
	*6位：0表示行驶；1表示刹车
	*7位：0表示正常；1表示故障
	**/
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


