
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
    console();
}

void setup() {
    timer.start();
    sensor.frequency(8000000);

    pwm[0].period_us(5);
    pwm[1].period_us(5);
    pwm[2].period_us(5);

    bEnable = 1;
    CurrentSense_init();
    timer.reset();

    turnChangeT.attach(turnChange, 0.01);
}
void console() {
    // pc.printf("shaftAngle:%d ,elAngle:%d ,theta:%d ,diff:%d ",
    //           shaftAngle, elAngle, theta, diff);
    // pc.printf("A0:%d,A1:%d", (int)(current.a * 1000), (int)(current.b *
    // 1000));
    // pc.printf("Id:%d\tIq:%d\t", (int)(dqCurrent.d * 1000),
    //           (int)(dqCurrent.q * 1000));
    // pc.printf("amout:%d\r\n", amout % 360);
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
            shaftAngle = degBetween_signed(readAngle(), angleZero);
            elAngle = (POLEPAIR * shaftAngle) % 360;
            elAngle = (elAngle < 0) ? 360 + elAngle : elAngle;
            dqCurrent = getFOCCurrents(elAngle, current, currentPrev);
            theta = elAngle; // + amout;

            elAnglePrev = elAngle;
            diff = theta - elAngle;
            diff = (diff < 0) ? 360 + diff : diff;

            float A = power * cos32_T(theta);
            float B = power * sin32_T(theta);

            // dq軸から波形を生成したい
            float dutyU = A * COS0 + B * SIN0 + 0.5;
            float dutyV = A * COS120 + B * SIN120 + 0.5;
            float dutyW = A * COS240 + B * SIN240 + 0.5;
            // float dutyU = power * sin32_T(theta) + 0.5;
            // float dutyV = power * sin32_T(theta + 120) + 0.5;
            // float dutyW = power * sin32_T(theta + 240) + 0.5;
            float max_ = max(dutyU, max(dutyV, dutyW)); //最大値
            float min_ = min(dutyU, min(dutyV, dutyW)); //最大値
            float offset = (max_ + min_) / 2;           //変調波
            //変調波を合成(元の振幅を維持)
            dutyU = dutyU * 11547 / 10000 - offset;
            dutyV = dutyV * 11547 / 10000 - offset;
            dutyW = dutyW * 11547 / 10000 - offset;
            writePWM(dutyU, dutyV, dutyW);
            // wait_ms(1);
        }
    }
}