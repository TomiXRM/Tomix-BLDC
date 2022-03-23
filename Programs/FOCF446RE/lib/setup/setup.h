#ifndef _SETUP_H_
#define _SETUP_H_

#include "FastPWM.h"
#include "TrigonometricTable.h"
#include <AS5048A.h>
#include <setup_common.h>

#define POLEPAIR 8

As5048Spi sensor(D11, D12, D13, D10);
RawSerial pc(USBTX, USBRX, 2000000); // tx, rx
FastPWM pwm[3] = {PC_7, PB_4, PB_10};
AnalogIn AN[2] = {A0, A2};
DigitalOut bEnable(D8);
Timer timer;

#endif