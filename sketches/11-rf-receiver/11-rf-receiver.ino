/*
 * Test basic functionality of the RF24 receiver (connected to "Arduino/Genuino Uno")
 *  -uses RF24 library version 1.3.0 (vs 1.1.6)
 */
#include <SPI.h>
#include <RF24.h>
#include "printf.h"

/* 
* Configure nRF24L01 radio module on SPI bus plus pins 8 for CE & 9 for CS 
* arguments are (ce, cs), use receive address of 0xF0F0F0F0E1LL
*/
RF24 radio(9,10);
const uint64_t _receive_pipe = 0xF0F0F0F0E1LL;


/*
* run once at startup, shows the RF module status. A healthy module looks like this
* 
* STATUS        = 0x0e RX_DR=0 TX_DS=0 MAX_RT=0 RX_P_NO=7 TX_FULL=0
* RX_ADDR_P0-1  = 0xf0f0f0f0e1 0x4141417852
* RX_ADDR_P2-5  = 0xc3 0xc4 0xc5 0xc6
* TX_ADDR       = 0xe7e7e7e7e7
* RX_PW_P0-6    = 0x20 0x20 0x00 0x00 0x00 0x00
* EN_AA         = 0x3f
* EN_RXADDR     = 0x03
* RF_CH         = 0x4c
* RF_SETUP      = 0x03
* CONFIG        = 0x0f
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
  radio.openReadingPipe(0,_receive_pipe); 
  radio.enableDynamicPayloads() ;
  radio.setPALevel(RF24_PA_LOW); 
  radio.startListening();

  printf("RF24 module status:\n");
  radio.printDetails();
}

/*
* run constantly in the background
*/
void loop(void)
{
  while (radio.available()) {    
    char buff[100] = {0};
    int size = radio.getDynamicPayloadSize();
    radio.read(buff,size);
    Serial.println(buff);
  } 
}

