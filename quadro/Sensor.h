#ifndef SENSOR_H
#define SENSOR_H

#include "Config.h"
#include "Quaternion.h"
#include "AccelGyro.h"
#include "Magneto.h"

class Sensor{
private:
	Vector<3> mInitialMagneticVector;
	bool mIsOnGround;
	Vector<3> computeAxisOffset(const Vector<3> & whatIs, const Vector<3> & whatShouldBe, float weight);
public:
	AccelGyro accelGyro;
	Magneto magneto;
	void init();
	void read();
	Quaternion attitude;
	void setIsOnGround(bool is){
		mIsOnGround = is;
	}
};

#endif // SENSOR_H
