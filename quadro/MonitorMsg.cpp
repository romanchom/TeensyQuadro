#include "MonitorMsg.h"


void monitorPrintContents(){}

template<>
void printValue<float>(float value){
	Serial.print(value, 7);
}

template<>
void printValue<Quaternion>(Quaternion value){
	value.print();
}

template<>
void printValue<Vector<3>>(Vector<3> value){
	value.print();
}
