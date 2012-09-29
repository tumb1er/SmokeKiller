#include "LedController.h"

LedController::LedController() {
	LedController(13);
};

LedController::LedController(int pinLED) {
	this->pinLED = pinLED;
	starttime = 0;
	duration = 0;
	state = OFF;
}

void LedController::setup() {
	pinMode(pinLED, OUTPUT);
	digitalWrite(pinLED, LOW);
}

void LedController::blinkFor(unsigned long duration) {
	state = ON;
	starttime = timer;
	this->duration = duration;
	on();
}

void LedController::on() {
	digitalWrite(pinLED, HIGH);
}

void LedController::off() {
	digitalWrite(pinLED, LOW);
}

void LedController::loop(){
	if (state == OFF)
		return;
	if (elapsed(starttime) > duration){
		state = OFF;
		off();
	}
}
