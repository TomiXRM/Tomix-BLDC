#include "mbed.h"
#include <AS5048A.h>
#include "TrigonometricTable.h"
#include "FastPWM.h"



As5048Spi sensor(D11, D12, D13, D10);
RawSerial pc(USBTX, USBRX,2000000); // tx, rx
FastPWM pwm[3] = {PC_7, PB_4, PB_10};
DigitalOut bEnable(D8);
Timer timer;


int timeT,theta;
uint8_t polePair = 8;
int shaftAngle;
int elAngleZero,elAngle,elAnglePrev = 0;

int16_t degBetween_signed(int16_t deg1,int16_t deg2){
      int16_t a = deg1 - deg2;
      while(a < 0) a += 360;
      while(a > 180) a -= 360;
      return a;
}

int16_t readAngle(){
      timer.reset();
      int angle = *sensor.read_angle();
      if(sensor.parity_check(angle)) {
            int degrees = As5048Spi::degrees(angle)/100;
            timeT = timer.read_us();
            // pc.printf("%i,%d\r\n",degrees,timeT);
            return degrees;
      }else{
            pc.printf("Parity check failed.\r\n");
      }
      return -1;
}

void setup(){
      timer.start();
      sensor.frequency(8000000);

      pwm[0].period_us(5);
      pwm[1].period_us(5);
      pwm[2].period_us(5);

      bEnable = 1;
}



int main() {
      setup();
      while(1) {
            // readAngle();
            // wait_ms(1);
            pc.printf("setZero\r\n");
            pwm[0].write(0.5);
            wait_ms(500);
            elAngleZero = readAngle();
            pwm[0] = 0;
            pc.printf("setZero:%d\r\n",elAngleZero);
            wait_ms(500);

            while(1) {
                  // 強制転流用の電気角theta(これでぶん回す)
                  theta +=1;

                  // 磁気エンコーダから観測された電気角
                  shaftAngle = degBetween_signed(readAngle(),elAngleZero);
                  elAngle = (polePair*shaftAngle)%360;
                  elAngle = (elAngle > 0) ? elAngle : 360 + elAngle;

                  // wait_us(10);
                  pwm[0].write(0.25*sin32_T(theta)+0.5);
                  pwm[1].write(0.25*sin32_T(theta+120)+0.5);
                  pwm[2].write(0.25*sin32_T(theta-120)+0.5);

                  elAnglePrev = elAngle;
                  // pc.printf("%d,%d,%d\r\n",shaftAngle,elAngle,theta%360);
            }
      }
}