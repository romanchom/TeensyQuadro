#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vector.h"

#define PRECISE_ANGULAR_RATE_INTEGRATION 1

class Quaternion{
	Vector<4> mData;
public:
	Quaternion();

	void setIdentity();
	void fromAngleAxis(float angle, const Vector<3> & rot);
	void integrateAngularRate(const Vector<3> & angularRates);

	void operator*=(const Quaternion & other);
	Vector<4> & data(){ return mData; }

	float & w(){ return mData[0]; }
	float & x(){ return mData[1]; }
	float & y(){ return mData[2]; }
	float & z(){ return mData[3]; }
	Vector<3> & v(){ return reinterpret_cast<Vector<3> &>(mData[1]); }
	//void operator*=(float other);

	// actually conjugate, but for unit quaternions it's the same
	void invert();

	void normalize();

	Vector<3> transform(const Vector<3> & p);

	void setFromTo(const Vector<3> & from, const Vector<3> & to);
	void power(float p);

  void print(){
    mData.print();
  }
};

#endif // QUATERNION_H
