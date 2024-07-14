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

bool EnableMaster = true; //EnableMaster = true 设置为发射模式  EnableMaster = false 设置为接收模式

#define RF_BUFFER_SIZE 64

uint8_t TXBuffer[RF_BUFFER_SIZE]; // TX buffer
uint8_t RXBuffer[RF_BUFFER_SIZE]; // RX buffer
u8 buff[30];//OLED显示缓存数组
uint8_t RFstate;
u16 m4_value;						  //可燃气体检测值
u16 m6_value;						  //可燃气体检测值
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


uint16_t m4_value;						  //甲烷检测值
uint16_t m5_value;						//液化气检测值
//蜂鸣器的工作标志位
int flag;
extern volatile uint8_t rfIntRequest;
void McuInit()
{
	 delay_init();
	 LED_Init();
	 Adc1_Channe_Init();			//ADC通道初始化
}
void SE243L_PA_Enable()//SE243L打开PA通道
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_RESET);// CPS=0
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_SET);//CSD=1
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_SET);//CTX=1
  
  //GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);//选择SE2431L天线端口ANT2     SEL=1
  GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_RESET);//选择SE2431L天线端口ANT1   SEL=0
}

void SE243L_LNA_Enable()//SE243L打开LAN通道
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_SET);//CPS=1
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_SET);//CSD=1
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_RESET);//CTX=0
  
  //GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);//选择SE2431L天线端口ANT2     SEL=1
  GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_RESET);//选择SE2431L天线端口ANT1   SEL=0
}

void SE243L_SLEEP()//SE243L进入SLEEP
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_RESET);//CPS=0
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_RESET);//CSD=0 
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_RESET);//CTX=0
  
  GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);
}
//射频初始化
void RadioInit()
{
	RfSetup();
	Strobe(CCxx0x_SIDLE);	
}
void Get_Data()//获取传感器数据
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
	BEEP=0;//全部都初始化完成，蜂鸣器响一下
	if (EnableMaster == true)
	{	
		RFstate = RF_STATE_TX_INIT; //发射模式
	}
	else
	{
		RFstate = RF_STATE_RX_INIT;//接收模式
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
	u16 adc_value_m4;//发送缓冲区中
	u16 adc_value_m6;
	switch (RFstate)
	{
/*--------------------------发射--------------------------------*/
	case RF_STATE_TX_INIT:

		SE243L_PA_Enable();//打开PA通道
		// 将传感器数据存储在发送缓冲区中
    adc_value_m4 = m4_value;
		adc_value_m6 = m6_value;
		rfIntRequest = 0;	
		TXBuffer[0] = adc_value_m4 & 0xFF; // 低字节
    TXBuffer[1] = (adc_value_m4 >> 8) & 0xFF; // 高字节
    TXBuffer[2] = adc_value_m6 & 0xFF; // 低字节
    TXBuffer[3] = (adc_value_m6 >> 8) & 0xFF; // 高字节
		crc_value = RadioComputeCRC(TXBuffer, 4, CRC_TYPE_IBM);//CRC  
		TXBuffer[4] = crc_value >> 8;
		TXBuffer[5] = crc_value;
		SendPacket(TXBuffer, TXBuffer[0]+1);
		RFstate = RF_STATE_TX_RUNNING;
		break;
	case RF_STATE_TX_RUNNING:

		while (!rfIntRequest); //等待发射完成
			
		rfIntRequest = 0;
		RFstate = RF_STATE_TX_DONE;
		break;

	case RF_STATE_TX_DONE: //发射完成
		LED1 = 0;
		delay_ms(100); //延时100ms
		LED1 = 1;
    //LEDToggle(); //LED闪烁
		RFstate = RFLR_STATE_SLEEP;
		break;
	case RFLR_STATE_SLEEP:
		
	   SE243L_SLEEP();//SE243L进入SLEEP	
		 Strobe(CCxx0x_SPWD); //进入低功耗	
	
		 Strobe(CCxx0x_SIDLE);//唤醒模块	
	   delay_ms(10);
		RFstate = RF_STATE_TX_INIT;
		break;
/*--------------------------接收-----------------------------------*/
	case RF_STATE_RX_INIT: //接收模式初始化

	  SE243L_LNA_Enable(); //打开接收通道
	  Strobe(CCxx0x_SRX); //进入接收模式	
		RFstate = RF_STATE_RX_RUNNING;
	
		break;
	case RF_STATE_RX_RUNNING:

	 if(rfIntRequest)//等待接收完成
		{

			rfIntRequest=0;
			if(ReceivePacket(RXBuffer, RF_BUFFER_SIZE+1)) 
			{	
		    Strobe(CCxx0x_SRX); //进入接收模式
				// 解析接收到的数据
        adc_value_m4 = (RXBuffer[1] << 8) | RXBuffer[0];
        adc_value_m6 = (RXBuffer[3] << 8) | RXBuffer[2];
        received_crc_value = (RXBuffer[4] << 8) | RXBuffer[5];
		
				if (received_crc_value == RadioComputeCRC(RXBuffer, 4, CRC_TYPE_IBM)) // CRC check 
				{
					delay_ms(100); //延时100ms
					LED1 = 0;
					delay_ms(100); //延时100ms
					LED1 = 1;//接收到正确数据 LED闪烁
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

