#include "Magneto.h"


#define MAGNETO_PERIOD (10 / (1000 / LOOP_FREQUENCY))
#if MAGNETO_PERIOD < 1
	#undef MAGNETO_PERIOD
	#define MAGNETO_PERIOD 1
#endif

enum {
	MAGN_ADDRESS = 0x0C,

	REG_M_DEVICE_ID = 0x00,
	REG_M_DATA = 0x03,
	REG_M_CONTROL = 0x0A,
	REG_M_SENSITIVITY_ADJUSTMENT = 0x10,
	REG_M_SELF_TEST = 0x0C,
};


// I have no idea where I got this numbers from
static const Vector<3> magCal0(-0.109937, 0.204617, 0.059061);

// callibrate, swap x and y, negate z
static const Vector<3> magCal1(0.021530, 1.957276, -0.003436);
static const Vector<3> magCal2(1.877764, 0.021530, 0.024950);
static const Vector<3> magCal3(-0.024950, 0.003436, -1.976183);

Magneto::Magneto() :
	magnetoRead(MAGN_ADDRESS, REG_M_DATA),
	magnetoMeasurementEnableWrite(MAGN_ADDRESS, REG_M_CONTROL),
	magnetoPhase(0)
{}


void Magneto::init(){
	I2C::writeSync(MAGN_ADDRESS, REG_M_CONTROL, 0);
	delayMicroseconds(1);		// magnetometer is kinda slow, needs some time to process things
	I2C::writeSync(MAGN_ADDRESS, REG_M_CONTROL, 0b00001111);
	delayMicroseconds(1);

	{
		I2CRead<3> magnAdjRead(MAGN_ADDRESS, REG_M_SENSITIVITY_ADJUSTMENT);
		magnAdjRead.execute();

		Serial.write("Magn adj ");
		for(int i = 0; i < 3; ++i){
			magnetoScale[i] = magnAdjRead[i];
			magnetoScale[i] -= 128;
			magnetoScale[i] *= (1.0 / 256);
			magnetoScale[i] += 1;
		}
		magnetoScale.print();
		Serial.write('\n');
	}

	magnetoMeasurementEnableWrite[0] = 1;
	magnetoMeasurementEnableWrite.schedule();
}

void Magneto::read(){
	if(magnetoPhase == 1){
		Vector<3> temp;
    	for(int i = 0; i < 3; ++i){
			temp[i] = ((short *) magnetoRead.data())[i] / 256.0;
    	}

    	temp -= magCal0;
    	magneto.x() = temp.dot(magCal1);
		magneto.y() = temp.dot(magCal2);
		magneto.z() = temp.dot(magCal3);
		/*temp.cwiseMul(magnetoScale);
		magneto.x() = temp.y();
		magneto.y() = temp.x();
		magneto.z() = -temp.z();*/
    	magneto.normalize();
	}
	if(magnetoPhase == MAGNETO_PERIOD){
		magnetoRead.schedule();
		magnetoMeasurementEnableWrite.schedule();
		magnetoPhase = 0;
	}
	++magnetoPhase;
}
