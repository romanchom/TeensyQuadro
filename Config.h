#ifndef CONFIG_H
#define CONFIG_H

#define LOOP_FREQUENCY 100
#define LOW_PASS_FILTER 3

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
