#ifndef CONFIG_H
#define CONFIG_H

#define LOOP_FREQUENCY 100
#define LOW_PASS_FILTER 3

static const float LOOP_DT = 1.0 / LOOP_FREQUENCY;
static const float STANDARD_G = 9.80665;

// undef evil code-breaking macros

#endif // CONFIG_H

#include <Arduino.h>
#undef min
#undef max
#undef abs
#undef constrain
#undef round
#undef radians
#undef degrees
#undef sq
