#include "led.h"
	    
//LED IO��ʼ��
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA PB PC�˿�ʱ��
			
 	//�����İ׵�PA3�����������������������PA11 12 15��������������PA8
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_3;	
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
 GPIO_Init(GPIOB, &GPIO_InitStructure);		
	//��Ƭ���İ���LED��PC13
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
 GPIO_Init(GPIOC, &GPIO_InitStructure);		
	
 //BEEP=BEEP_OFF;         //�رշ����������
}
 
