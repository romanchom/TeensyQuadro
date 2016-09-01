#ifndef ACCELGYRO_H
#define ACCELGYRO_H

#include <cstdint>
#include <I2C.h>
#include "Config.h"
#include "Vector.h"

class AccelGyro {
private:
	enum{
		CALIBRATION_SAMPLES = 100,
		CALIBRATION_DELAY = 10,

		// upper bound is sqrt of 2 times range of short
		// lower bound is half of that
		ACCEL_UPPER_RANGE = 23170,
		ACCEL_LOWER_RANGE = 11585,
		GYRO_UPPER_RANGE = 23170,
		GYRO_LOWER_RANGE = 11585,
	};

	static constexpr float GYRO_CALIBRATION_TOLERANCE = 0.005f;
	static constexpr float ACCEL_CALIBRATION_TOLERANCE = 0.2f;

	uint8_t accelRange;
	uint8_t gyroRange;
	I2CWrite<1> setAccelRangeWrite;
	I2CWrite<1> setGyroRangeWrite;
	I2CRead<14> accelGyroTempRead;
	float gyroMul;
	float accelMul;
	void calibrate();
	void readAccel();
	void readGyro();
	void setAccelRange(int16_t maxAbs);
	void setGyroRange(int16_t maxAbs);
	Vector<3> mGyroAvgDrift;
	Vector<3> mAccelAvgOffset;
public:
	Vector<3> accel;
	Vector<3> gyro;

	AccelGyro();
	void init();
	void read();
};


#endif // ACCELGYRO_H
