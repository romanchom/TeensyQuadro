#ifndef SENSOR_H
#define SENSOR_H

#include "Config.h"
#include "Quaternion.h"
#include "AccelGyro.h"
#include "Magneto.h"

class Sensor{
private:
	Vector<3> mInitialMagneticVector;
public:
	AccelGyro accelGyro;
	Magneto magneto;
	void init();
	void read();
	Quaternion attitude;
};

#endif // SENSOR_H
