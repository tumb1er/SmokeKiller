#include "AsyncIRC.h"

unsigned long elapsedSince(unsigned long since, unsigned long now)
{
	return since < now ? now-since : 0xFFFFFFFFUL - (now - since);
};

unsigned long elapsedSince(unsigned long since)
{
	return elapsedSince( since, micros() );
};


void process_interrupt(){
	AIRC.process_int();
};

void AsyncIRController::attach(char IRpin, int start, int end, char zero, char one){
	if (this->attached)
		return;
	switch (IRpin) {
	case 2:
		this->interrupt = INT0;
		this->attached = true;
		break;
	case 3:
		this->interrupt = INT1;
		this->attached = true;
		break;
	default:
		this->attached = false;
		return;
	}
	this->pin = IRpin;
	this->start = start;
	this->end = end;
	this->zero = zero;
	this->one = one;
	//attachInterrupt(this->interrupt, process_interrupt, CHANGE);
	Serial.println("OK");
}

void AsyncIRController::detach(){
}

bool AsyncIRController::has_signal(){
}

AsyncIRController::AsyncIRController(){
	attached = false;
	state = IDLE;
	current = 0;
	for(int i=0; i< dlen; i++){
		durations[i] = 0;
	}
}

void AsyncIRController::process_int(){
	bool transmitting = (PIND & (1<<pin)) == 0;
	starttime = micros();
	if (transmitting){
		switch(state){
		case IDLE:
		case RECEIVED:
			state = RECEIVING;
			current = 0;
			signal = 0;
			break;
		case RECEIVING:
			break;
		}
	} else {
		unsigned long duration = elapsedSince(starttime);
		switch(state){
		case IDLE:
		case RECEIVED:
			break;
		case RECEIVING:
			processPulse(duration);
		}
	}
}

void AsyncIRController::processPulse(unsigned long duration){
	if (current < dlen){
		durations[current++] = duration;
	}
}
void AsyncIRController::dump(){
	if (current) {
		for (int i=0; i < current; i++) {
			Serial.print(durations[i], DEC);
			Serial.print(';');
		}
		Serial.println();
		current = 0;
	}
	else {
		Serial.println("No signal");
	}
}

int led = 0;
ISR( INT0_vect )
{
	digitalWrite(13, led);
	led = !led;
	AIRC.process_int();
}