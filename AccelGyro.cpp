#include "AccelGyro.h"

#include <cmath>
#include <algorithm>
#include "Debug.h"

enum {
	GYRO_OUTPUT_RATE = (LOW_PASS_FILTER ? 1000 : 8000),
	SAMPLE_RATE_DIVIDER = (GYRO_OUTPUT_RATE / LOOP_FREQUENCY - 1),

	GYRO_SELFTEST_X_BIT = 7,
	GYRO_SELFTEST_Y_BIT = 6,
	GYRO_SELFTEST_Z_BIT = 5,

	ACCEL_SELFTEST_X_BIT = 7,
	ACCEL_SELFTEST_Y_BIT = 6,
	ACCEL_SELFTEST_Z_BIT = 5,

	ACCEL_GYRO_ADDRESS = 0x68,

	REG_AG_SAMPLE_RATE_DIVIDER = 0x19,
	REG_AG_CONFIG = 0x1A,
	REG_AG_GYRO_CONFIG = 0x1B,
	REG_AG_ACCEL_CONFIG = 0x1C,
	REG_AG_FIFO_ENABLE = 0x23,
	REG_AG_INT_PIN_CFG = 0x37,
	REG_AG_INT_ENABLE = 0x38,
	REG_AG_DATA = 0x3B,
	REG_AG_USER_CONTROL = 0x6A,
	REG_AG_POWER_MANAGEMENT = 0x6B,
	REG_AG_WHO_AM_I = 0x75,
};


static const float gyroMultipliers[4] = {
	(250.0 * M_PI) / (180.0 * (1 << 15)),
	(250.0 * M_PI) / (180.0 * (1 << 14)),
	(250.0 * M_PI) / (180.0 * (1 << 13)),
	(250.0 * M_PI) / (180.0 * (1 << 12)),
};

static const float accelMultipliers[4] = {
	(STANDARD_G / (1 << 14)),
	(STANDARD_G / (1 << 13)),
	(STANDARD_G / (1 << 12)),
	(STANDARD_G / (1 << 11)),
};

AccelGyro::AccelGyro() :
	accelRange(0),
	gyroRange(0),
	setAccelRangeWrite(ACCEL_GYRO_ADDRESS, REG_AG_ACCEL_CONFIG),
	setGyroRangeWrite(ACCEL_GYRO_ADDRESS, REG_AG_GYRO_CONFIG),
	accelGyroTempRead(ACCEL_GYRO_ADDRESS, REG_AG_DATA)
{}

void AccelGyro::init(){
	bool i2c_ok = false;
	for(int i = 0; i < 10; ++i){
		if(I2C::readSync(ACCEL_GYRO_ADDRESS, REG_AG_WHO_AM_I) == ACCEL_GYRO_ADDRESS){
			i2c_ok = true;
			Serial.println("Accel gyro ok");
			break;
		}else{
			Serial.println("I2C fail");
			delay(1);
		}
	}
	//delay(10000);

	if(!i2c_ok){
		//_log("Could not connect to sensor.\n\r");
		blink_error(0b01010101);
		return;
	}

	// interrupt active low, push-pull, pulse, clear by any read, i2c bypass
	I2C::writeSync(ACCEL_GYRO_ADDRESS, REG_AG_INT_PIN_CFG, 0b10010010);

	// disable I2C master
	I2C::writeSync(ACCEL_GYRO_ADDRESS, REG_AG_USER_CONTROL, 0);

	// x axis gyro frequency reference
	I2C::writeSync(ACCEL_GYRO_ADDRESS, REG_AG_POWER_MANAGEMENT, 1);

	// set samlpe rate divider
	I2C::writeSync(ACCEL_GYRO_ADDRESS, REG_AG_SAMPLE_RATE_DIVIDER, SAMPLE_RATE_DIVIDER);
	// set low pass filter configuration
	I2C::writeSync(ACCEL_GYRO_ADDRESS, REG_AG_CONFIG, LOW_PASS_FILTER);

	// set gyroscope range
	I2C::writeSync(ACCEL_GYRO_ADDRESS, REG_AG_GYRO_CONFIG, 0);
	gyroMul = gyroMultipliers[0];

	// set accelerometer range
	I2C::writeSync(ACCEL_GYRO_ADDRESS, REG_AG_ACCEL_CONFIG, 0);
	accelMul = accelMultipliers[0];

	calibrate();
}

