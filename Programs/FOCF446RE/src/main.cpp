#include "FastPWM.h"
#include "TrigonometricTable.h"
#include "mbed.h"
#include <AS5048A.h>

As5048Spi sensor(D11, D12, D13, D10);
RawSerial pc(USBTX, USBRX, 2000000); // tx, rx
FastPWM pwm[3] = {PC_7, PB_4, PB_10};
AnalogIn AN[2] = {A0, A2};
DigitalOut bEnable(D8);
Timer timer;
Ticker turnChangeT;

int timeT, theta, diff;
uint8_t polePair = 8;
int shaftAngle;
int elAngleZero, elAngle, elAnglePrev = 0;

struct DQCurrent_s {
    float d;
    float q;
};
struct PhaseCurrent_s {
    float a;
    float b;
    float c;
};

PhaseCurrent_s current;
DQCurrent_s dqCurrent;
float volts_to_amps_ratio = 1.0f / 0.01 / 50; // volts to amps
// gains for each phase
float gain_a, gain_b;
float offset_ia, offset_ib;

int16_t amout = 0;

int16_t degBetween_signed(int16_t deg1, int16_t deg2) {
    int16_t a = deg1 - deg2;
    while (a < 0)
        a += 360;
    while (a > 180)
        a -= 360;
    return a;
}

void CurrentSense_init() {
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

int16_t readAngle() {
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

void setAngleZero() {
    pc.printf("setZero\r\n");
    pwm[0].write(0.2);
    wait_ms(500);
    elAngleZero = readAngle();
    pwm[0] = 0;
    pc.printf("setZero:%d\r\n", elAngleZero);
    wait_ms(500);
}

int turnPluse = 0;
int turnAmout = 1;
float power = 0.45;

void turnChange() {
    amout += 1;
    //     amout = -90;
    //     if (amout == 0) {
    //         amout = 180;
    //     } else {
    //         amout = 0;
    //     }
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
            elAngle = (polePair * shaftAngle) % 360;
            elAngle = (elAngle < 0) ? 360 + elAngle : elAngle;
            dqCurrent = getFOCCurrents(elAngle);
            theta = elAngle + amout; // 0:CW 180:CCw

            pwm[0].write(power * sin32_T(theta) + 0.1);
            pwm[1].write(power * sin32_T(theta + 120) + 0.1);
            pwm[2].write(power * sin32_T(theta - 120) + 0.1);

            elAnglePrev = elAngle;
            diff = theta - elAngle;
            diff = (diff < 0) ? 360 + diff : diff;

            // pc.printf("shaftAngle:%d ,elAngle:%d ,theta:%d ,diff:%d ",
            //           shaftAngle, elAngle, theta, diff);
            // pc.printf("A0:%d,A1:%d", (int)(current.a * 1000),
            //           (int)(current.b * 1000));
            pc.printf("Id:%d,Iq:%d\r\n", (int)(dqCurrent.d * 1000),
                      (int)(dqCurrent.q * 1000));
            // wait_us(10);
        }
    }
}