#include "MotorController.h"

static const float ROLL_KD = 0.0f;
static const float ROLL_KP = 0.0f;
static const float ROLL_KI = 0.0f;

static const float PITCH_KD = ROLL_KD;
static const float PITCH_KP = ROLL_KP;
static const float PITCH_KI = ROLL_KP;

static const float YAW_KD = 0.0f;
static const float YAW_KP = 0.0f;
static const float YAW_KI = 0.0f;

static const float VERTICAL_KD = 0.0f;
static const float VERTICAL_KP = 0.0f;
static const float VERTICAL_KI = 0.0f;

MotorController::MotorController(){
	mRollPID.setParams(ROLL_KD, ROLL_KP, ROLL_KI);
	mPitchPID.setParams(PITCH_KD, PITCH_KP, PITCH_KI);
	mYawPID.setParams(YAW_KD, YAW_KP, YAW_KP);
	mVerticalPID.setParams(VERTICAL_KD, VERTICAL_KP, VERTICAL_KI);
}

void MotorController::init(){
	mMotors.init();
	Serial.println("Motors initialized.");
    mSensor.init();
	Serial.println("Sensor initialized.");
}

void MotorController::update(){
	mSensor.read();

	// step  one
	// obtain rotation from current attitude to target attitude
	Quaternion difference;
	Quaternion attInv = mSensor.attitude;
	attInv.invert();
	difference = mTargetOrientation;
	difference *= attInv;

	// step two
	// obtain angle axis representation of the rotation
	Vector<3> axis = difference.v();
	// atan2 is supposedly more numerically stable
	float angle = atan2(axis.length(), difference.w());
	axis /= sin(angle);
	angle *= 2;
	axis *= angle;
	// axis now containst errors for each rotation axis

	// step three
	// calculate vertical acceleration errors
	Vector<3> up(0.0f, 0.0f, 1.0f);
	up = mSensor.attitude.transform(up);
	static const float minAcc = 0.2f;
	float accelError = 0;
	// comparing with something larger than 0 avoids singularity near horizontal flight
	if(up.z() > 0.05f){
		// try to keep absolute vertical acceleration close to g
		accelError = 1.0f / up.z();

		angle = acos(up.z());
		angle -= M_PI * 0.4f;
		angle *= 30;
		// as long as relatively vertical
		accelError /= exp(angle);
		// after that keep a small constant acceleration to maintain maneuverability
		accelError += minAcc / exp(-angle);
	}else{
		accelError = minAcc;
	}
	accelError *= STANDARD_G;
	accelError -= mSensor.accelGyro.accel.z();

	// step four
	// apply errors to PIDs
	mPitchPID.update(axis.x(), LOOP_DT);
	mRollPID.update(axis.y(), LOOP_DT);
	mYawPID.update(axis.z(), LOOP_DT);
	mVerticalPID.update(accelError, LOOP_DT);

	// step five
	// accumulate PID outputs for each motor
	// motor 0 - front
	float power;

	power = mVerticalPID.output();
	power += mPitchPID.output();
	power += mYawPID.output();
	mMotors.setPower(0, power);

	power = mVerticalPID.output();
	power += mRollPID.output();
	power -= mYawPID.output();
	mMotors.setPower(1, power);

	power = mVerticalPID.output();
	power -= mPitchPID.output();
	power += mYawPID.output();
	mMotors.setPower(2, power);

	power = mVerticalPID.output();
	power -= mRollPID.output();
	power -= mYawPID.output();
	mMotors.setPower(3, power);
}
