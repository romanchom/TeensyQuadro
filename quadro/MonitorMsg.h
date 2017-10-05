#ifndef MONITORMSG_H
#define MONITORMSG_H

#include "Quaternion.h"
#include "Vector.h"

template<typename T>
void printValue(T value){
	Serial.print(value);
}

template<>
void printValue<float>(float value);

template<>
void printValue<Quaternion>(Quaternion value);

template<>
void printValue<Vector<3>>(Vector<3> value);


void monitorPrintContents();

template<typename T, typename ... Args>
void monitorPrintContents(T value, Args... args){
	Serial.print('\t');
	printValue(value);
	monitorPrintContents(args...);
}

template<typename T, typename ... Args>
void monitorPrint(T value, Args... args){
	Serial.println();
	Serial.print(value);
	monitorPrintContents(args ...);
}


#endif // MONITORMSG_H
