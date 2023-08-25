#include "rf_task.h"
#include "GPIO.h"

#define WRITE_BURST     0x40
#define READ_SINGLE     0x80
#define READ_BURST      0xC0

typedef struct
{
 uint8_t addr;
 uint8_t data;
}registerSetting_t;

#define PA_TABLE {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00}//1dBm
uint8_t paTable_CCxx0x[8] = PA_TABLE;
// Address Config = No address check 
// Base Frequency = 2433.000000 
// CRC Autoflush = false 
// CRC Enable = true 
// Carrier Frequency = 2433.000000 
// Channel Number = 0 
// Channel Spacing = 199.951172 
// Data Format = Normal mode 
// Data Rate = 2.39897 
// Deviation = 38.085938 
// Device Address = 0 
// Manchester Enable = false 
// Modulated = true 
// Modulation Format = 2-FSK 
// Packet Length = 255 
// Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Preamble Count = 4 
// RX Filter BW = 203.125000 
// Sync Word Qualifier Mode = 30/32 sync word bits detected 
// TX Power = 1 
// Whitening = false 
static const registerSetting_t preferredSettings[]= 
{
  {CCxx0x_IOCFG0,      0x06},//2.4(FSK)
  {CCxx0x_PKTCTRL0,    0x05},
  {CCxx0x_FSCTRL1,     0x08},
  {CCxx0x_FREQ2,       0x5D},
  {CCxx0x_FREQ1,       0x93},
  {CCxx0x_FREQ0,       0xB1},
  {CCxx0x_MDMCFG4,     0x86},
  {CCxx0x_MDMCFG3,     0x83},
  {CCxx0x_MDMCFG2,     0x03},
  {CCxx0x_DEVIATN,     0x44},
  {CCxx0x_MCSM0,       0x18},
  {CCxx0x_FOCCFG,      0x16},
  {CCxx0x_FSCAL1,      0x00},
  {CCxx0x_FSCAL0,      0x11},
};
unsigned char SPI_write(unsigned char value)
{
    unsigned char i;
    
    for(i=0; i<8; i++) 
    {
        if(value & 0x80)
        {
            RF_MOSI_HIGH();
        }
        else	
        {
            RF_MOSI_LOW();
        }
         __NOP(); 
        RF_SCK_HIGH();
        value <<= 1;
        
        RF_SCK_LOW();
    }

    return 1;
}
unsigned char SPI_read(void)
{
    unsigned char i;
    unsigned char value;
    
    for(i=0; i<8; i++) 
    { 
        RF_SCK_HIGH();
        value <<= 1;
        
        value |= RF_MISO();
        
        RF_SCK_LOW();
    }
    
    return value;
}

unsigned char ReadReg(unsigned char addr) 
{
	unsigned char value;
  unsigned char i;       
	RF_CSN_LOW();
	for(i=0; i<100; i++);
	addr |= READ_SINGLE;
	SPI_write(addr);
	value = SPI_read();
	RF_CSN_HIGH();

	return value;
}


void WriteReg(unsigned char addr, unsigned char value) 
{
	unsigned char i;
        
  RF_CSN_LOW();
	for(i=0; i<100; i++);
	addr &= ~READ_SINGLE;
	SPI_write(addr);
	SPI_write(value);
	RF_CSN_HIGH();
}
unsigned char ReadStatus(unsigned char addr) 
{
	unsigned char value;
  unsigned char i;
        
	RF_CSN_LOW();
	for(i=0; i<100; i++);
	addr |= READ_BURST;
	SPI_write(addr);
	value = SPI_read();
	RF_CSN_HIGH();
        
	return value;
}

void ReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count) 
{
	unsigned char i;
	
  RF_CSN_LOW();
	for(i=0; i<100; i++);
	addr |= READ_BURST;
	SPI_write(addr);
	for(i=0; i<count; i++) 
	{
	  buffer[i] = SPI_read();
	}
	RF_CSN_HIGH(); 
}

void WriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count)
{
	unsigned char i;
	
        RF_CSN_LOW();
	for(i=0; i<100; i++);
	addr &= ~READ_SINGLE;
        addr |= WRITE_BURST;
	SPI_write(addr);
	for(i=0; i<count; i++) 
	{
	  SPI_write(buffer[i]);
	}
	RF_CSN_HIGH();
}

void Strobe(unsigned char strobe) 
{
	unsigned char i;
        
  RF_CSN_LOW();
	for(i=0; i<100; i++);
	SPI_write(strobe);
	RF_CSN_HIGH();
}

void halRfWriteRfSettings(void) 
{
	uint16_t i; 
  for(i = 0; i < sizeof(preferredSettings)/sizeof(registerSetting_t); i++) 
  {
    WriteReg( preferredSettings[i].addr, preferredSettings[i].data);
  }

}
void POWER_UP_RESET_CCxx00(void) 
{
  unsigned int i;
  
  RF_CSN_HIGH();
  for(i=0; i<10; i++);  
  RF_CSN_LOW();
  for(i=0; i<10; i++);  
  RF_CSN_HIGH();
  for(i=0; i<200; i++);  

  Strobe(CCxx0x_SRES);
  for(i=0; i<2000; i++);
}
  uint8_t value;
void RfSetup(void)
{ 
  POWER_UP_RESET_CCxx00();
  halRfWriteRfSettings();
  Strobe(CCxx0x_SIDLE);
  Strobe(CCxx0x_SFRX);
  Strobe(CCxx0x_SFTX);
  Strobe(CCxx0x_SCAL);
  WriteBurstReg(CCxx0x_PATABLE, paTable_CCxx0x, sizeof(paTable_CCxx0x));//Write PATABLE

  if(ReadReg(preferredSettings[0].addr) != preferredSettings[0].data)while(1);//检查SPI是否正常通信
}
unsigned char SendPacket(unsigned char *txBuffer, unsigned char length) 
{
  Strobe(CCxx0x_SIDLE);
  Strobe(CCxx0x_SFTX);
  WriteBurstReg(CCxx0x_TXFIFO, txBuffer, length);
  Strobe(CCxx0x_STX);
  
  return 1;
}
unsigned char RSSI_dec;
unsigned char ReceivePacket(unsigned char *rxBuffer, unsigned char max_length) 
{
  unsigned char packetLength;	
  unsigned char status[2];
  Strobe(CCxx0x_SIDLE);	
  
  packetLength = ReadStatus(CCxx0x_RXBYTES);

  if(packetLength & BYTES_IN_RXFIFO) 
  {			
    packetLength = ReadReg(CCxx0x_RXFIFO);
    if(packetLength < max_length)
    {
      rxBuffer[0] = packetLength;
      ReadBurstReg(CCxx0x_RXFIFO, rxBuffer+1, packetLength); 
      ReadBurstReg(CCxx0x_RXFIFO, status, 2);
      RSSI_dec=status[RSSI];     			
      Strobe(CCxx0x_SFRX);				
      return !!(status[LQI] & CRC_OK);
    }
    else
    {
      Strobe(CCxx0x_SFRX);
      return 0;
    }
  } 
  else 
  {
    Strobe(CCxx0x_SFRX);
    return 0;
  }
}

