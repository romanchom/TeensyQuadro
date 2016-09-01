#ifndef MOTORS_H
#define MOTORS_H

class Motors {
public:
	void init();
	void setPower(int index, float power);
	void setPowerAll(float power);
};

#endif // MOTORS_H
