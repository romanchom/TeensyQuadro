#include "Config.h"
#include "Sensor.h"


#include "Debug.h"

static const float MAX_COMPASS_DIFFERENCE_ANGLE = 0.0 * (M_PI / 180.0);

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
}

void Sensor::read(){
	accelGyro.read();
	magneto.read();

	Vector<3> gyroI = accelGyro.gyro;
	gyroI *= (1.0f / LOOP_FREQUENCY);

	attitude.integrateAngularRate(gyroI);
	if(magneto.hasNewData()){
		Vector<3> northAsRead = magneto.magneto;
		northAsRead.normalize();

		Quaternion invAtt = attitude;
		invAtt.invert();
		Vector<3> northAsComputed = invAtt.transform(mInitialMagneticVector);
		northAsComputed.normalize();


		Vector<3> axis = northAsRead.cross(northAsComputed);
		axis.normalize();
		float angle = northAsRead.dot(northAsComputed);
		angle = acos(angle);
		if(angle > MAX_COMPASS_DIFFERENCE_ANGLE){
			angle -= MAX_COMPASS_DIFFERENCE_ANGLE;
			angle *= 0.02f;

			Quaternion attFix;
			attFix.fromAngleAxis(angle, axis);
			// multiplication on left side is in sensor frame of reference
			attitude *= attFix;
		}

		Serial.print("Quat\t");
		attitude.print();
		Serial.println();

		/*Serial.print("Mag\t");
		magneto.magneto.print();
		Serial.println();

		Serial.print("Acc\t");
		northAsComputed.print();
		Serial.println();*/
	}

	attitude.normalize();
}
