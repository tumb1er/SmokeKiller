#include "MotorController.h"

MotorController::MotorController(int motor_pin, int dc_pin) {
	this->motor_pin = motor_pin;
	this->dc_pin = dc_pin;
	started = 0;
	working = false;
	duration = 0;
}

void MotorController::setup() {
	pinMode(dc_pin, OUTPUT);
	motor.attach(motor_pin);
	stop();
}

void MotorController::backward(unsigned long sec){
	started = timer;
	digitalWrite(dc_pin, HIGH);
	motor.writeMicroseconds(sp_backward);
	duration = sec;
	working = true;
}

void MotorController::stop() {
	motor.writeMicroseconds(sp_stop);
	digitalWrite(dc_pin, LOW);
	working = false;
}

void MotorController::forward(unsigned long sec){
	started = timer;
	digitalWrite(dc_pin, HIGH);
	motor.writeMicroseconds(sp_forward);
	duration = sec;
	working = true;
}

void MotorController::loop() {
	if (working && (elapsed(started) > duration))
		stop();
}
