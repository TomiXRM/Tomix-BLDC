#ifndef _SETUP_COMMON_H_
#define _SETUP_COMMON_H_

#define INLINE inline __attribute__((always_inline))
#define BAUDRATE 2000000

struct DQCurrent_s {
    float d;
    float q;
};
struct PhaseCurrent_s {
    float a;
    float b;
    float c;
};

#endif