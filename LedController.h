#pragma once

#include "Timer.h"

class LedController {
	enum LED_STATE {OFF, ON};
private:
	unsigned long starttime;
	unsigned long duration;
	int pinLED;
	LED_STATE state;
public:
	LedController();
	LedController(int pinLED);
	void blinkFor(unsigned long duration);
	void on();
	void off();
	void setup();
	void loop();
};