void AccelGyro::calibrate(){
	mGyroAvgDrift.setZero();
	mAccelAvgOffset.setZero();
	read();
	Vector<3> average;
	Vector<3> last;
	for(int i = 0; i < GYRO_CALIBRATION_SAMPLES; ++i){
		last -= gyro;
		if(last.sqLength() > GYRO_CALIBRATION_TOLERANCE * GYRO_CALIBRATION_TOLERANCE){
			average.setZero();
			i = 0;
		}else{
			average += gyro;
		}
		last = gyro;
		read();
		delay(GYRO_CALIBRATION_DELAY);
	}
	average /= GYRO_CALIBRATION_SAMPLES;
	mGyroAvgDrift = average;
	average.setZero();
	last = accel;
	for(int i = 0; i < ACCEL_CALIBRATION_SAMPLES; ++i){
		last -= accel;
		if(last.sqLength() > ACCEL_CALIBRATION_TOLERANCE * ACCEL_CALIBRATION_TOLERANCE){
			average.setZero();
			i = 0;
		}else{
			average += accel;
		}
		last = accel;
		read();
		delay(ACCEL_CALIBRATION_DELAY);
	}
	average *= 1.0f / ACCEL_CALIBRATION_SAMPLES;
	mAccelAvgOffset = average;
	mAccelAvgOffset.z() -= STANDARD_G;
}

void AccelGyro::readAccel(){
	int16_t maxAbs = 0;

	for(int i = 0; i < 3; ++i){
		int16_t data = ((int16_t *) accelGyroTempRead.data())[i];
		data = __builtin_bswap16(data);
		maxAbs = std::max<int16_t>(maxAbs, std::abs<int16_t>(data));
		accel[i] = data;
	}

	accel *= accelMul;
	accel -= mAccelAvgOffset;

	setAccelRange(maxAbs);
}

void AccelGyro::readGyro(){
	int16_t maxAbs = 0;
	for(int i = 0; i < 3; ++i){
		int16_t data = ((int16_t *) accelGyroTempRead.data())[i + 4];
		data = __builtin_bswap16(data);
		maxAbs = std::max<int16_t>(maxAbs, std::abs<int16_t>(data));
		gyro[i] = data;
	}

	gyro *= gyroMul;
	gyro -= mGyroAvgDrift;

	setGyroRange(maxAbs);
}

void AccelGyro::setAccelRange(int16_t maxAbs){
	// range changed in previous cycle
	if(accelRange & 0x80){
		// reset flag
		accelRange ^= 0x80;
		// apply multiplier change
		accelMul = accelMultipliers[accelRange];
	}else{
		uint8_t newRange = accelRange;
		if(maxAbs > GYRO_UPPER_RANGE && accelRange < 3) ++newRange;
		else if(maxAbs < GYRO_LOWER_RANGE && accelRange > 0) --newRange;

		if(newRange != accelRange) {
			setAccelRangeWrite[0] = newRange << 3;
			setAccelRangeWrite.schedule();
			accelRange = newRange | 0x80;
		}
	}
}

void AccelGyro::setGyroRange(int16_t maxAbs){
	// range changed in previous cycle
	if(gyroRange & 0x80){
		// reset flag
		gyroRange ^= 0x80;
		// apply multiplier change
		gyroMul = gyroMultipliers[gyroRange];
	}else{
		uint8_t newRange = gyroRange;
		if(maxAbs > GYRO_UPPER_RANGE && gyroRange < 3) ++newRange;
		else if(maxAbs < GYRO_LOWER_RANGE && gyroRange > 0) --newRange;

		if(newRange != gyroRange) {
			setGyroRangeWrite[0] = newRange << 3;
			setGyroRangeWrite.schedule();
			// set new range and set flag
			gyroRange = newRange | 0x80;
		}
	}
}

void AccelGyro::read(){
	readAccel();
	readGyro();
	accelGyroTempRead.schedule();
}
