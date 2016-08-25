#ifndef PID_H
#define PID_H

class PIDController{
private:
	float mKd;
	float mKp;
	float mKi;
	float mLastError;
	float mIntegral;
	float mOutput;
public:
	PIDController();
	void update(float error, float dt);
	void setParams(float d, float p, float i){
		mKd = d;
		mKp = p;
		mKi = i;
	}
	float output(){ return mOutput; }
};

#endif // PID_H
