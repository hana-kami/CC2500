#include "stm32f10x.h"
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "oled.h"
#include "GPIO.h"
#include "crc.h"
#include "rf_task.h"
#include "stdbool.h"
#include "string.h"
#include <stdio.h>
#include "esp8266.h"
#include "onenet.h"
#include "mqttkit.h"
bool EnableMaster = false; //EnableMaster = true ����Ϊ����ģʽ  EnableMaster = false ����Ϊ����ģʽ

#define RF_BUFFER_SIZE 64

uint8_t TXBuffer[RF_BUFFER_SIZE]; // TX buffer
uint8_t RXBuffer[RF_BUFFER_SIZE]; // RX buffer
u8 buff[30];//OLED��ʾ��������
u16 m4_value;						  //��ȼ������ֵ
u16 m6_value;						  //��ȼ������ֵ
u8 adc_value_m4;
u8 adc_value_m6;
u16 received_crc_value;
uint8_t RFstate;
void Radio_Process(void);

typedef enum
{
	RF_STATE_TX_INIT,
	RF_STATE_TX_RUNNING,
	RF_STATE_TX_DONE,

	RF_STATE_RX_INIT,
	RF_STATE_RX_RUNNING,
	RF_STATE_RX_DONE,

	RFLR_STATE_SLEEP,
} tRFLRStates;


uint16_t m4_value;						  //������ֵ
uint16_t m5_value;						//Һ�������ֵ
//�������Ĺ�����־λ
int flag;
extern volatile uint8_t rfIntRequest;
void McuInit()
{
	 delay_init();
	 LED_Init();
	 OLED_Init();
	 OLED_Clear();						//OLED����
	 OLED_ShowCHinese(0,0,23);
	 OLED_ShowCHinese(16,0,24);
	 OLED_ShowCHinese(32,0,25);
	 OLED_ShowCHinese(48,0,26);
	 OLED_ShowCHinese(64,0,27);
	 OLED_ShowCHinese(0,2,28);
	 OLED_ShowCHinese(16,2,29);
	 OLED_ShowCHinese(32,2,30);
	 OLED_ShowCHinese(48,2,31);
}

void Hardware_Init(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϿ�������������

	Usart1_Init(115200);							//����1����ӡ��Ϣ��
	Usart2_Init(115200);							//����2������ESP8266��
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
}


void SE243L_PA_Enable()//SE243L��PAͨ��
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_RESET);// CPS=0
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_SET);//CSD=1
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_SET);//CTX=1
  
  GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);//ѡ��SE2431L���߶˿�ANT2     SEL=1
//GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_RESET);//ѡ��SE2431L���߶˿�ANT1   SEL=0
}

void SE243L_LNA_Enable()//SE243L��LANͨ��
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_SET);//CPS=1
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_SET);//CSD=1
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_RESET);//CTX=0
  
GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);//ѡ��SE2431L���߶˿�ANT2     SEL=1
//GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_RESET);//ѡ��SE2431L���߶˿�ANT1   SEL=0
}

void SE243L_SLEEP()//SE243L����SLEEP
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_RESET);//CPS=0
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_RESET);//CSD=0
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_RESET);//CTX=0
  
  GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);
}
//��Ƶ��ʼ��
void RadioInit()
{
	RfSetup();
	Strobe(CCxx0x_SIDLE);	
}


void OneNet_FillBufq(char *buf)
{
    
	char text[32];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, ",;");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "MQ1,%d;", adc_value_m4);
	strcat(buf, text);	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "led2,%d;", key2_velue);
//	strcat(buf, text);
//	memset(text, 0, sizeof(text));
//	sprintf(text, "led3,%d;", key3_velue);
//	strcat(buf, text);
	

}

int main(void)
{
	unsigned short timeCount = 0;	//���ͼ������
	
	unsigned char *dataPtr = NULL;
	

//	while(OneNet_DevLink())			//����OneNET

	 McuInit();
	 RfInit();
	 RadioInit();
	 RF_INT_EXTI_Init(); 
	
	 BEEP=1;
	 LED1 = 1 ;
	 delay_ms(300);
	 BEEP=0;//ȫ������ʼ����ɣ���������һ��
		Hardware_Init();				//��ʼ����ΧӲ��
	ESP8266_Init();					//��ʼ��ESP8266
	while(OneNet_DevLink())
			delay_ms(500);
	if (EnableMaster == true)
	{	
		RFstate = RF_STATE_TX_INIT; //����ģʽ
	}
	else
	{
		RFstate = RF_STATE_RX_INIT;//����ģʽ
	}
	
   while(1)
	 {		
			
		Radio_Process();
//		 OLED_ShowNumber(8,3,timeCount,2,12);
		 ++timeCount;
		if(timeCount >= 500)									//���ͼ��5s
				{
//					char buf[128];
//	
//						memset(buf, 0, sizeof(buf));
//					OneNet_FillBufq(buf);
					OneNet_SendData(adc_value_m4,adc_value_m6);									//��������
					
					timeCount = 0;
					ESP8266_Clear();
				}		 
	 }		
 } 
