
#include "mbed.h"
#include <setup.h>

void console();

void turnChange() {
    amout += turnAmout;
    if (amout <= 200 || amout >= 340) {
        turnAmout = -turnAmout;
    }
    //     turnAmout++;
    //     amout = 270 + 60 * sin32_T(turnAmout);
    //     console();
}

void setup() {
    timer.start();
    sensor.frequency(8000000);

    pwm[0].period_us(20);
    pwm[1].period_us(20);
    pwm[2].period_us(20);

    bEnable = 1;
    currentSense_init();
    timer.reset();

    turnChangeT.attach(turnChange, 0.01);
}

void console() {
    // pc.printf("shaftAngle:%d ,elAngle:%d ,theta:%d ,diff:%d ",
    //           shaftAngle, elAngle, theta, diff);
    pc.printf("elAngle:%d\t", elAngle);
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
            dqCurrent = getFOCCurrents(elAngle, current, currentPrev);
            theta = elAngle; // + amout;

            elAnglePrev = elAngle;
            diff = theta - elAngle;
            diff = (diff < 0) ? 360 + diff : diff;
            writePWM(power * sin32_T(theta) + 0.5,
                     power * sin32_T(theta + 120) + 0.5,
                     power * sin32_T(theta + 240) + 0.5);
            // wait_ms(1);
            console();
        }
    }
}