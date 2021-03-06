#include "MotorController.h"

//#define ROLL_PITCH_SEPARATE_PID


#include "MonitorMsg.h"


float ROLL_KD = 0.03f;
float ROLL_KP = 0.15f;
float ROLL_KI = 0.00005f;


#define PITCH_KD ROLL_KD
#define PITCH_KP ROLL_KP
#define PITCH_KI ROLL_KI

/*
static const float ROLL_KD = 0.03f;
static const float ROLL_KP = 0.15f;
static const float ROLL_KI = 0.00005f;

#ifdef ROLL_PITCH_SEPARATE_PID
static const float PITCH_KD = 0.0f;
static const float PITCH_KP = 0.0f;
static const float PITCH_KI = 0.0f;
#else
static const float PITCH_KD = ROLL_KD;
static const float PITCH_KP = ROLL_KP;
static const float PITCH_KI = ROLL_KI;
#endif
static const float YAW_KD = 0.0f;
static const float YAW_KP = 0.0f;
static const float YAW_KI = 0.0f;*/


#define YAW_KD ROLL_KD
#define YAW_KP ROLL_KP
#define YAW_KI ROLL_KI

static const float VERTICAL_KD = 0.0000001f;
static const float VERTICAL_KP = 0.005f;
static const float VERTICAL_KI = 0.025f;

MotorController::MotorController() :
	mInputX(0.0f),
	mInputY(0.0f),
	mInputVertical(0.0f),
	mEnabled(false)
{
	mRollPID.setParams(ROLL_KD, ROLL_KP, ROLL_KI);
	mPitchPID.setParams(PITCH_KD, PITCH_KP, PITCH_KI);
	mYawPID.setParams(YAW_KD, YAW_KP, YAW_KI);
	mVerticalPID.setParams(VERTICAL_KD, VERTICAL_KP, VERTICAL_KI);
}

void MotorController::init(){
	mMotors.init();
	Serial.println("Motors initialized.");
    mSensor.init();
	Serial.println("Sensor initialized.");
	mSensor.setIsOnGround(true);
}

void MotorController::update(){
	mSensor.read();
	mRollPID.setParams(ROLL_KD, ROLL_KP, ROLL_KI);
	mPitchPID.setParams(PITCH_KD, PITCH_KP, PITCH_KI);

	if(!mEnabled) return;

	{
		Quaternion userInputRot;
		// rotating right is rotating oround positive Y axis
		// rotating forward is rotating around negative X axis, I think
		Vector<3> axis(-mInputY, mInputX);
		float angle = axis.length();
		if(angle > 0.00001f){
			axis /= angle;
			// input scaling
			userInputRot.fromAngleAxis(angle * LOOP_DT, axis);
			userInputRot *= mTargetOrientation;
			mTargetOrientation = userInputRot;
		}
	}
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
	if(angle > M_PI / 2) angle -= M_PI;
	//axis /= sin(angle);
	axis.normalize();
	//angle *= 2;
	axis *= angle;
	// axis now containst errors for each rotation axis

	// step three
	// calculate vertical acceleration errors
	Vector<3> up(0.0f, 0.0f, 1.0f);
	up = mSensor.attitude.transform(up);
	float minAcc = 0.8f + mInputVertical;
	float accelError = 0;
	accelError = mInputVertical;
	// comparing with something larger than 0 avoids singularity near horizontal flight
	if(up.z() > 0.05f){
		// try to keep absolute vertical acceleration close to g +- user input
		accelError = (1.0f + mInputVertical) / up.z();

		angle = acos(up.z());
		angle -= M_PI * 0.4f;
		angle *= 30;
		// as long as relatively vertical
		accelError /= (1 + exp(angle));
		// after that keep a small constant acceleration to maintain maneuverability
		accelError += minAcc / (1 + exp(-angle));
	}else{
		accelError = minAcc;
	}
	accelError *= STANDARD_G;
	accelError = accelError - mSensor.accelGyro.accel.z();

	//monitorPrint(20, "ERROR: ", accelError);

	//Serial.print(accelError);
	//Serial.println();

	// step four
	// apply errors to PIDs
	mPitchPID.update(axis.x(), LOOP_DT);
	mRollPID.update(axis.y(), LOOP_DT);
	mYawPID.update(axis.z(), LOOP_DT);
	mVerticalPID.update(accelError, LOOP_DT);

	// step five
	// accumulate PID outputs for each motor
	// motor 0 - front right
	// motor 1 - rear right
	// motor 2 - rear left
	// motor 3 - front left
	float power;
	//return;
	power = mVerticalPID.output();
	power += mPitchPID.output();
	power -= mRollPID.output();
	power += mYawPID.output();
	mMotors.setPower(0, power);

	power = mVerticalPID.output();
	power -= mPitchPID.output();
	power -= mRollPID.output();
	power -= mYawPID.output();
	mMotors.setPower(1, power);

	power = mVerticalPID.output();
	power -= mPitchPID.output();
	power += mRollPID.output();
	power += mYawPID.output();
	mMotors.setPower(2, power);

	power = mVerticalPID.output();
	power += mPitchPID.output();
	power += mRollPID.output();
	power -= mYawPID.output();
	mMotors.setPower(3, power);
}

void MotorController::setEnabled(bool enabled){
	if(mEnabled != enabled){
		mEnabled = enabled;
		mSensor.setIsOnGround(!enabled);
		if(!enabled){
			mTargetOrientation.setIdentity();
			mMotors.setPowerAll(0.0f);
			mVerticalPID.reset();
			mPitchPID.reset();
			mYawPID.reset();
		}
	}
}
