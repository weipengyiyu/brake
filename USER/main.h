#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "can.h" 
#include "timer.h"
#include "brake.h"
#include "wdg.h"
#include "rtc.h"
#include "stmflash.h"
#include "iwdg.h"
#include "w25qxx.h"
#include "spi.h"
#include "ff.h"  
#include "exfuns.h"  
#include "malloc.h" 

void PeriphInit(void);

#endif
