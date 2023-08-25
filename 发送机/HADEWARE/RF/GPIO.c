#include "GPIO.h"


volatile uint8_t rfIntRequest = 0;

void RfInit(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);  // ʹ��APB2ʱ��ʹ��

    GPIO_InitStructure.GPIO_Pin = RF_CSN_PIN | RF_SPI_SCK_PIN | RF_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = RF_CSD_PIN | RF_CPS_PIN | ANT_SEL_PIN | RF_CTX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		

	
	GPIO_InitStructure.GPIO_Pin =RF_SPI_MISO_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
        
	GPIO_SetBits(GPIOB, RF_SPI_MISO_PIN );
	
	GPIO_InitStructure.GPIO_Pin =RF_GDO0_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(RF_GDO0_PORT, &GPIO_InitStructure);	
}
void  RF_INT_EXTI_Init(void)
{	
		EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;  // STM32F10xʹ�� EXTI15_10_IRQn
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);  // STM32F10xʹ�� GPIO_PortSourceGPIOB
    EXTI_InitStruct.EXTI_Line = EXTI_Line15;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    { 
        // �жϴ�����룬����������ñ�־��ִ����������
        rfIntRequest = 1; // ��������һ����־����ʾ�жϴ���

        EXTI_ClearITPendingBit(EXTI_Line15); // ����жϱ�־λ
    }
}



