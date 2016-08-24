#ifndef MOTORS_H
#define MOTORS_H

class Motors {
private:
	enum {
		PIN0 = 3,
		MOTOR_COUNT = 4, // who would have guessed
		FREQUENCY = 100,
		RESOLUTION = 16,
	};
public:
	void init();
	void setPower(int index, float power);
};

#endif // MOTORS_H
