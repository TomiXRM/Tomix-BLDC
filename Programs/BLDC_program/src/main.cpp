#include <mbed.h>
// Hall  3 1 2
InterruptIn Hall3(D10);
InterruptIn Hall1(D11);
InterruptIn Hall2(D12);

RawSerial pc(USBTX,USBRX,1000000);
bool hallState[3];
uint8_t state,statePrev,hallStateAsBit;;
int16_t count,degree;

void hallHandle(){
      hallState[0] = Hall1.read();
      hallState[1] = Hall2.read();
      hallState[2] = Hall3.read();
      hallStateAsBit = Hall1.read() << 1 | Hall2.read() | Hall3.read() << 2;
      switch (hallStateAsBit) {
      case 1: // 001
            state = 1; break;
      case 3: // 011
            state = 2; break;
      case 2: // 010
            state = 3; break;
      case 6: // 110
            state = 4; break;
      case 4: // 100
            state = 5; break;
      case 5: // 101
            state = 6; break;
      default:
            break;
      }
      if(statePrev != state && ((statePrev - state) == 1 || (statePrev - state) == -5)) {
            count++;
      }else if(statePrev != state && ((statePrev - state) == -1 || (statePrev - state) == 5)) {
            count--;
      }
      degree = (count*75)%3600;
      statePrev = state;
      pc.printf(" %d count:%d\t degree:%d\r\n",state,count,degree);
}

int  main(){
      Hall3.rise(&hallHandle);
      Hall1.rise(&hallHandle);
      Hall2.rise(&hallHandle);
      Hall3.fall(&hallHandle);
      Hall1.fall(&hallHandle);
      Hall2.fall(&hallHandle);

      count = 0;
      while(1) {

            // pc.printf("count:%d\r\n",count);
            // wait_ms(10);

      }
}