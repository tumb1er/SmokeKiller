#include <Servo.h>
#include <dht11.h>
#include <IRremote.h>
#include <LiquidCrystalRus.h>

#include "MainController.h"

MainController *ctrl;

void setup()
{
	ctrl = new MainController();
	ctrl->setup();
}

void loop()
{
	ctrl->loop();
}
