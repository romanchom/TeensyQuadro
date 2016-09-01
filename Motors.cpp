#include "Motors.h"

#include <Arduino.h>
#include <core_pins.h>


enum {
	PIN0 = 3,
	MOTOR_COUNT = 4, // who would have guessed
	RESOLUTION = 16,
};

static const float FREQUENCY = 366.2109; // frequency closest to 400Hz at 96MHz clock

static const float SAFETY_POWER = 1.0f;

static float clampSafe(float val){
	if(val < 0.0f) return 0.0f;
	if(val > SAFETY_POWER) return SAFETY_POWER;
	return val;
}

static float clamp01(float val){
	if(val < 0.0f) return 0.0f;
	if(val > 1.0f) return 1.0f;
	return val;
}

static int computeDutyCycle(float power){
	float duty = (1.0f + power) * (FREQUENCY * 0.001 * (1 << 16));
	return static_cast<int>(duty);
}

void Motors::init(){
	analogWriteResolution(RESOLUTION);
	for(int i = 0; i < MOTOR_COUNT; ++i){
		int pin = PIN0 + i;
		pinMode(pin, OUTPUT);
    	analogWriteFrequency(pin, FREQUENCY);
	}
	//setPowerAll(0.0f);
	//delay(100);
	setPowerAll(0.0f);
	//delay(1000);
	//setPowerAll(0.0f);
}

void Motors::setPower(int index, float power){
	int duty = computeDutyCycle(clampSafe(power));
	analogWrite(PIN0 + index, duty);
}

void Motors::setPowerAll(float power){
	int duty = computeDutyCycle(power);
	for(int i = 0; i < MOTOR_COUNT; ++i){
		analogWrite(PIN0 + i, duty);
	}
}
