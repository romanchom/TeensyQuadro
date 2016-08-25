#include "Quaternion.h"

Quaternion::Quaternion() : mData(1.0f, 0.0f, 0.0f, 0.0f) {}


void Quaternion::operator*=(const Quaternion & other){
	float temp[4];

	const float * p = other.mData.data();
	const float * q = mData.data();

	temp[0] =  q[0] * p[0];
	temp[0] -= q[1] * p[1];
	temp[0] -= q[2] * p[2];
	temp[0] -= q[3] * p[3];

	temp[1] =  q[0] * p[1];
	temp[1] += q[1] * p[0];
	temp[1] += q[2] * p[3];
	temp[1] -= q[3] * p[2];

	temp[2] =  q[0] * p[2];
	temp[2] -= q[1] * p[3];
	temp[2] += q[2] * p[0];
	temp[2] += q[3] * p[1];

	temp[3] =  q[0] * p[3];
	temp[3] += q[1] * p[2];
	temp[3] -= q[2] * p[1];
	temp[3] += q[3] * p[0];

	mData[0] = temp[0];
	mData[1] = temp[1];
	mData[2] = temp[2];
	mData[3] = temp[3];
}


/*Quaternion & Quaternion::operator*=(const Vector<1, frac> & other){
	q *= other;
	return (*this);
}*/



void Quaternion::invert(){
	mData[1] = -mData[1];
	mData[2] = -mData[2];
	mData[3] = -mData[3];
}

void Quaternion::normalize(){
	mData.normalize();
}

Vector<3> Quaternion::transform(const Vector<3> & p){
	// this sorcery is said to work
	Vector<3> & v = reinterpret_cast<Vector<3> &>(mData[1]);

	Vector<3> ret = v.cross(p);
	ret *= 2.0f;
	Vector<3> vCrossRet = v.cross(ret);
	ret *= mData[0];
	ret += p;
	ret += vCrossRet;
	return ret;
}

void Quaternion::fromAngleAxis(float angle, const Vector<3> & rot){
	angle *= 0.5f;
	mData[0] = cos(angle);
	float s = sin(angle);
	for(int i = 0; i < 3; ++i){
		mData[i + 1] = rot[i] * s;
	}
}

void Quaternion::integrateAngularRate(const Vector<3> & a){
	Vector<3> v = a;
	v *= 0.5f;
#if PRECISE_ANGULAR_RATE_INTEGRATION
	float length = v.length();
	v /= length;
	float c = cos(length);
	float s = sin(length);

	Quaternion dq;
	dq.data()[0] = c;
	dq.data()[1] = v[0] * s;
	dq.data()[2] = v[1] * s;
	dq.data()[3] = v[2] * s;
	// left sided multiplication is in sensors frame of reference
	*this *= dq;
#else
	// approximation for small rotation
	// such as continously integrating gyroscope readings
	Quaternion dq;
	dq.data()[0] = 1;
	dq.data()[1] = v[0];
	dq.data()[2] = v[1];
	dq.data()[3] = v[2];
	// left sided multiplication is in sensors frame of reference
	*this *= dq;
#endif
}

void Quaternion::setFromTo(const Vector<3> & from, const Vector<3> & to){
	Vector<3> & v = reinterpret_cast<Vector<3> &>(mData[1]);
	// this sorcery is said to work as well
	v = from.cross(to);
	mData[0] = sqrt(from.sqLength() * to.sqLength()) + from.dot(to);
}


void Quaternion::power(float p){
	// TODO possibly optimize this using some tricks
	float qLen = mData.length();
	float theta = acos(mData[0] / qLen);
	theta *= p;
	mData[0] = pow(qLen, p) * cos(theta);
	reinterpret_cast<Vector<3> &>(mData[1]).normalize();
	float s = sin(theta);
	for(int i = 1; i < 4; ++i){
		mData[i] *= s;
	}
}

/*
void Quaternion::fromRotationVector(Vector<3> rot){
	float sqLength = rot.sqLength();
	bool shortVector = false;
	if(sqLength.values[0] == 0){
		rot <<= (frac);
		sqLength = rot.sqLength();
		shortVector = true;
	}

	Vector<1, frac> invSqrt = recipSqrt(sqLength);

	rot *= invSqrt;
	sqLength *= invSqrt;
	if(shortVector) {
		sqLength >>= frac;
	}
	fromAngleAxis(sqLength, rot);
}*/
