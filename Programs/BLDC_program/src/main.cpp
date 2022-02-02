#include <mbed.h>

RawSerial pc(USBTX,USBRX,1000000);

int  main(){
      while(1) {
            pc.printf("hello world\r\n");
            wait(100);
      }
}