#include <mbed.h>
// Hall  3 1 2
InterruptIn Hall3(D10);
InterruptIn Hall1(D11);
InterruptIn Hall2(D12);

RawSerial pc(USBTX,USBRX,1000000);
bool hallState[3];

int16_t state;
uint8_t hallStateAsBit;


void printbits(uint8_t v) {
      for(int8_t i = 7; i >= 0; i--) putchar('0' + ((v >> i) & 1));
      putchar('\n');
}


void hallHandle(){
      hallState[0] = Hall1.read();
      hallState[1] = Hall2.read();
      hallState[2] = Hall3.read();
      hallStateAsBit = Hall1.read() << 1 | Hall2.read() | Hall3.read() << 2;
      // printbits(hallStateAsBit);
      // pc.printf(" %d\r\n",hallStateAsBit);
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
      pc.printf(" %d\r\n",state);
}

// static unsigned int getHallPosition(void){
//       if( (HALL1==HALL_HIGH) && (HALL2==HALL_LOW) && (HALL3==HALL_HIGH) ) {
//             return 1;
//       }
//       else if( (HALL1==HALL_HIGH) && (HALL2==HALL_LOW) && (HALL3==HALL_LOW) ) {
//             return 2;
//       }
//       else if( (HALL1==HALL_HIGH) && (HALL2==HALL_HIGH) && (HALL3==HALL_LOW) ) {
//             return 3;
//       }
//       else if( (HALL1==HALL_LOW) && (HALL2==HALL_HIGH) && (HALL3==HALL_LOW) ) {
//             return 4;
//       }
//       else if( (HALL1==HALL_LOW) && (HALL2==HALL_HIGH) && (HALL3==HALL_HIGH) ) {
//             return 5;
//       }
//       else if( (HALL1==HALL_LOW) && (HALL2==HALL_LOW) && (HALL3==HALL_HIGH) ) {
//             return 6;
//       }else{
//             return 0;
//       }
// }

int  main(){
      Hall3.rise(&hallHandle);
      Hall1.rise(&hallHandle);
      Hall2.rise(&hallHandle);
      Hall3.fall(&hallHandle);
      Hall1.fall(&hallHandle);
      Hall2.fall(&hallHandle);
      while(1) {

            // pc.printf("count:%d\r\n",count);
            wait_ms(10);
      }
}