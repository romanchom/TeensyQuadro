#include "Config.h"
#include "Sensor.h"


#include "Debug.h"

void Sensor::init(){
	I2C::init();
	magneto.init();
	accelGyro.init();
	mInitialMagneticVector.setZero();
	for(int i = 0; i < 20; ++i){
		magneto.read();
		mInitialMagneticVector += magneto.magneto;
		delay(10);
	}
	mInitialMagneticVector.normalize();
	mIsOnGround = false;
}


Vector<3> Sensor::computeAxisOffset(const Vector<3> & whatIs, const Vector<3> & whatShouldBe, float weight){
	Quaternion invAtt = attitude;
	invAtt.invert();
	Vector<3> whatShouldBeLocal = invAtt.transform(whatShouldBe);
	whatShouldBeLocal.normalize();

	Vector<3> axis = whatIs.cross(whatShouldBeLocal);
	axis.normalize();
	float angle = whatIs.dot(whatShouldBeLocal) / (whatIs.length() * whatShouldBeLocal.length());
	angle = acos(angle);
	angle *= weight;
	axis *= angle;
	return axis;
}

void Sensor::read(){
	accelGyro.read();
	magneto.read();

	Vector<3> gyroI = accelGyro.gyro;
	gyroI *= (1.0f / LOOP_FREQUENCY);

	attitude.integrateAngularRate(gyroI);

	Vector<3> fixAxis;
	bool driftFix = false;

	if(magneto.hasNewData()){
		fixAxis = computeAxisOffset(magneto.magneto, mInitialMagneticVector, 0.02f);
		driftFix = true;
		Quaternion attFix;
		float angle = fixAxis.length();
		attFix.fromAngleAxis(angle, fixAxis);
		// multiplication on left side is in sensor frame of reference
		attitude *= attFix;
	}
	if(mIsOnGround){
		Vector<3> up(0.0f, 0.0f, 1.0f);
		fixAxis = computeAxisOffset(accelGyro.accel, up, 0.05f);
		driftFix = true;
		Quaternion attFix;
		float angle = fixAxis.length();
		attFix.fromAngleAxis(angle, fixAxis);
		// multiplication on left side is in sensor frame of reference
		attitude *= attFix;
	}
	/*if(driftFix){
		Quaternion attFix;
		float angle = fixAxis.length();
		attFix.fromAngleAxis(angle, fixAxis);
		// multiplication on left side is in sensor frame of reference
		attitude *= attFix;
	}*/


	/*Serial.print("Mag\t");
	magneto.magneto.print();
	Serial.println();


	Serial.print("Accel\t");
	accelGyro.accel.print()
	Serial.println();*/

	attitude.normalize();
}
