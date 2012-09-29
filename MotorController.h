#pragma once
#include "Timer.h"

class MotorController {
	static const int sp_stop = 1505;
	static const int sp_forward = 1205;
	static const int sp_backward = 1805;
	Servo motor;
	int motor_pin;
	int dc_pin;
	unsigned long duration;
	unsigned long started;
	bool working;
public:
	MotorController(int motor_pin, int dc_pin);
	void setup();
	void loop();
	void forward(unsigned long sec);
	void stop();
	void backward(unsigned long sec);
};
