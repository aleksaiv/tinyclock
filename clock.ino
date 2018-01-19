
// We are using thirty-party libraries for working with DS3231 and 315MHz receiver/transmitter
#include <Manchester.h>
#include <DS3231.h>

// Using our library for working with matrix display
#include "minimatrix.h"

// Library for green-mode functions
#include "lowpower.h"

boolean dots=false;

MiniMatrix lc=MiniMatrix(3,4,5,4); // using pings 3,4,5 for matrix display pins (DIN,CLK,CS)
DS3231 Clock; // DS3231 connected to pins 0, 2 on digispark 

#define RX_PIN 1 
#define BUFFER_SIZE 10
uint8_t buffer[BUFFER_SIZE];

void setup() {
  // Start the I2C interface
  Wire.begin();
  
  /* Power up matrix display */
  lc.shutdown(false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0);
  /* and clear the display */
  lc.clearDisplay();
  /* using mini font 5x3 pixels */
  lc.setFont(font);
  /* Initializing receiver (use pin 1) */
  //rf.RXInit(RX_PIN);
  man.setupReceive(RX_PIN, MAN_1200);
  man.beginReceiveArray(BUFFER_SIZE, buffer);
  
  // setting up WatchDogTimer to 0.5 sec (save the power)
  setup_watchdog(5);

}

/*
 * Main loop
 */
void loop() { 
  bool h12,PM;
  int8_t h, m, s, t;

  h=(int16_t)Clock.getHour(h12,PM);
  m=(int16_t)Clock.getMinute();
  s=(int16_t)Clock.getSecond();
  t=(int16_t)Clock.getTemperature();

  /* output hours */
  lc.printChar(0, '0'+(h/10));
  lc.printChar(4, '0'+(h%10));
  /* output minutes */
  lc.printChar(10, '0'+(m/10));
  lc.printChar(14, '0'+(m%10));
  /* output temperature */
  lc.printChar(21, '0'+(t/10));
  lc.printChar(25, '0'+(t%10));
 
  /* draw temperature sign */
  lc.setLed(3,7,5,true);
  lc.setLed(3,7,6,true);
  lc.setLed(3,7,7,true);
  lc.setLed(3,6,5,true);
  lc.setLed(3,6,7,true);
  lc.setLed(3,5,5,true);
  lc.setLed(3,5,6,true);
  lc.setLed(3,5,7,true);
  /* blink by double dots */
  dots=!dots;
  lc.setLed(1,6,0,dots);
  lc.setLed(1,4,0,dots);
  
  // checking for new received message
  if (man.receiveComplete()) { 
    // checking for correct header
    if(buffer[0] == 5){ 
      // byte 1 - CRC
      if(buffer[1] == (buffer[2] + buffer[3] + buffer[4]) & 0xFF) { 
        
        // blink dot at lower right corner
        lc.setLed(3,0,7,true);
        
        // setting up a new time 
        Clock.setHour(buffer[2]);
        Clock.setMinute(buffer[3]);
        Clock.setSecond(buffer[4]);

        // blink screen (invert)
        lc.invert();
        /*for(int i = 0; i < 8; i++)
          for(int j = 0; j< 32; j++)
            if(lc.getLed(i,j))
              lc.setLed((j/8),i,j%8,false);
            else 
              lc.setLed((int)(j/8),i,j%8,true);*/
        lc.showBuffer();
        delay(250);
        lc.invert();
        /*for(int i = 0; i < 8; i++)
          for(int j = 0; j< 32; j++)
            if(lc.getLed(i,j))
              lc.setLed((j/8),i,j%8,false);
            else 
              lc.setLed((int)(j/8),i,j%8,true);*/
        lc.showBuffer();
        delay(100);
      }
    } else {
      lc.setLed(3, 0, 7, false);
    }
    
    //start listening for next message right
    man.beginReceiveArray(BUFFER_SIZE,buffer); 
  }

  /* make a changes visible on the display */
  lc.showBuffer();
  /* going to sleep */
  delay(500);
  system_sleep(); 
}


