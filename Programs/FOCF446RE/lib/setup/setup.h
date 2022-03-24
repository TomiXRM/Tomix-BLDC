#ifndef _SETUP_H_
#define _SETUP_H_

#include "FastPWM.h"
#include "TrigonometricTable.h"
#include <AS5048A.h>
#include <setup_common.h>

#define POLEPAIR 8

As5048Spi sensor(D11, D12, D13, D10);
RawSerial pc(USBTX, USBRX, 2000000);
FastPWM pwm[3] = {PC_7, PB_4, PB_10};
AnalogIn AN[2] = {A0, A2};
DigitalOut bEnable(D8);

Timer timer;
Ticker turnChangeT;
int timeT, theta, diff;
int shaftAngle;
int elAngleZero, elAngle, elAnglePrev = 0;
float volts_to_amps_ratio = 1.0f / 0.01 / 50; // volts to amps

int turnAmout = 1;
float power = 0.3;
int16_t amout = 210;
// gains for each phase
float gain_a, gain_b;
float offset_ia, offset_ib;

PhaseCurrent_s current;
PhaseCurrent_s currentPrev;
DQCurrent_s dqCurrent;

extern void writePWM(float pU, float pV, float pW) {
    pwm[0].write(pU);
    pwm[1].write(pV);
    pwm[2].write(pW);
}

extern int16_t degBetween_signed(int16_t deg1, int16_t deg2) {
    int16_t a = deg1 - deg2;
    while (a < 0)
        a += 360;
    while (a > 180)
        a -= 360;
    return a;
}

extern int16_t readAngle() {
    int angle = *sensor.read_angle();
    if (sensor.parity_check(angle)) {
        int degrees = As5048Spi::degrees(angle) / 100;
        // pc.printf("%i,%d\r\n",degrees,timeT);
        return degrees;
    } else {
        pc.printf("Parity check failed.\r\n");
    }
    return -1;
}

extern void setAngleZero() {
    pc.printf("setZero\r\n");
    pwm[0].write(0.5);
    wait_ms(500);
    elAngleZero = readAngle();
    pwm[0] = 0;
    pc.printf("setZero:%d\r\n", elAngleZero);
    wait_ms(500);
}

extern void currentSense_init() {
    offset_ia = 0;
    offset_ib = 0;
    for (size_t i = 0; i < 1000; i++) {
        offset_ia += AN[0].read();
        offset_ib += AN[1].read();
    }
    offset_ia = offset_ia / 1000;
    offset_ib = offset_ib / 1000;
    pc.printf("offset_ia: %d offset_ib: %d\r\n", (int)(offset_ia * 1000),
              (int)(offset_ib * 1000));
    gain_a = volts_to_amps_ratio;
    gain_b = volts_to_amps_ratio;
    gain_b *= -1;
}

DQCurrent_s getFOCCurrents(float angle_el, PhaseCurrent_s &current,PhaseCurrent_s &currentPrev) {
    // read current phase currents
    current.a =
        (3.3 * (AN[0].read() - offset_ia) * gain_a) * 0.6 + currentPrev.a * 0.4;
    current.b =
        (3.3 * (AN[1].read() - offset_ib) * gain_b) * 0.6 + currentPrev.b * 0.4;

    currentPrev.a = current.a;
    currentPrev.a = current.b;
    // calculate clarke transform
    float i_alpha = current.a;
    float i_beta = (current.a + 2 * current.b) / 1.73205;

    // // calculate park transform
    float ct = cos32_T(angle_el);
    float st = cos32_T(angle_el);

    DQCurrent_s return_current;
    return_current.d = i_alpha * ct + i_beta * st;
    return_current.q = i_beta * ct - i_alpha * st;
    return return_current;
}

#endif