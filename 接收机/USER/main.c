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
bool EnableMaster = false; //EnableMaster = true 设置为发射模式  EnableMaster = false 设置为接收模式

#define RF_BUFFER_SIZE 64

uint8_t TXBuffer[RF_BUFFER_SIZE]; // TX buffer
uint8_t RXBuffer[RF_BUFFER_SIZE]; // RX buffer
u8 buff[30];//OLED显示缓存数组
u16 m4_value;						  //可燃气体检测值
u16 m6_value;						  //可燃气体检测值
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


uint16_t m4_value;						  //甲烷检测值
uint16_t m5_value;						//液化气检测值
//蜂鸣器的工作标志位
int flag;
extern volatile uint8_t rfIntRequest;
void McuInit()
{
	 delay_init();
	 LED_Init();
	 OLED_Init();
	 OLED_Clear();						//OLED清屏
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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	Usart1_Init(115200);							//串口1，打印信息用
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
}


void SE243L_PA_Enable()//SE243L打开PA通道
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_RESET);// CPS=0
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_SET);//CSD=1
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_SET);//CTX=1
  
  GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);//选择SE2431L天线端口ANT2     SEL=1
//GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_RESET);//选择SE2431L天线端口ANT1   SEL=0
}

void SE243L_LNA_Enable()//SE243L打开LAN通道
{
  GPIO_WriteBit(RF_CPS_PORT, RF_CPS_PIN, Bit_SET);//CPS=1
  GPIO_WriteBit(RF_CSD_PORT, RF_CSD_PIN, Bit_SET);//CSD=1
  GPIO_WriteBit(RF_CTX_PORT, RF_CTX_PIN, Bit_RESET);//CTX=0
  
GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_SET);//选择SE2431L天线端口ANT2     SEL=1
//GPIO_WriteBit(ANT_SEL_PORT, ANT_SEL_PIN, Bit_RESET);//选择SE2431L天线端口ANT1   SEL=0
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
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	

//	while(OneNet_DevLink())			//接入OneNET

	 McuInit();
	 RfInit();
	 RadioInit();
	 RF_INT_EXTI_Init(); 
	
	 BEEP=1;
	 LED1 = 1 ;
	 delay_ms(300);
	 BEEP=0;//全部都初始化完成，蜂鸣器响一下
		Hardware_Init();				//初始化外围硬件
	ESP8266_Init();					//初始化ESP8266
	while(OneNet_DevLink())
			delay_ms(500);
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
			
		Radio_Process();
//		 OLED_ShowNumber(8,3,timeCount,2,12);
		 ++timeCount;
		if(timeCount >= 500)									//发送间隔5s
				{
//					char buf[128];
//	
//						memset(buf, 0, sizeof(buf));
//					OneNet_FillBufq(buf);
					OneNet_SendData(adc_value_m4,adc_value_m6);									//发送数据
					
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
			LED0 = 0;
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
					LED0 = 0;
					delay_ms(100); //延时100ms
					LED0 = 1;//接收到正确数据 LED闪烁
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

