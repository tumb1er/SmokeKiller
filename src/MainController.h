#pragma once
#include "Timer.h"
#include "LedController.h"
#include "PIRController.h"
#include "MotorController.h"

// LED pins
#define LEDpin 13

#define blueLED 10
#define redLED 11
#define greenLED 9

// Sensor pins
#define IRRpin 2
#define PIRpin 3
#define DHTpin A0

// Servo pins
#define MTRpin A2
#define DCpin A3

#define moveTimeout 1000


#define IRopen 0x40BF7887
#define IRclose 0x40BFF807
#define IRisopened 0x40BFE817
#define IRisclosed 0x40BF08F7
#define Tstepopen 500
#define Tstepclose 500
#define Topen 15000
#define Tclose 15000
#define Tmove 300000

class MainController {
	enum State {OPEN, HALFOPEN, OPENING, STEPOPEN, CLOSING_MOVE, CLOSED, CLOSED_MOVE, CLOSING, STEPCLOSE};
	static const int LCDpins[];
	LedController sysLed, redLed, greenLed, blueLed;
	unsigned long blink_start;
	unsigned long dht_start;
	unsigned long lcd_start;
	unsigned long move_start;
	unsigned long remote_start;
	unsigned long state_start;
	bool moving;
	int temperature;
	int humidity;
	dht11 dht_sensor;
	LiquidCrystalRus lcd;
	PIRController pir_sensor;
	MotorController motor;
	IRrecv ir_sensor;
	decode_results ir_results;
	unsigned long ir_command;
	State state;
public:
	MainController();
	void setup();
	void loop();
	void blink_leds();
	void check_dht();
	void check_move();
	void lcd_output();
	void check_remote();
	void process_remote(unsigned long cmd);
	void process_state();
	void process_move();
	void change_state(State state);
	void print_state(State state);
};

// LC contacts RS, E, DB5, DB6, DB7, DB8
const int MainController::LCDpins[] = {12, 8, 7, 6, 5, 4};

