#ifndef __LED_H
#define __LED_H	 
#include "sys.h"


#define LED0 PBout(8)    //灯
#define LED0_ON		1
#define LED0_OFF	0
#define LED2 PBout(9)    //灯
#define LED2_ON		1
#define LED2_OFF	0
#define LED1 PCout(13)   //最小系统板的板载灯
#define LED1_ON		1
#define LED1_OFF	0
#define BEEP PBout(3)    // 蜂鸣器接口
#define BEEP_ON  0       //S8550三极管 IO口低电平导通   C和E
#define BEEP_OFF 1       //S8550三极管 IO口高电平不导通


void LED_Init(void);    //灯和步进电机的IO口初始化

#endif
