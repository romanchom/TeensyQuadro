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
	// optional division by 2 which might as well be included in Ki
	mIntegral += (error + mLastError) * dt;


	// derivative
	mOutput = (error - mLastError) * mKd / dt;
	// proportional
	mOutput += error * mKp;
	// integral
	mOutput += mIntegral * mKi;

	mLastError = error;
}
