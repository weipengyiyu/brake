#ifndef __BRAKE_H__
#define __BRAKE_H__

#include "sys.h"

void recv_heart(u8 *buf);
void urgency_stop(void);
void key_brake(void);
void brake(void);
int is_brake(void);
void is_flag(void);
char *reverse(char *s);
char *my_itoa(int n);
void USART_Send(int *Data);
void record(void);

#endif
