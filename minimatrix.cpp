#include <avr/pgmspace.h>
#include <Arduino.h>
#include "minimatrix.h"

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

MiniMatrix::MiniMatrix(int dataPin, int clkPin, int csPin, int numDevices) {
      SPI_MOSI=dataPin;
      SPI_CLK=clkPin;
      SPI_CS=csPin;
      if(numDevices<=0 || numDevices>4 )
          numDevices=4;
      maxDevices=numDevices;
      pinMode(SPI_MOSI,OUTPUT);
      pinMode(SPI_CLK,OUTPUT);
      pinMode(SPI_CS,OUTPUT);
      digitalWrite(SPI_CS,HIGH);
      for(int i=0;i<32;i++) 
          status[i]=0x00;
      for(int i=0;i<maxDevices;i++) {
          spiTransfer(i,OP_DISPLAYTEST,0);
          //scanlimit is set to max on startup
          spiTransfer(i, OP_SCANLIMIT,7);
          //decode is done in source
          spiTransfer(i,OP_DECODEMODE,0);
          //we go into shutdown-mode on startup
          shutdown(i,false);
      }
      clearDisplay();
    }
    void MiniMatrix::shutdown(int addr, bool b) {
      if(addr<0 || addr>=maxDevices)
          return;
      if(b)
          spiTransfer(addr, OP_SHUTDOWN,0);
      else
          spiTransfer(addr, OP_SHUTDOWN,1);
    }
    
    void MiniMatrix::shutdown(bool b) {
      for(int addr=0;addr<maxDevices;addr++) {
        if(b)
          spiTransfer(addr, OP_SHUTDOWN,0);
        else
          spiTransfer(addr, OP_SHUTDOWN,1);
      }
    }

    void MiniMatrix::clearDisplay(){
      for(int addr=0;addr<maxDevices;addr++){
        for(int i=0;i<8;i++){
          //status[addr*8+i] = i%2 ? B10101010 : B01010101;
          spiTransfer(addr, i+1, 0); //status[addr*8+i]);
        }
      }
    }
    void MiniMatrix::setFont(byte *newfont) {
      font=newfont;
    }
    void MiniMatrix::setDigit(int x, byte digit){
      byte val;
      byte tmp;
      for(int row=0;row<8;row++) {
        val=0;
        for(int col=0;col<3;col++){
          tmp=font[(digit-'0')*3+col] >> (row);
          tmp=tmp & 0x01;
          val = val | tmp;
          val = val << 1;
        }
        if((x%2) <1){
          status[(x/2)*8+row]=(status[(x/2)*8+row] & 0x0F) | (val<<4);
        }else{
          status[(x/2)*8+row]=(status[(x/2)*8+row] & 0xF0) | val;
        }
      }
    }
    void MiniMatrix::printChar(int x, byte ch){
      byte val;
      byte tmp;
      int8_t offset;
      int8_t mask;
      for(int col=0;col<3;col++){
        offset = (x + col) / 8;
        mask = ~(128  >> ((x + col) % 8));
        for(int row=3;row<8;row++) {
          val = status[offset*8+row] & mask;
          tmp = font[(ch-'0')*3+col] >> (row);
          tmp = tmp & 0x01;
          tmp = tmp << (7 - ((x + col) % 8));
          val = val | tmp;
          status[offset*8+row] = val;
        }
      }
    }
    void MiniMatrix::showBuffer(){
      for(int row=0;row<8;row++){
        for(int addr=maxDevices-1;addr>=0;addr--){
             spiTransfer(addr,row+1, reverse(status[addr*8+row]));
        }
      }
    }
    void MiniMatrix::setIntensity(int intensity){
      for(int addr=0;addr<maxDevices;addr++){
        spiTransfer(addr, OP_INTENSITY,intensity);
      }
    }
    void MiniMatrix::setLed(int addr, int row, int column, boolean state) {
      int offset;
      byte val=0x00;
  
      if(addr<0 || addr>=maxDevices)
          return;
      if(row<0 || row>7 || column<0 || column>7)
          return;
      offset=addr*8;
      val=B10000000 >> column;
      if(state)
          status[offset+row]=status[offset+row]|val;
      else {
          val=~val;
          status[offset+row]=status[offset+row]&val;
      }
//      spiTransfer(addr, row+1,status[offset+row]);
    }
    void MiniMatrix::spiTransfer(int addr, volatile byte opcode, volatile byte data) {
      //Create an array with the data to shift out
      int offset=addr*2;
      int maxbytes=maxDevices*2;
  
      for(int i=0;i<maxbytes;i++)
          spidata[i]=(byte)0;
      //put our device data into the array
      spidata[offset+1]=opcode;
      spidata[offset]=data;
      //enable the line 
      digitalWrite(SPI_CS,LOW);
      //Now shift out the data 
      for(int i=maxbytes;i>0;i--)
          shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
      //latch the data onto the display
      digitalWrite(SPI_CS,HIGH);
    }

