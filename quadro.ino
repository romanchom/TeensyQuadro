#include "Sensor.h"
#include "Motors.h"
#include "Debug.h"

#include "Vector.h"

#include <nrf24l01.h>

enum {
  ledPin = 13,
};

Sensor sensor;
Motors motors;
RadioNRF radio;

void blink(){
    static uint8_t ledState = 0;
    ledState ^= 1;
    digitalWrite(ledPin, ledState);
}

const uint8_t address[] = { 192, 168, 123 };

void setup() {
	pinMode(ledPin, OUTPUT);
	blink();
	motors.init();
	delay(1000);
    Serial.begin(9600);
	Serial.println("Motors initialized.");
	Serial.println("Hello");
    sensor.init();
	Serial.println("Sensor initialized.");

	SPI.setSCK(14);
	SPI.begin();
	Serial.println("SPI initialized.");

	radio.setCE(8);
	radio.setCSN(9);

	radio.init();
	Serial.println("Radio initialized.");
	radio.setChannel(111);
	radio.setDataRateAndPower(radio.RF_DATA_RATE_1M | radio.RF_POWER_0DBM);
	radio.setAdressLength(3);
	radio.setNetworkAdress(address);
	radio.setRetransmissions(1, 1); // 500us, 1 try
	Serial.println("Radio configured.");

	Serial.println("Setup end");

	radio.setMode(radio.MODE_RECEIVER);
	radio.setListening(true);
	Serial.println("Radio powered on.");
	Serial.println("Setup done.");
}

float clamp01(float val){
	if(val < 0.0f) return 0.0f;
	if(val > 1.0f) return 1.0f;
	return val;
}

int lastPacket = 0;

float powerLeft = 0.0f;
float powerRight = 0.0f;

void loop() {
	unsigned t = micros() + 1000000 / LOOP_FREQUENCY;
	sensor.read();
	int8_t data[5];
	++lastPacket;
	int count = radio.available();

	if(count == 5){
		radio.read(data, count);
		float powerX = data[0];
		float powerY = reinterpret_cast<uint8_t &>(data[1]);

		powerX /= 128.0f;
		powerY /= 255.0f;

		powerLeft = 0.5f + powerX;
		powerLeft *= powerY;
		powerLeft = clamp01(powerLeft);

		powerRight = 0.5f - powerX;
		powerRight *= powerY;
		powerRight = clamp01(powerRight);

		lastPacket = 0;
	}

	if(lastPacket >= 15){
		powerLeft = 0.0f;
		powerRight = 0.0f;
	}
	motors.setPower(1, powerLeft);
	motors.setPower(3, powerRight);

	sensor.attitude.print();
	Serial.println();

	static int divider = 0;
	if(++divider == 10){
		divider = 0;
		blink();
	}

	while(micros() < t);
}
