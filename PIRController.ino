#include "PIRController.h"

PIRController::PIRController(int pin):
	interval(100),
	timeout(1000) {
		this->pin = pin;
}

PIRController::PIRController(int pin, int interval):
	timeout(1000) {
		this->pin = pin;
		this->interval = interval;
}

PIRController::PIRController(int pin, int interval, int timeout) {
	this->pin = pin;
	this->interval = interval;
	this->timeout = timeout;
}


void PIRController::setup() {
	pinMode(this->pin, INPUT);
}

bool PIRController::is_moving() {
	return elapsed(move_start) < timeout;
}

void PIRController::loop() {
	if (elapsed(move_check) < interval)
		return;
	move_check = timer;
	if (digitalRead(pin))
		move_start = timer;
}
