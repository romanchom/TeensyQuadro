#include "Config.h"
#include "Sensor.h"


#include "Debug.h"

void Sensor::init(){
	I2C::init();
	accelGyro.init();
	magneto.init();
}

void Sensor::read(){
	accelGyro.read();
	magneto.read();

	Vector<3> gyroI = accelGyro.gyro;
	gyroI *= (1.0f / LOOP_FREQUENCY);

	if(gyroI.sqLength() > 0.00000000001f){

		attitude.integrateAngularRate(gyroI);

		attitude.normalize();
	}
}
