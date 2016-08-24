#ifndef VECTOR3_H
#define VECTOR3_H

#include <Arduino.h>
#include <cmath>

template<int size>
class Vector{
private:
	float mData[size];
public:
	// constructors
	Vector(){};
	template<typename... Args>
	Vector(Args... args) :
		mData{args...}
	{}

	// accessors
	inline float & operator[](int index){ return mData[index]; }
	float operator[](int index) const { return mData[index]; }
	float & x(){ return mData[0]; }
	float & y(){ return mData[1]; }
	float & z(){ return mData[2]; }
	float & w(){ return mData[3]; }

	float x() const { return mData[0]; }
	float y() const { return mData[1]; }
	float z() const { return mData[2]; }
	float w() const { return mData[3]; }

	const float * data() const { return mData; }

	void setZero(){
		for(int i = 0; i < size; ++i){
			mData[i] = 0.0f;
		}
	}
	// operators
	void operator+=(const Vector & other){
		for(int i = 0; i < size; ++i){
			mData[i] += other.mData[i];
		}
	}
	void operator-=(const Vector & other){
		for(int i = 0; i < size; ++i){
			mData[i] -= other.mData[i];
		}
	}
	void cwiseMul(const Vector & other){
		for(int i = 0; i < size; ++i){
			mData[i] *= other.mData[i];
		}
	}
	void cwiseDiv(const Vector & other){
		for(int i = 0; i < size; ++i){
			mData[i] /= other.mData[i];
		}
	}

	void operator*=(const float & other){
		for(int i = 0; i < size; ++i){
			mData[i] *= other;
		}
	}
	void operator/=(const float & other){
		float recip = 1 / other;
		for(int i = 0; i < size; ++i){
			mData[i] *= recip;
		}
	}

	float dot(const Vector & other) const{
		float sum = 0;
		for(int i = 0; i < size; ++i){
			sum += mData[i] * other.mData[i];
		}
		return sum;
	}

	Vector cross(const Vector & o) const {
		Vector ret;
		ret[0] = y() * o.z() - z() * o.y();
		ret[1] = z() * o.x() - x() * o.z();
		ret[2] = x() * o.y() - y() * o.x();
		return ret;
	}

	// functions
	void print() const{
		for(int i = 0; i < size; ++i){
			Serial.print(mData[i], 6);
			Serial.print('\t');
		}
	}

	float sqLength() const{
		return dot(*this);
	}

	float length() const{
		return sqrt(sqLength());
	}

	void normalize(){
		float recipSqLen = 1 / length();
		for(int i = 0; i < size; ++i){
			mData[i] *= recipSqLen;
		}
	}
};


#endif // VECTOR3_H
