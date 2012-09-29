#include "MainController.h"
#define DEBUG 1

MainController::MainController():	
	lcd(LCDpins[0], LCDpins[1], LCDpins[2], LCDpins[3], LCDpins[4], LCDpins[5]),
	pir_sensor(PIRpin, 100, moveTimeout),
	ir_sensor(IRRpin),
	motor(MTRpin, DCpin) {
		sysLed = LedController(LEDpin);
		redLed = LedController(redLED);
		greenLed = LedController(greenLED);
		blueLed = LedController(blueLED);
		blink_start = 0;
		dht_start = 1000;
		lcd_start = 2500;
		remote_start = 0;
		move_start = 0;
		temperature = 0;
		state_start = 0;
		humidity = 0;
		moving = false;
		autoclose_enabled = true;
		ir_command = 0;
		state = MainController::OPEN;
}

void MainController::setup(){
	timer_loop();
#if DEBUG
	Serial.begin(9600);
#endif
	sysLed.setup();
	redLed.setup();
	greenLed.setup();
	blueLed.setup();

	pir_sensor.setup();

	lcd.begin(20, 4);

	ir_sensor.enableIRIn();

	motor.setup();
}

void MainController::loop() {
	timer_loop();
	sysLed.loop();
	redLed.loop();
	greenLed.loop();
	blueLed.loop();
	pir_sensor.loop();
	motor.loop();
	
	check_remote();
	blink_leds();
	check_dht();
	check_move();
	lcd_output();
	process_state();
	delay(10);
}

void MainController::check_dht(){ 
	if (elapsed(dht_start) < 1000)
		return;
	dht_start = timer;
	// метод update заставляет сенсор выдать текущие измерения
	int res = dht_sensor.read(DHTpin);

	switch (res)
	{
	case DHTLIB_OK:
		char msg[128];
		// данные последнего измерения можно считать соответствующими
		// методами
		temperature = dht_sensor.temperature;
		humidity = dht_sensor.humidity;
#if DEBUG
		sprintf(msg, "Temperature = %dC, Humidity = %d%%", 
			temperature, humidity);
		Serial.println(msg);
#endif
		break;
#if DEBUG
	case DHTLIB_ERROR_CHECKSUM:
		Serial.println("Error: checksum");
		break;
	case DHTLIB_ERROR_TIMEOUT:
		Serial.println("Error: read timeout");
		break;
#endif
	}
}

void MainController::blink_leds(){
	if(elapsed(blink_start) < 2000)
		return;
	blink_start = timer;
	sysLed.blinkFor(10);
}

void MainController::lcd_output(){
	if (elapsed(lcd_start) < 1000)
		return;
	lcd_start = timer;
	char msg[21];
	lcd.setCursor(0, 0);

	sprintf(msg, "  Температура: %dC", temperature); 
	lcd.print(msg);
	lcd.setCursor(0, 1);
	sprintf(msg, "   Влажность: %d%%", humidity); 
	lcd.print(msg);
	lcd.setCursor(0, 2);
	if (moving) {
		lcd.print("    [ Движение ]");
		redLed.blinkFor(500);
	} else {
		lcd.print("                ");
	} 
	lcd.setCursor(6,3);
	if (ir_command != 0){
		greenLed.blinkFor(500);
		sprintf(msg, "    [%.8lX]", ir_command);
		lcd.print(msg);
	} else {
	    lcd.print("                ");
	}
	lcd.setCursor(0,3);
	if (autoclose_enabled){
		lcd.print("[ВКЛ]");
	} else {
		lcd.print("[ВЫКЛ]");
	}
}

void MainController::check_move(){
	if (elapsed(move_start) < 1000)
		return;
	move_start = timer;
	moving = pir_sensor.is_moving();
	if (moving)
		process_move();

#if DEBUG
	int cnt = 0;
	for (int i = pir_sensor.cur_event; i < pir_sensor.cur_event + 16; i++) {
		Serial.print(pir_sensor.events[i % 16]);
		if (pir_sensor.events[i % 16])
			cnt++;
	}
	Serial.print(" (");
	Serial.print(cnt);
	Serial.println(")");
#endif
}

