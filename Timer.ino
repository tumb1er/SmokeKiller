#include "Timer.h"

unsigned long elapsed(unsigned long start){
	unsigned long now = timer;
	unsigned long res = now - start;
	if (res >= 0)
		return res;
	return 0xFFFFFFFF - res;
};

void timer_loop() {
	timer = millis();
}