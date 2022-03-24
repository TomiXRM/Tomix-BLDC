#include "mbed.h"
#include <math.h>
// #include "rtos.h"

#define PI 3.14159265358979
#define TS1 0.2

int i = 0, q = 0;

short f1 = 0, a1 = 0;
int START = 8; // 8
float ut1 = 0, ut2 = 0, usi = 0;
float vt1 = 0, vt2 = 0, vsi = 0;
float wt1 = 0, wt2 = 0, wsi = 0;

float Speed;
float tau = 60; // 60

PwmOut mypwmA(PA_8);  // PWM_OUT 8
PwmOut mypwmB(PA_9);  // 9
PwmOut mypwmC(PA_10); // 10

DigitalOut EN1(PC_10);
DigitalOut EN2(PC_11);
DigitalOut EN3(PC_12);

InterruptIn HA(PA_15);
InterruptIn HB(PB_3);
InterruptIn HC(PB_10);

AnalogOut aout(PA_4);

// AnalogIn V_adc(PB_1);//Volume
AnalogIn V_adc(PC_2); // gaibu Volume

AnalogIn Current_W(PC_0);

float Vr_adc, output, Current_Ws;
float zint = tau * 1E-6; // 40E-6 MaX
Serial pc(USBTX, USBRX);

DigitalOut myled(LED1);

Timer uT;
Timer vT;
Timer wT;

Ticker zt;
Ticker thread;

int u[5] = {0x7FFF, 0xC000, 0, 0x0001, 0}; // a1,a2,f0,f1,f2
int v[5] = {0x7FFF, 0xC000, 0, 0x0001, 0}; // a1,a2,f0,f1,f2
int w[5] = {0x7FFF, 0xC000, 0, 0x0001, 0}; // a1,a2,f0,f1,f2

void ztrans()
{

      u[2] = ((u[1] * u[4]) >> 14) + ((u[0] * u[3]) >> 14);
      u[4] = u[3];
      u[3] = u[2];

      v[2] = ((v[1] * v[4]) >> 14) + ((v[0] * v[3]) >> 14);
      v[4] = v[3];
      v[3] = v[2];

      w[2] = ((w[1] * w[4]) >> 14) + ((w[0] * w[3]) >> 14);
      w[4] = w[3];
      w[3] = w[2];
}

void HAH()
{
      ut1 = uT.read_us();

      f1 = (sin(2 * 3.14159 * (1 / (usi * 1E-6)) * zint) * 16384); // 125E-6
      a1 = (2 * cos(2 * 3.14159 * (1 / (usi * 1E-6)) * zint) * 16384);

      u[0] = a1;
      u[1] = 0xC000;
      u[2] = 0;
      u[3] = f1;
      u[4] = 0;
}
void HAL()
{
      ut2 = uT.read_us();
      uT.reset();
}
void HBH()
{

      vt1 = vT.read_us();

      f1 = (sin(2 * 3.14159 * (1 / (vsi * 1E-6)) * zint) * 16384);
      a1 = (2 * cos(2 * 3.14159 * (1 / (vsi * 1E-6)) * zint) * 16384);

      v[0] = a1;
      v[1] = 0xC000;
      v[2] = 0;
      v[3] = f1;
      v[4] = 0;
}
void HBL()
{
      vt2 = vT.read_us();
      vT.reset();
}
void HCH()
{
      wt1 = wT.read_us();

      f1 = (sin(2 * 3.14159 * (1 / (wsi * 1E-6)) * zint) * 16384);
      a1 = (2 * cos(2 * 3.14159 * (1 / (wsi * 1E-6)) * zint) * 16384);

      w[0] = a1;
      w[1] = 0xC000;
      w[2] = 0;
      w[3] = f1;
      w[4] = 0;
}

void HCL()
{
      wt2 = wT.read_us();
      wT.reset();
}

void CPLT()
{
      pc.printf("%.3f , %.3f \r\n", Speed, Vr_adc);
      // pc.printf("%.3f \r" ,output);
}

void timerTS1()
{
      CPLT();
}

int main()
{

      pc.baud(2000000);

      zt.attach_us(&ztrans, tau); //  40MAX
      thread.attach_us(&timerTS1, 1000000);
      EN1 = 1;
      EN2 = 1;
      EN3 = 1;

      mypwmA.period_us(20);
      mypwmB.period_us(20);
      mypwmC.period_us(20);

      uT.start();
      vT.start();
      wT.start();

      // RtosTimer RtosTimerTS1(timerTS1);
      // RtosTimerTS1.start((unsigned int)(TS1 * 5000)); // 2000
      // Thread::wait(100);                              // 1000

      while (1)
      {

            Vr_adc = V_adc.read();

            if ((Vr_adc > 0.15f) && (q == 0))
            {
                  while (q < 50)
                  {

                        mypwmA.write(0); // 0.8f
                        mypwmB.write(0);
                        mypwmC.write(0.8f);
                        wait_ms(START);

                        mypwmA.write(0);
                        mypwmB.write(0.8f);
                        mypwmC.write(0);
                        wait_ms(START);

                        mypwmA.write(0.8f);
                        mypwmB.write(0);
                        mypwmC.write(0); // 0.8f
                        wait_ms(START);
                        q++;
                  }
            }
            HA.rise(&HAH);
            HC.fall(&HCL);
            HB.rise(&HBH);
            HA.fall(&HAL);
            HC.rise(&HCH);
            HB.fall(&HBL);

            if (Vr_adc < 0.08f)
            {
                  q = 0;
            }

            usi = 2 * (ut2 - ut1);
            vsi = 2 * (vt2 - vt1);
            wsi = 2 * (wt2 - wt1);

            if ((u[2]) <= 0)
            { //飽和処理
                  u[2] = 0;
            }
            if (u[2] >= 16383)
            {
                  u[2] = 16383;
            }

            if (v[2] <= 0)
            { //飽和処理
                  v[2] = 0;
            }
            if (v[2] >= 16383)
            {
                  v[2] = 16383;
            }
            if (w[2] <= 0)
            { //飽和処理
                  w[2] = 0;
            }
            if (w[2] >= 16383)
            {
                  w[2] = 16383;
            }

            aout = (float(v[2]) / 16383);

            mypwmA.write((float(u[2]) / 16383) * (Vr_adc));

            mypwmB.write((float(v[2]) / 16383) * (Vr_adc));

            mypwmC.write((float(w[2]) / 16383) * (Vr_adc));

            Speed = 60 * (1 / (7.0 * usi * 1E-6));

            myled = !myled;
      }
}