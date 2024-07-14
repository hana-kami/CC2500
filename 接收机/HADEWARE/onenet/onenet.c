/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"
#include "LED.h"
//Ӳ������
#include "usart.h"
#include "delay.h"
#include "oled.h"
//C��
#include <string.h>
#include <stdio.h>


//#define PROID		"582034"//��ƷID

//#define AUTH_INFO	"version=2018-10-31&res=products%2F582034%2Fdevices%2F123&et=1747464862&method=md5&sign=XUXVkKFNwlqHlZG0qMc%2BBw%3D%3D"//��Ȩ��Ϣ

//#define DEVID		"2166661658"//�豸id

#define PROID		"582247"//��ƷID

#define AUTH_INFO	"123456"//��Ȩ��Ϣ

#define DEVID		"1058643508"//�豸id


extern unsigned char esp8266_buf[128];


//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                        , PROID, AUTH_INFO, DEVID);
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(500);									//�ȴ�ƽ̨��Ӧ
		UsartPrintf(USART_DEBUG, "WARN\r\n");
		if(dataPtr != NULL)
		{
			UsartPrintf(USART_DEBUG, "WARN2\r\n");
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				UsartPrintf(USART_DEBUG, "WARN3\r\n");
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
unsigned short int key1_velue = 0;    //����ֵ
unsigned short int key2_velue = 0;    //����ֵ
u8 key3_velue = 0;    //����ֵ
u8 key4_velue = 0;    //����ֵ
u8 key5_velue = 0;    //����ֵ
u8 key6_velue = 0;    //����ֵ
u8 key7_velue = 0;    //����ֵ
u8 key8_velue = 0;    //����ֵ
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
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(unsigned short int MQSUM,unsigned short MQSUM1)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
//	
	short body_len = 0, i = 0;
	key1_velue = MQSUM;
	key2_velue = MQSUM1;
//	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
//	
	memset(buf, 0, sizeof(buf));
	body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
//  body_len = strlen(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//���
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);

			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
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
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ظ�����
					MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, ':');					//����'}'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
//		if(strstr((char *)req_payload, "key1"))		//����"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			key1_velue = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led1",3);
//			LED = num;
//		}
//		if(strstr((char *)req_payload, "key2"))		//����"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			OLED_Clear();
//			OLED_ShowString(0,0,"led2",3);
//			LED1 = num;
//		}
//		if(strstr((char *)req_payload, "key3"))		//����"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED2 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led3",3);
//		}
//		if(strstr((char *)req_payload, "key4"))		//����"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED3 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led4",3);
//		}
//		if(strstr((char *)req_payload, "key5"))		//����"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			OLED_Clear();
//			OLED_ShowString(0,0,"led5",3);
//			LED4 = num;
//		}
//		if(strstr((char *)req_payload, "key6"))		//����"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED5 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led6",3);
//		}
//		if(strstr((char *)req_payload, "key7"))		//����"key"
//		{
//			UsartPrintf(USART_DEBUG, "key1_velue: %d\r\n", num);
//			LED6 = num;
//			OLED_Clear();
//			OLED_ShowString(0,0,"led7",3);
//		}
//		if(strstr((char *)req_payload, "key8"))		//����"key"
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
