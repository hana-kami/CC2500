#ifndef _DELAY_H_
#define _DELAY_H_

#include<stdint.h>

extern volatile  uint32_t TickCounter;

void Delay_Us (uint32_t delay);
void Delay_Ms(uint32_t delay );


#endif
