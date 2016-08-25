#include "Motors.h"

#include <core_pins.h>

static float clamp01(float val){
	if(val < 0.0f) return 0.0f;
	if(val > 1.0f) return 1.0f;
	return val;
}

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
	float duty = (1.0f + clamp01(power)) * (FREQUENCY * 0.001f * (1 << 16));
	int dutyInt = static_cast<int>(duty);
	analogWrite(PIN0 + index, dutyInt);
}
