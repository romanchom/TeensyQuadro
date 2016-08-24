#include "Debug.h"


#include "IntervalTimer.h"

IntervalTimer blinker;

static void blink(){
    static uint8_t state = 0;
    state ^= 1;
    digitalWriteFast(13, state);
	//log("blink");
}

void blink_error(unsigned char error_code){
	blinker.end();
	while(true){
		digitalWriteFast(LED, LOW);
		delay(1000);
		for(int i = 7; i >= 0; --i){
			digitalWriteFast(LED, HIGH);
			delay(250);
			if((error_code & 1 << i) == 0){
				digitalWriteFast(LED, LOW);
			}
			delay(500);
			digitalWriteFast(LED, LOW);
			delay(250);
		}
	}
}

void debugInit(){
	pinMode(LED, OUTPUT);
	blinker.begin(blink, 500000);
}