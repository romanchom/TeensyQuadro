#ifndef MONITORMSG_H
#define MONITORMSG_H

template<typename T>
void printValue(T value){
	Serial.print(value);
}

template<>
void printValue(float value){
	Serial.print(value, 7);
}

void monitorPrintContents(){}

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
