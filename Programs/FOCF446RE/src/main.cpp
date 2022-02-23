#include "mbed.h"
#include <AS5048A.h>
#include "TrigonometricTable.h"
As5048Spi sensor(D11, D12, D13, D10);
RawSerial pc(USBTX, USBRX,1000000); // tx, rx
Timer timer;
int timeT;
int s,c;
int i = 0;
int main() {
      timer.start();
      sensor.frequency(8000000);
      while(1) {
            timer.reset();
            int angle = *sensor.read_angle();
            if(sensor.parity_check(angle)) {
                  int degrees = As5048Spi::degrees(angle)/100;
                  timeT = timer.read_us();
                  pc.printf("%i,%d\r\n",degrees,timeT);
            }else{
                  pc.printf("Parity check failed.\r\n");
            }
            wait_ms(1);
      }
}