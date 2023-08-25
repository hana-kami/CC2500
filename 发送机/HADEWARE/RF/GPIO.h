#ifndef __GPIO_H
#define __GPIO_H
#include "stm32f10x.h"


#define RF_CSN_PIN                      GPIO_Pin_10//
#define RF_CSN_PORT                     GPIOB

#define RF_SPI_SCK_PIN                  GPIO_Pin_12
#define RF_SPI_SCK_PORT                 GPIOB

#define RF_SPI_MOSI_PIN                 GPIO_Pin_11
#define RF_SPI_MOSI_PORT                GPIOB

#define RF_SPI_MISO_PIN                 GPIO_Pin_13
#define RF_SPI_MISO_PORT                GPIOB

#define RF_CTX_PIN                      GPIO_Pin_11
#define RF_CTX_PORT                     GPIOA

#define RF_CPS_PIN                      GPIO_Pin_6
#define RF_CPS_PORT                     GPIOB

#define RF_CSD_PIN                      GPIO_Pin_5
#define RF_CSD_PORT                     GPIOB

#define ANT_SEL_PIN                      GPIO_Pin_4
#define ANT_SEL_PORT                     GPIOB

#define RF_GDO0_PIN                 GPIO_Pin_15
#define RF_GDO0_PORT                GPIOA



#define RF_CSN_LOW()     GPIO_ResetBits(RF_CSN_PORT, RF_CSN_PIN)
#define RF_CSN_HIGH()    GPIO_SetBits(RF_CSN_PORT, RF_CSN_PIN)

#define RF_SCK_LOW()     GPIO_ResetBits(RF_SPI_SCK_PORT, RF_SPI_SCK_PIN)
#define RF_SCK_HIGH()    GPIO_SetBits(RF_SPI_SCK_PORT, RF_SPI_SCK_PIN)

#define RF_MOSI_LOW()     GPIO_ResetBits(RF_SPI_MOSI_PORT, RF_SPI_MOSI_PIN)
#define RF_MOSI_HIGH()    GPIO_SetBits(RF_SPI_MOSI_PORT, RF_SPI_MOSI_PIN)

#define RF_MISO()         GPIO_ReadInputDataBit(RF_SPI_MISO_PORT, RF_SPI_MISO_PIN)


void  RF_INT_EXTI_Init(void);
void RfInit(void);




#endif



