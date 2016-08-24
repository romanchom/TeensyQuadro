#ifndef MAGNETO_H
#define MAGNETO_H

#include <cstdint>
#include "Vector.h"
#include "Config.h"
#include "I2C.h"

class Magneto {
private:
	I2CRead<6> magnetoRead;
	I2CWrite<1> magnetoMeasurementEnableWrite;

	unsigned magnetoPhase;
	Vector<3> magnetoScale;
	Vector<3> magnetoOffset;

	unsigned period;
public:
	Magneto();
	void init();
	void read();
	Vector<3> magneto;
};


#endif // MAGNETO_H