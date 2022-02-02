#include <mbed.h>
#include <FastPWM.h>

FastPWM m1(PA_10);
RawSerial pc(USBTX,USBRX,230400);

int  main(){
      m1.period_us(2);

      while(1) {
            
      }
}