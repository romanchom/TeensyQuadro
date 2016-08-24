#ifndef DEBUG_H
#define DEBUG_H

#include "Arduino.h"
#define _log(x...) _print(x);

#define LED 13

#define NL "\n\r"

void blink_error(unsigned char error_code);

void debugInit();

template<typename T>
void _print(T t){
	Serial.print(t);
}

template<typename T, typename... Args>
void _print(T t, Args... args){
	_print(t);
	_print(args...);
}


#endif // DEBUG_H
