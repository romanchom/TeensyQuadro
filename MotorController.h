#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "Sensor.h"
#include "Motors.h"
#include "PIDController.h"

class MotorController{
private:
	Sensor mSensor;
	Motors mMotors;

	PIDController mRollPID;
	PIDController mPitchPID;
	PIDController mYawPID;
	PIDController mVerticalPID;

	Quaternion mTargetOrientation;
public:
	MotorController();
	void init();
	void update();
};

#endif // MOTORCONTROLLER_H
