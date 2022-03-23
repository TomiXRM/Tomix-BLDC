
#include "mbed.h"
#include <setup.h>

Ticker turnChangeT;

int timeT, theta, diff;

int shaftAngle;
int elAngleZero, elAngle, elAnglePrev = 0;

PhaseCurrent_s current;
DQCurrent_s dqCurrent;
float volts_to_amps_ratio = 1.0f / 0.01 / 50; // volts to amps
// gains for each phase
float gain_a, gain_b;
float offset_ia, offset_ib;

int turnAmout = 1;
float power = 0.45;

int16_t amout = 210;
void console();

DQCurrent_s getFOCCurrents(float angle_el) {
    // read current phase currents
    current.a = 3.3 * (AN[0].read() - offset_ia) * gain_a;
    current.b = 3.3 * (AN[1].read() - offset_ib) * gain_b;

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

void turnChange() {
    amout += turnAmout;
    if (amout <= 200 || amout >= 340) {
        turnAmout = -turnAmout;
    }
    //     turnAmout++;
    //     amout = 270 + 60 * sin32_T(turnAmout);
    console();
}

void setup() {
    timer.start();
    sensor.frequency(8000000);

    pwm[0].period_us(20);
    pwm[1].period_us(20);
    pwm[2].period_us(20);

    bEnable = 1;
    CurrentSense_init();
    timer.reset();

    turnChangeT.attach(turnChange, 0.01);
}
void console() {
    // pc.printf("shaftAngle:%d ,elAngle:%d ,theta:%d ,diff:%d ",
    //           shaftAngle, elAngle, theta, diff);
    pc.printf("A0:%d,A1:%d", (int)(current.a * 1000), (int)(current.b * 1000));
    pc.printf("Id:%d\tIq:%d\t", (int)(dqCurrent.d * 1000),
              (int)(dqCurrent.q * 1000));
    pc.printf("amout:%d\r\n", amout % 360);
}
int main() {
    setup();
    while (1) {
        setAngleZero();
        while (1) {
            theta = theta % 360;
            if (theta >= 360)
                theta = 0;

            // 磁気エンコーダから観測された電気角
            shaftAngle = degBetween_signed(readAngle(), elAngleZero);
            elAngle = (POLEPAIR * shaftAngle) % 360;
            elAngle = (elAngle < 0) ? 360 + elAngle : elAngle;
            dqCurrent = getFOCCurrents(elAngle);
            theta = elAngle - 30; // + amout;

            elAnglePrev = elAngle;
            diff = theta - elAngle;
            diff = (diff < 0) ? 360 + diff : diff;
            writePWM(power * sin32_T(theta) + 0.5,
                     power * sin32_T(theta + 120) + 0.5,
                     power * sin32_T(theta + 240) + 0.5);
            // wait_ms(1);
        }
    }
}