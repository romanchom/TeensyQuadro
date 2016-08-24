#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vector.h"

class Quaternion{
	Vector<4> mData;
public:
	Quaternion();

	void fromAngleAxis(float angle, const Vector<3> & rot);
	void integrateAngularRate(const Vector<3> & angularRates);

	void operator*=(const Quaternion & other);
	Vector<4> & data(){ return mData; }
	//void operator*=(float other);

	// actually conjugate, but for unit quaternions it's the same
	void invert();

	void normalize();

	Vector<3> transform(const Vector<3> & p);

  void print(){
    mData.print();
  }
};

#endif // QUATERNION_H
