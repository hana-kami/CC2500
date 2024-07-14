#ifndef _ONENET_H_
#define _ONENET_H_





_Bool OneNet_DevLink(void);

void OneNet_SendData(unsigned short MQSUM,unsigned short MQSUM1);

void OneNet_RevPro(unsigned char *cmd);

#endif
