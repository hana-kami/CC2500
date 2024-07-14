/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"
#include "LED.h"
//硬件驱动
#include "usart.h"
#include "delay.h"
#include "oled.h"
//C库
#include <string.h>
#include <stdio.h>


//#define PROID		"582034"//产品ID

//#define AUTH_INFO	"version=2018-10-31&res=products%2F582034%2Fdevices%2F123&et=1747464862&method=md5&sign=XUXVkKFNwlqHlZG0qMc%2BBw%3D%3D"//鉴权信息

//#define DEVID		"2166661658"//设备id

#define PROID		"582247"//产品ID

#define AUTH_INFO	"123456"//鉴权信息

#define DEVID		"1058643508"//设备id


extern unsigned char esp8266_buf[128];


//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                        , PROID, AUTH_INFO, DEVID);
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(500);									//等待平台响应
		UsartPrintf(USART_DEBUG, "WARN\r\n");
		if(dataPtr != NULL)
		{
			UsartPrintf(USART_DEBUG, "WARN2\r\n");
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				UsartPrintf(USART_DEBUG, "WARN3\r\n");
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
unsigned short int key1_velue = 0;    //开关值
unsigned short int key2_velue = 0;    //开关值
u8 key3_velue = 0;    //开关值
u8 key4_velue = 0;    //开关值
u8 key5_velue = 0;    //开关值
u8 key6_velue = 0;    //开关值
u8 key7_velue = 0;    //开关值
u8 key8_velue = 0;    //开关值
unsigned char OneNet_FillBuf(char *buf)
{
    
	char text[32];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, ",;");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "MQ1,%d;", key1_velue);
	strcat(buf, text);	
	memset(text, 0, sizeof(text));
	sprintf(text, "MQ2,%d;", key2_velue);
	strcat(buf, text);
//	memset(text, 0, sizeof(text));
//	sprintf(text, "led3,%d;", 23);
//	strcat(buf, text);
	
	return strlen(buf);

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(unsigned short int MQSUM,unsigned short MQSUM1)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
//	
	short body_len = 0, i = 0;
	key1_velue = MQSUM;
	key2_velue = MQSUM1;
//	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
//	
	memset(buf, 0, sizeof(buf));
	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
//  body_len = strlen(buf);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);

			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = MQTT_UnPacketRecv(cmd);

	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, ':');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//转为数值形式
//		if(strstr((char *)req_payload, "key1"))		//搜索"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			key1_velue = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led1",3);
//			LED = num;
//		}
//		if(strstr((char *)req_payload, "key2"))		//搜索"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			OLED_Clear();
//			OLED_ShowString(0,0,"led2",3);
//			LED1 = num;
//		}
//		if(strstr((char *)req_payload, "key3"))		//搜索"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED2 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led3",3);
//		}
//		if(strstr((char *)req_payload, "key4"))		//搜索"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED3 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led4",3);
//		}
//		if(strstr((char *)req_payload, "key5"))		//搜索"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			OLED_Clear();
//			OLED_ShowString(0,0,"led5",3);
//			LED4 = num;
//		}
//		if(strstr((char *)req_payload, "key6"))		//搜索"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED5 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led6",3);
//		}
//		if(strstr((char *)req_payload, "key7"))		//搜索"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED6 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led7",3);
//		}
//		if(strstr((char *)req_payload, "key8"))		//搜索"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED7 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led8",3);
//		}
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
