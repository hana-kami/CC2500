#ifndef RF_TASK_H
#define RF_TASK_H

#include "stm32f10x.h"


//------------------------------------------------------------------------------------------------------
// CCxxx0 STROBE, CONTROL AND STATUS REGSITER
#define CCxx0x_IOCFG2       0x00        // GDO2 output pin configuration
#define CCxx0x_IOCFG1       0x01        // GDO1 output pin configuration
#define CCxx0x_IOCFG0       0x02        // GDO0 output pin configuration
#define CCxx0x_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define CCxx0x_SYNC1        0x04        // Sync word, high byte
#define CCxx0x_SYNC0        0x05        // Sync word, low byte
#define CCxx0x_PKTLEN       0x06        // Packet length
#define CCxx0x_PKTCTRL1     0x07        // Packet automation control
#define CCxx0x_PKTCTRL0     0x08        // Packet automation control
#define CCxx0x_ADDR         0x09        // Device address
#define CCxx0x_CHANNR       0x0A        // Channel number
#define CCxx0x_FSCTRL1      0x0B        // Frequency synthesizer control
#define CCxx0x_FSCTRL0      0x0C        // Frequency synthesizer control
#define CCxx0x_FREQ2        0x0D        // Frequency control word, high byte
#define CCxx0x_FREQ1        0x0E        // Frequency control word, middle byte
#define CCxx0x_FREQ0        0x0F        // Frequency control word, low byte
#define CCxx0x_MDMCFG4      0x10        // Modem configuration
#define CCxx0x_MDMCFG3      0x11        // Modem configuration
#define CCxx0x_MDMCFG2      0x12        // Modem configuration
#define CCxx0x_MDMCFG1      0x13        // Modem configuration
#define CCxx0x_MDMCFG0      0x14        // Modem configuration
#define CCxx0x_DEVIATN      0x15        // Modem deviation setting
#define CCxx0x_MCSM2        0x16        // Main Radio Control State Machine configuration
#define CCxx0x_MCSM1        0x17        // Main Radio Control State Machine configuration
#define CCxx0x_MCSM0        0x18        // Main Radio Control State Machine configuration
#define CCxx0x_FOCCFG       0x19        // Frequency Offset Compensation configuration
#define CCxx0x_BSCFG        0x1A        // Bit Synchronization configuration
#define CCxx0x_AGCCTRL2     0x1B        // AGC control
#define CCxx0x_AGCCTRL1     0x1C        // AGC control
#define CCxx0x_AGCCTRL0     0x1D        // AGC control
#define CCxx0x_WOREVT1      0x1E        // High byte Event 0 timeout
#define CCxx0x_WOREVT0      0x1F        // Low byte Event 0 timeout
#define CCxx0x_WORCTRL      0x20        // Wake On Radio control
#define CCxx0x_FREND1       0x21        // Front end RX configuration
#define CCxx0x_FREND0       0x22        // Front end TX configuration
#define CCxx0x_FSCAL3       0x23        // Frequency synthesizer calibration
#define CCxx0x_FSCAL2       0x24        // Frequency synthesizer calibration
#define CCxx0x_FSCAL1       0x25        // Frequency synthesizer calibration
#define CCxx0x_FSCAL0       0x26        // Frequency synthesizer calibration
#define CCxx0x_RCCTRL1      0x27        // RC oscillator configuration
#define CCxx0x_RCCTRL0      0x28        // RC oscillator configuration
#define CCxx0x_FSTEST       0x29        // Frequency synthesizer calibration control
#define CCxx0x_PTEST        0x2A        // Production test
#define CCxx0x_AGCTEST      0x2B        // AGC test
#define CCxx0x_TEST2        0x2C        // Various test settings
#define CCxx0x_TEST1        0x2D        // Various test settings
#define CCxx0x_TEST0        0x2E        // Various test settings

// Strobe commands
#define CCxx0x_SRES         0x30        // Reset chip.
#define CCxx0x_SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define CCxx0x_SXOFF        0x32        // Turn off crystal oscillator.
#define CCxx0x_SCAL         0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define CCxx0x_SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define CCxx0x_STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define CCxx0x_SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define CCxx0x_SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
#define CCxx0x_SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CCxx0x_SPWD         0x39        // Enter power down mode when CSn goes high.
#define CCxx0x_SFRX         0x3A        // Flush the RX FIFO buffer.
#define CCxx0x_SFTX         0x3B        // Flush the TX FIFO buffer.
#define CCxx0x_SWORRST      0x3C        // Reset real time clock.
#define CCxx0x_SNOP         0x3D        // No operation. May be used to pad strobe commands to two
                                        // bytes for simpler software.

#define CCxx0x_PARTNUM      0x30
#define CCxx0x_VERSION      0x31
#define CCxx0x_FREQEST      0x32
#define CCxx0x_LQI          0x33
#define CCxx0x_RSSI         0x34
#define CCxx0x_MARCSTATE    0x35
#define CCxx0x_WORTIME1     0x36
#define CCxx0x_WORTIME0     0x37
#define CCxx0x_PKTSTATUS    0x38
#define CCxx0x_VCO_VC_DAC   0x39
#define CCxx0x_TXBYTES      0x3A
#define CCxx0x_RXBYTES      0x3B

#define CCxx0x_PATABLE      0x3E
#define CCxx0x_TXFIFO       0x3F
#define CCxx0x_RXFIFO       0x3F


#define CRC_OK              0x80  
#define RSSI                0
#define LQI                 1
#define BYTES_IN_RXFIFO     0x7F 

#define KEY_LIGHT 1
#define RF_RX   0
#define RF_TX   1

typedef enum {
	SYMBOL_RATE_250K = 0,
	SYMBOL_RATE_100K = 1,
	SYMBOL_RATE_38_4K = 2,
	SYMBOL_RATE_10K = 3,
	SYMBOL_RATE_2_4K = 4,
} symbol_rate_t;
extern void WriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
extern void ReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count);
extern void WriteReg(unsigned char addr, unsigned char value);
extern void Strobe(unsigned char strobe);
extern unsigned char ReadReg(unsigned char addr);
extern unsigned char ReadStatus(unsigned char addr);
extern void Data_Rate(symbol_rate_t data_rate);
extern void halRfWriteRfSettings(void);
extern void RfInit(void);
extern void RfSetup(void);
extern unsigned char SendPacket(unsigned char *txBuffer, unsigned char length);
extern unsigned char ReceivePacket(unsigned char *rxBuffer, unsigned char max_length);

#endif
