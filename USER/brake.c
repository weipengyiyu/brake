#include "main.h"

#define SIZE sizeof(TEXT_Buffer)		//���鳤��
//#define FLASH_SAVE_ADDR  0X08070000		//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)
#define SIZECHAR  19
static long int recv_cnt = 0;
static int flag = 1;
static int brake_flag = 1;								//û�з��ͼ�ͣ

//�յ����� ͳ����Ϣ����ʱ����ͳ�ư����ʹ�ӡ�ı��λ��
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
	
	//��伱ɲ����ɲ����0~255��
	/**�������֡
	*0λ��0��ʾ�Ӵ����Ͽ���1��ʾ�Ӵ����պ�
	*1λ��0��ʾŤ�أ�1��ʾת��
	*2λ��0��ʾת����1��ʾֹͣ
	*3λ��0/1����
	*4λ��0��ʾ��ת��1��ʾ��ת
	*5λ��0��ʾ������1��ʾ����ת�ؿ���
	*6λ��0��ʾ��ʻ��1��ʾɲ��
	*7λ��0��ʾ������1��ʾ����
	**/
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


