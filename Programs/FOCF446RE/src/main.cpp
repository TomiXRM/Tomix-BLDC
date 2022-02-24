#include "mbed.h"
#include <AS5048A.h>
#include "TrigonometricTable.h"
#include "FastPWM.h"



As5048Spi sensor(D11, D12, D13, D10);
RawSerial pc(USBTX, USBRX,2000000); // tx, rx
FastPWM pwm[3] = {PC_7, PB_4, PB_10};
AnalogIn AN[2] = {A0,A1};
DigitalOut bEnable(D8);
Timer timer;


int timeT,theta,diff;
uint8_t polePair = 8;
int shaftAngle;
int elAngleZero,elAngle,elAnglePrev = 0;

struct DQCurrent_s {
      float d;
      float q;
};
struct PhaseCurrent_s {
      float a;
      float b;
      float c;
};


int16_t degBetween_signed(int16_t deg1,int16_t deg2){
      int16_t a = deg1 - deg2;
      while(a < 0) a += 360;
      while(a > 180) a -= 360;
      return a;
}


// void getFOCCurrents(float angle_el){
//       // read current phase currents
//       PhaseCurrent_s current = getPhaseCurrents();

//       // calculate clarke transform
//       float i_alpha, i_beta;
//       if(!current.c) {
//             // if only two measured currents
//             i_alpha = current.a;
//             i_beta = _1_SQRT3 * current.a + _2_SQRT3 * current.b;
//       } else {
//             // signal filtering using identity a + b + c = 0. Assumes measurement error is normally distributed.
//             float mid = (1.f/3) * (current.a + current.b + current.c);
//             float a = current.a - mid;
//             float b = current.b - mid;
//             i_alpha = a;
//             i_beta = _1_SQRT3 * a + _2_SQRT3 * b;
//       }

//       // calculate park transform
//       float ct = _cos(angle_el);
//       float st = _sin(angle_el);
//       DQCurrent_s return_current;
//       return_current.d = i_alpha * ct + i_beta * st;
//       return_current.q = i_beta * ct - i_alpha * st;
//       return return_current;

// }
int16_t readAngle(){
      int angle = *sensor.read_angle();
      if(sensor.parity_check(angle)) {
            int degrees = As5048Spi::degrees(angle)/100;
            // pc.printf("%i,%d\r\n",degrees,timeT);
            return degrees;
      }else{
            pc.printf("Parity check failed.\r\n");
      }
      return -1;
}

void setAngleZero(){
      pc.printf("setZero\r\n");
      pwm[0].write(0.5);
      wait_ms(500);
      elAngleZero = readAngle();
      pwm[0] = 0;
      pc.printf("setZero:%d\r\n",elAngleZero);
      wait_ms(500);
}

int turnPluse = 0;
int turnAmout = 1;
float power = 0.45;
int8_t carrea(){
      if(timer.read_ms() > 50) {
            turnPluse += turnAmout;
            if (turnPluse <= 0 || turnPluse >= 40) {
                  turnAmout = -turnAmout;
            }
            timer.reset();
      }
      power = 0.3;
      return turnPluse;
}

void setup(){
      timer.start();
      sensor.frequency(8000000);

      pwm[0].period_us(5);
      pwm[1].period_us(5);
      pwm[2].period_us(5);

      bEnable = 1;
      timer.reset();
}


uint16_t Current[2];
int main() {
      setup();
      while(1) {
            setAngleZero();
            while(1) {
                  // 強制転流用の電気角theta(これでぶん回す)
                  theta += carrea();
                  theta = theta % 360;

                  // 磁気エンコーダから観測された電気角
                  shaftAngle = degBetween_signed(readAngle(),elAngleZero);
                  elAngle = (polePair*shaftAngle)%360;
                  elAngle = (elAngle < 0) ? 360 + elAngle : elAngle;
                  Current[0] = AN[0].read_u16()/65.535; Current[1] = AN[1].read_u16()/65.535;

                  pwm[0].write(power*sin32_T(theta)+0.5);
                  pwm[1].write(power*sin32_T(theta+120)+0.5);
                  pwm[2].write(power*sin32_T(theta-120)+0.5);

                  elAnglePrev = elAngle;
                  diff = theta - elAngle;
                  pc.printf("shaftAngle:%d ,elAngle:%d ,theta:%d ,diff:%d ,A0:%d ,A1:%d\r\n",shaftAngle,elAngle,theta,diff,Current[0],Current[1]);
            }
      }
}