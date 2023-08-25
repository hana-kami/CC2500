#include <stdint.h>
#include "delay.h"

void Delay_Us (uint32_t delay)
{
  uint8_t i=0;
  uint32_t j=0;
  for(i=0;i<delay;i++)
  {
    for(j=0;j<8;j++);
  }
}


void Delay_Ms(uint32_t delay )
{
  uint32_t i=0;
  uint32_t j=0;
  
  for(i=0;i<delay;i++)
  {
    for(j=0;j<6810;j++);
  }
}



