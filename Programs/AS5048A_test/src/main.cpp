#include "mbed.h"
#include <AS5048A.h>

// The sensors connection are attached to pins 5-8
As5048Spi sensor(D11, D12, D13, D10);
RawSerial pc(USBTX, USBRX,1000000); // tx, rx
Timer timer;
int timeT;

int main() {
      timer.start();
      while(1) {
            timer.reset();
            const int* angles = sensor.read_angle();
            int angle = angles[0];

            // The read angle returns the value returned over the SPI bus, including parity bit
            // pc.printf("Read result: %x\r\n", angle);

            if( As5048Spi::parity_check(angle) )
            {
                  // Convert range from 0 to 2^14-1 to 0 - 360 degrees
                  int degrees = As5048Spi::degrees(angle)/100;
                  timeT = timer.read_us();
                  // pc.printf("Parity check succesfull.\r\n");
                  pc.printf("%i,%d\r\n",degrees,timeT);
            }
            else
            {
                  pc.printf("Parity check failed.\r\n");
            }

            wait_ms(1); 
      }
}