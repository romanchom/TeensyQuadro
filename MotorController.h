#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "Sensor.h"
#include "Motors.h"
#include "PIDController.h"

extern float ROLL_KD;
extern float ROLL_KP;
extern float ROLL_KI;

class MotorController{
private:
	Sensor mSensor;
	Motors mMotors;

	PIDController mRollPID;
	PIDController mPitchPID;
	PIDController mYawPID;
	PIDController mVerticalPID;

	Quaternion mTargetOrientation;
	float mInputX;
	float mInputY;
	float mInputVertical;
	bool mEnabled;
public:
	MotorController();
	void init();
	void update();
	void setEnabled(bool enabled);
	void setHorizontalInput(float x, float y){
		mInputX = x;
		mInputY = y;
	};
	void setVerticalInput(float input){
		mInputVertical = input;
	}
	Sensor & sensor(){ return mSensor; }
};

#endif // MOTORCONTROLLER_H
