#pragma once
const int interruptablePins[2]={2, 3};
const int dlen = 200;
enum IRC_State {IDLE, RECEIVING, RECEIVED};
	
class AsyncIRController {
private:
	bool attached;
	char interrupt;
	volatile IRC_State state;
	int start;
	int end;
	char zero;
	char one;
	volatile unsigned long starttime;
	volatile unsigned long signal;
	int current_pos;
	char pin;
	unsigned long durations[dlen];
	volatile int current;
public:
	AsyncIRController();
	void attach(char IRpin, int start, int end, char zero, char one);
	void detach();
	bool has_signal();
	void process_int();
	void dump();
	void processPulse(unsigned long duration);
};

AsyncIRController AIRC;
