/*
* Test basic functionality of the RF24 transmitter (connected to 
* what appears to be an unofficial clone of "Arduino Nano")
*  -uses RF24 library version 1.3.0 (vs 1.1.6)
* - make sure to select ATmega328P (old bootloader) when uploading to the Nano
*/
#include <SPI.h>
#include <RF24.h>
#include "printf.h"

/* 
* Configure nRF24L01 radio module on SPI bus plus pins 8 for CE & 9 for CS 
* arguments are (ce, cs), use transmit address of 0xF0F0F0F0E1LL
*/
RF24 radio(9,10);
const uint64_t _transmit_pipe = 0xF0F0F0F0E1LL;
uint16_t _index;

/*
* run once at startup, shows the RF modules status. A healthy module looks like this
* 
* 
* STATUS        = 0x0e RX_DR=0 TX_DS=0 MAX_RT=0 RX_P_NO=7 TX_FULL=0
* RX_ADDR_P0-1  = 0xf0f0f0f0e1 0xc2c2c2c2c2
* RX_ADDR_P2-5  = 0xc3 0xc4 0xc5 0xc6
* TX_ADDR       = 0xf0f0f0f0e1
* RX_PW_P0-6    = 0x20 0x00 0x00 0x00 0x00 0x00
* EN_AA         = 0x3f
* EN_RXADDR     = 0x03
* RF_CH         = 0x4c
* RF_SETUP      = 0x03
* CONFIG        = 0x0e
* DYNPD/FEATURE = 0x3f 0x04
* Data Rate     = 1MBPS
* Model         = nRF24L01+
* CRC Length    = 16 bits
* PA Power      = PA_LOW
* 
*/
void setup(void)
{
  Serial.begin(9600);
  printf_begin();
  radio.begin(); 
  radio.openWritingPipe(_transmit_pipe); 
  radio.enableDynamicPayloads();
  radio.setPALevel(RF24_PA_LOW);   
  radio.stopListening();

  printf("RF24 module status:\n");
  radio.printDetails();
}

/*
* run constantly in the background
*/
void loop(void)
{
  char buffer[100]; 
  sprintf(buffer, "Ping %d", _index++);  
  radio.write(&buffer, strlen(buffer));
  Serial.println(buffer);
  delay(1000);
}