void MainController::check_remote(){
	if (ir_sensor.decode(&ir_results)) {
		unsigned long value = ir_results.value;
		if (value != 0xFFFFFFFF){
			ir_command = value;
			remote_start = timer;
#if DEBUG
			Serial.println(ir_command, HEX);
#endif	
			process_remote(ir_command);
		}
		ir_sensor.resume(); // Receive the next value
	} 
	unsigned long el = elapsed(remote_start);
	if (el > 10000) {
		ir_command = 0;
	}
}
void MainController::print_state(State state){
	switch(state) {
	case MainController::OPEN:
		Serial.print("OPEN");
		break;
	case MainController::OPENING:
		Serial.print("OPENING");
		break;
	case MainController::HALFOPEN:
		Serial.print("HALFOPEN");
		break;
	case MainController::STEPOPEN:
		Serial.print("STEPOPEN");
		break;
	case MainController::STEPCLOSE:
		Serial.print("STEPCLOSE");
		break;
	case MainController::CLOSING:
		Serial.print("CLOSING");
		break;
	case MainController::CLOSING_MOVE:
		Serial.print("CLOSING_MOVE");
		break;
	case MainController::CLOSED:
		Serial.print("CLOSED");
		break;
	case MainController::CLOSED_MOVE:
		Serial.print("CLOSED_MOVE");
		break;
	}
}
void MainController::process_remote(unsigned long cmd) {
#if DEBUG
	Serial.print("State: ");
	print_state(state);
	Serial.println();
#endif
	switch(cmd) {
		case IRAutomove:
			autoclose_enabled = !autoclose_enabled;
			break;
		case IRclose:
			switch (state) {
			case MainController::OPEN:
				change_state(MainController::CLOSING);
				break;
			case MainController::OPENING:
			case MainController::CLOSING:
				change_state(MainController::HALFOPEN);
				break;
			case MainController::HALFOPEN:
				change_state(MainController::STEPCLOSE);
				break;
			}
			break;
		case IRopen:
			switch (state) {
			case MainController::CLOSED:
				change_state(MainController::OPENING);
				break;
			case MainController::OPENING:
			case MainController::CLOSING:
				change_state(MainController::HALFOPEN);
				break;
			case MainController::HALFOPEN:
				change_state(MainController::STEPOPEN);
				break;
			case MainController::CLOSED_MOVE:
				change_state(MainController::OPENING);
				break;
			}
			break;
		case IRisopened:
			change_state(MainController::OPEN);
			break;
		case IRisclosed:
			change_state(MainController::CLOSED);
			break;
#if DEBUG
		default:
			Serial.println("No state change");
			break;
#endif

	}
}

void MainController::process_move() {
	if (state == MainController::OPEN && autoclose_enabled)
		change_state(MainController::CLOSING_MOVE);
}

void MainController::process_state() {
	switch(state) {
	case MainController::OPENING:
		if (elapsed(state_start) >= Topen)
			change_state(MainController::OPEN);
		break;
	case MainController::CLOSING_MOVE:
		if (elapsed(state_start) >= Tclose)
			change_state(MainController::CLOSED_MOVE);
		break;
	case MainController::CLOSING:
		if (elapsed(state_start) >= Tclose)
			change_state(MainController::CLOSED);
		break;
	case MainController::STEPOPEN:
		if (elapsed(state_start) >= Tstepopen)
			change_state(MainController::HALFOPEN);
		break;
	case MainController::STEPCLOSE:
		if (elapsed(state_start) >= Tstepclose)
			change_state(MainController::HALFOPEN);
		break;
	case MainController::CLOSED_MOVE:
		if (elapsed(state_start) >= Tmove)
			change_state(MainController::OPENING);
		break;
	case MainController::OPEN:
	case MainController::CLOSED:
	case MainController::HALFOPEN:
		break;
	}
}
void MainController::change_state(State state) {
	switch(state) {
	case MainController::OPEN:
	case MainController::CLOSED:
	case MainController::CLOSED_MOVE:
	case MainController::HALFOPEN:
		motor.stop();
		break;
	case MainController::CLOSING:
	case MainController::CLOSING_MOVE:
		motor.forward(Tclose);
		break;
	case MainController::OPENING:
		motor.backward(Topen);
		break;
	case MainController::STEPOPEN:
		motor.backward(Tstepopen);
		break;
	case MainController::STEPCLOSE:
		motor.forward(Tstepopen);
		break;
	}
#if DEBUG
	Serial.print("Changing state from ");
	print_state(this->state);
	Serial.print(" to ");
	print_state(state);
	Serial.println();
#endif
	this->state = state;
	state_start = timer;
}