void Radio_Process(void)
{
	uint16_t crc_value;
	uint16_t num_rx = 0;

	switch (RFstate)
	{
/*--------------------------����--------------------------------*/
	case RF_STATE_TX_INIT:

		SE243L_PA_Enable();//��PAͨ��
		// �����������ݴ洢�ڷ��ͻ�������
    adc_value_m4 = m4_value;
		adc_value_m6 = m6_value;
		rfIntRequest = 0;	
		TXBuffer[0] = adc_value_m4 & 0xFF; // ���ֽ�
    TXBuffer[1] = (adc_value_m4 >> 8) & 0xFF; // ���ֽ�
    TXBuffer[2] = adc_value_m6 & 0xFF; // ���ֽ�
    TXBuffer[3] = (adc_value_m6 >> 8) & 0xFF; // ���ֽ�
		crc_value = RadioComputeCRC(TXBuffer, 4, CRC_TYPE_IBM);//CRC  
		TXBuffer[4] = crc_value >> 8;
		TXBuffer[5] = crc_value;
		SendPacket(TXBuffer, TXBuffer[0]+1);
		RFstate = RF_STATE_TX_RUNNING;
		break;
	case RF_STATE_TX_RUNNING:

		while (!rfIntRequest); //�ȴ��������
			
		rfIntRequest = 0;
		RFstate = RF_STATE_TX_DONE;
		break;

	case RF_STATE_TX_DONE: //�������
		
    //LEDToggle(); //LED��˸
		RFstate = RFLR_STATE_SLEEP;
		break;
	case RFLR_STATE_SLEEP:
		
	   SE243L_SLEEP();//SE243L����SLEEP	
		 Strobe(CCxx0x_SPWD); //����͹���	
	
		 Strobe(CCxx0x_SIDLE);//����ģ��	
	   delay_ms(10);
		RFstate = RF_STATE_TX_INIT;
		break;
/*--------------------------����-----------------------------------*/
	case RF_STATE_RX_INIT: //����ģʽ��ʼ��

	  SE243L_LNA_Enable(); //�򿪽���ͨ��
	  Strobe(CCxx0x_SRX); //�������ģʽ	
		RFstate = RF_STATE_RX_RUNNING;
	
		break;
	case RF_STATE_RX_RUNNING:

	 if(rfIntRequest)//�ȴ��������
		{
			LED0 = 0;
			rfIntRequest=0;
			if(ReceivePacket(RXBuffer, RF_BUFFER_SIZE+1)) 
			{	
		    Strobe(CCxx0x_SRX); //�������ģʽ
				// �������յ�������
        adc_value_m4 = (RXBuffer[1] << 8) | RXBuffer[0];
        adc_value_m6 = (RXBuffer[3] << 8) | RXBuffer[2];
        received_crc_value = (RXBuffer[4] << 8) | RXBuffer[5];
				if (received_crc_value == RadioComputeCRC(RXBuffer, 4, CRC_TYPE_IBM)) // CRC check 
				{
					delay_ms(100); //��ʱ100ms
					LED0 = 0;
					delay_ms(100); //��ʱ100ms
					LED0 = 1;//���յ���ȷ���� LED��˸
					OLED_Clear();	
					OLED_ShowCHinese(16,0,32);
					OLED_ShowCHinese(32,0,33);
					OLED_ShowCHinese(48,0,34);
					OLED_ShowCHinese(64,0,35);
					sprintf((char *)buff,"Pos 1M_4:%d",adc_value_m4);
					OLED_ShowString(8,3,buff,12);
					
					sprintf((char *)buff,"Pos 1:M_6:%d",adc_value_m6);
					OLED_ShowString(8,4,buff,12);	
					if(adc_value_m4 > 60 || adc_value_m6 > 180){
						BEEP = 1;
						LED2 = 0;
					}else{
						BEEP = 0;
						LED2 = 1;
					}
				}			 
			}
			 RFstate = RF_STATE_RX_DONE;	
		}
		break;
	case RF_STATE_RX_DONE:	
		  RFstate = RF_STATE_RX_INIT;
		break;
	}

}

