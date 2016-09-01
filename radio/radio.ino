#include <Yaps.h>
#include <Yanrf.h>
#include <cmath>

struct UserInput{
	float inputX;
	float inputY;
	float inputVertical;
	bool enable;
	float pid[3];
};

Yanrf radio;
Yaps pad(8);
const uint8_t address[] = { 192, 168, 123 };
UserInput userInput;

void setup() {
	delay(1000);
	Serial.begin(9600);
	Serial.println("Hallo");

	SPI.setSCK(14);
	SPI.begin();

	radio.setCE(9);
	radio.setCSN(10);

	radio.init();
	radio.setChannel(111);
	radio.setDataRateAndPower(radio.RF_DATA_RATE_1M | radio.RF_POWER_0DBM);
	radio.setAdressLength(3);
	radio.setNetworkAdress(address);
	radio.setRetransmissions(1, 1); // 500us, 1 try

	Serial.println("Setup end");

	userInput.enable = false;
	userInput.pid[0] = 0.14f;
	userInput.pid[1] = 0.26f;
	userInput.pid[2] = 0.00005f;
}

const int count = 5;
int packetsSent = 0;
int packetsLost = 0;
int totalReceived = 0;
int totalSent = 0;
const int packetsPerTest = 100;

float softDeadZone(float x){
	return x * std::abs(x);
}


void loop() {
	uint32_t t = micros();
	t += 10000;
	pad.read();

	// press corss to enable
	userInput.enable |= pad.isDown(pad.BUTTON_START);
	// press stick to turn off
	userInput.enable &= !pad.isDown(pad.BUTTON_STICKRIGHT);
	// disable if pad is not analog
	userInput.enable &= pad.isAnalog();

	if(userInput.enable){
		userInput.inputX = softDeadZone(pad.getStickFloat(pad.STICK_LEFT | pad.AXIS_X));
		userInput.inputY = softDeadZone(-pad.getStickFloat(pad.STICK_LEFT | pad.AXIS_Y));
		userInput.inputVertical = (1.0f - pad.getStickFloat(pad.STICK_RIGHT | pad.AXIS_Y)) * 0.35f;
	}

	int param = -1;
	if(pad.isDown(pad.BUTTON_SQUARE)) param = 0;
	else if(pad.isDown(pad.BUTTON_TRIANGLE)) param = 1;
	else if(pad.isDown(pad.BUTTON_CIRCLE)) param = 2;
	if(param != -1){
		if(pad.isPressed(pad.BUTTON_UP)) userInput.pid[param] *= std::exp(0.05f);
		else if(pad.isPressed(pad.BUTTON_DOWN)) userInput.pid[param] *= std::exp(-0.05f);
	}


	/*Serial.print("D: ");
	Serial.println(userInput.pid[0], 7);

	Serial.print("P: ");
	Serial.println(userInput.pid[1], 7);

	Serial.print("I: ");
	Serial.println(userInput.pid[2], 7);*/

	radio.sendPacket(&userInput, sizeof(UserInput));

	uint8_t ret = radio.waitForEndOfTransmission(5000);

	++packetsSent;
	++totalSent;
	if(ret & radio.UNSUCCESSFUL) {
		++packetsLost;
		Serial.print("Total: sent: ");
		Serial.print(totalSent);
		Serial.print(" received: ");
		Serial.println(totalReceived);
	}else{
		++totalReceived;
	}
	while(int count = radio.available()){
		float ack[4];
		radio.read(ack, count);
		for(int i = 0; i < 4; ++i){
			Serial.print(ack[i], 7);
			Serial.print('\t');
		}
		Serial.println();
	}

	if(packetsSent == packetsPerTest){
		float ratio = 100.0f * packetsLost / packetsPerTest;
		packetsLost = 0;
		packetsSent = 0;
		Serial.print("Lost: ");
		Serial.println(ratio);
	}

	uint32_t d = micros();
	if(t > d){
		d = t - d;
		delayMicroseconds(d);
	}
}
