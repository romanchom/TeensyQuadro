#include "Motors.h"

#include <core_pins.h>

void Motors::init(){
	analogWriteResolution(RESOLUTION);
	for(int i = 0; i < MOTOR_COUNT; ++i){
		int pin = PIN0 + i;
		pinMode(pin, OUTPUT);
		setPower(i, 0.0f);
    	analogWriteFrequency(pin, FREQUENCY);
	}
	delay(100);
}

void Motors::setPower(int index, float power){
	float duty = (0.001f + 0.001f * power) * FREQUENCY;
	int dutyInt = (int)(duty * (1 << 16));
	analogWrite(PIN0 + index, dutyInt);
}
