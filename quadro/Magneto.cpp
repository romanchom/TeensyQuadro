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

// I got those numbers from an app called Magneto, see
// https://sites.google.com/site/sailboatinstruments1/home
static const Vector<3> magCal0(87.251003, -16.269410, -13.964382);

// callibrate, swap x and y, negate z
static const Vector<3> magCalX(0.006861, 0.000085, -0.000040);
static const Vector<3> magCalY(0.000085, 0.007306, -0.000005);
static const Vector<3> magCalZ(-0.000040, -0.000005, 0.007618);

Magneto::Magneto() :
	magnetoRead(MAGN_ADDRESS, REG_M_DATA),
	magnetoMeasurementEnableWrite(MAGN_ADDRESS, REG_M_CONTROL),
	magnetoPhase(0)
{}


void Magneto::init(){
	I2C::writeSync(MAGN_ADDRESS, REG_M_CONTROL, 0);
	delayMicroseconds(1);		// magnetometer is kinda slow, needs some time to process things

	magnetoMeasurementEnableWrite[0] = 1;
	magnetoMeasurementEnableWrite.schedule();
}

void Magneto::read(){
	if(magnetoPhase == 1){
		Vector<3> temp;
    	for(int i = 0; i < 3; ++i){
			temp[i] = ((short *) magnetoRead.data())[i];
    	}

		//magneto = temp;
    	temp += magCal0;
    	magneto.x() = temp.dot(magCalY);
		magneto.y() = temp.dot(magCalX);
		magneto.z() = -temp.dot(magCalZ);
    	//magneto.normalize();
	}
	if(magnetoPhase == MAGNETO_PERIOD){
		magnetoRead.schedule();
		magnetoMeasurementEnableWrite.schedule();
		magnetoPhase = 0;
	}
	++magnetoPhase;
}
