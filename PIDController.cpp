#include "PIDController.h"


PIDController::PIDController() :
	mKd(0.0f),
	mKp(0.0f),
	mKi(0.0f),
	mLastError(0.0f),
	mIntegral(0.0f)
{}

void PIDController::update(float error, float dt){
	// WOW!! trapezoidal rule
	float temp = error;
	temp += mLastError;
	temp *= dt;
	temp *= 0.5f;
	mIntegral += temp;

	temp = error;
	temp -= mLastError;
	temp /= dt;

	temp *= mKd;

	mOutput = temp;

	temp = error;
	temp *= mKp;
	mOutput += temp;

	temp = mIntegral;
	temp *= mKi;
	mOutput += temp;
}
