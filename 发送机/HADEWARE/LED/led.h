#ifndef __LED_H
#define __LED_H	 
#include "sys.h"


#define LED0 PBout(8)    //��
#define LED0_ON		1
#define LED0_OFF	0
#define LED2 PBout(9)    //��
#define LED2_ON		1
#define LED2_OFF	0
#define LED1 PCout(13)   //��Сϵͳ��İ��ص�
#define LED1_ON		1
#define LED1_OFF	0
#define BEEP PBout(3)    // �������ӿ�
#define BEEP_ON  0       //S8550������ IO�ڵ͵�ƽ��ͨ   C��E
#define BEEP_OFF 1       //S8550������ IO�ڸߵ�ƽ����ͨ


void LED_Init(void);    //�ƺͲ��������IO�ڳ�ʼ��

#endif
