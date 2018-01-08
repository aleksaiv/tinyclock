// We are using thirty-party library for working with DS3231
#include <DS3231.h>
#include <Wire.h>

#include <stdlib.h>

// Our library for working with matrix display
#include "minimatrix.h"
// Library for green-mode functions
#include "lowpower.h"

boolean dots=false;

MiniMatrix lc=MiniMatrix(3,4,5,4); // using pings 3,4,5 for matrix display pins (DIN,CLK,CS)
DS3231 Clock; // DS3231 connected to pins 0, 2 on digispark 


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

  // setting up WatchDogTimer to 0.5 sec (save the power)
  setup_watchdog(5);

}

/*
 * Main loop
 */
void loop() { 
  char buf[100];
  bool h12,PM;

  /* Get the time and temperature from DS3231 chip and put them to the formatted string */
  sprintf(buf, "%02d%02d%02d%02d",
          (int16_t)Clock.getHour(h12,PM),
          (int16_t)Clock.getMinute(),
          (int16_t)Clock.getSecond(),
          (int16_t)Clock.getTemperature());
  
  /* output hours */
  lc.printChar(0, buf[0]);
  lc.printChar(4, buf[1]);
  /* output minutes */
  lc.printChar(10, buf[2]);
  lc.printChar(14, buf[3]);
  /* output temperature */
  lc.printChar(21, buf[6]);
  lc.printChar(25, buf[7]);
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
  /* make changes visible on matrix display */
  lc.showBuffer();
  /* going to sleep */
  system_sleep(); 
}


