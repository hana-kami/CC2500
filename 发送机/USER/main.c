#include "stm32f10x.h"
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "GPIO.h"
#include "oled.h"
#include "crc.h"
#include "ADC.h"
#include "rf_task.h"
#include "stdbool.h"
#include "string.h"

bool EnableMaster = true; //EnableMaster = true ����Ϊ����ģʽ  EnableMaster = false ����Ϊ����ģʽ

#define RF_BUFFER_SIZE 64

uint8_t TXBuffer[RF_BUFFER_SIZE]; // TX buffer
uint8_t RXBuffer[RF_BUFFER_SIZE]; // RX buffer
u8 buff[30];//OLED��ʾ��������
uint8_t RFstate;
u16 m4_value;						  //��ȼ������ֵ
u16 m6_value;						  //��ȼ������ֵ
u16 adc_value_m4;
u16 adc_value_m6;
u16 received_crc_value;
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
	 Adc1_Channe_Init();			//ADCͨ����ʼ��
}
void SE243L_PA_Enable()//SE243L��PAͨ��
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_RESET);// CPS=0
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_SET);//CSD=1
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_SET);//CTX=1
  
  //GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);//ѡ��SE2431L���߶˿�ANT2     SEL=1
  GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_RESET);//ѡ��SE2431L���߶˿�ANT1   SEL=0
}

void SE243L_LNA_Enable()//SE243L��LANͨ��
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_SET);//CPS=1
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_SET);//CSD=1
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_RESET);//CTX=0
  
  //GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);//ѡ��SE2431L���߶˿�ANT2     SEL=1
  GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_RESET);//ѡ��SE2431L���߶˿�ANT1   SEL=0
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
void Get_Data()//��ȡ����������
{

	m4_value  = get_Adc_Value(ADC_Channel_6)/5;
	m6_value  = get_Adc_Value(ADC_Channel_5)/5;    

}
int main(void)
{
	McuInit();	
	RfInit();
	RF_INT_EXTI_Init();
	RadioInit();
	BEEP=1;
	LED1 = 1 ;
	delay_ms(300);
	BEEP=0;//ȫ������ʼ����ɣ���������һ��
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
		Get_Data();
		delay_ms(1000);
		Radio_Process();	 		
	 }		
 } 
void Radio_Process(void)
{
	uint16_t crc_value;
	uint16_t num_rx = 0;
	u16 adc_value_m4;//���ͻ�������
	u16 adc_value_m6;
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
		LED1 = 0;
		delay_ms(100); //��ʱ100ms
		LED1 = 1;
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
					LED1 = 0;
					delay_ms(100); //��ʱ100ms
					LED1 = 1;//���յ���ȷ���� LED��˸
					OLED_Clear();	
					sprintf((char *)buff,"M_4    :%d",adc_value_m4);
					OLED_ShowString(8,1,buff,12);
					sprintf((char *)buff,"M_6    :%d",adc_value_m6);
					OLED_ShowString(8,2,buff,12);	
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

