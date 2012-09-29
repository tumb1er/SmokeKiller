#pragma once
#include "Timer.h"

class PIRController {
	int pin;
	unsigned long move_start;
	unsigned long move_check;
	int interval;
	int timeout;
public:
	PIRController(int pin);
	PIRController(int pin, int interval);
	PIRController(int pin, int interval, int timeout);
	bool is_moving();
	void setup();
	void loop();
};