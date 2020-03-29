#include "main.h"

#define SIZE sizeof(TEXT_Buffer)		//���鳤��
#define FLASH_SAVE_ADDR  0X08070000		//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)

static int recv_cnt = 0;
static int flag = 1;
static int brake_flag = 1;								//û�з��ͼ�ͣ
static int record_flag = 0;								//��¼���λ


//�յ����� ͳ����Ϣ����ʱ����ͳ�ư����ʹ�ӡ�ı��λ��
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
	
	//��伱ɲ����ɲ����0~255��
	msg[5] = 255 & 0xff;
	msg[6] = (255>>8) & 0xff;
	msg[7] = 2;
	
	//������뼱ͣ���Ͳ��ٷ��ͽ��գ�
	if(flag && brake_flag)
	{
		brake_flag = 0;
		flag = 0;
		//����ֵ:0,�ɹ�;����,ʧ��;
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
	if(key == KEY0_PRES)										//KEY0_PRES����
	{
		flag = 0;															//��֤�˳���ͣģʽ�����ٽ���urgency_stop
		printf("  exit OK    ");
	}
	else if(key == KEY1_PRES)										//KEY1_PRES���£�������ͣģʽ
	{
		flag = 1;														
		printf("  start OK  ");
	}
}

void brake(void)
{
	brake_flag = 1;										//��������		
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

