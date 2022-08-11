/**
* Arduino (C++ framework) based project. This code is used to run a program which pumps water in intervals according to the moisture level detected by a sensor connected to dirt/soil.
* Written by Ken Zhu and Eric McKanna
*/

// Establishing pin variables to our different pumps buttons and sensors
const int SYSTEM_BUTTON_PIN = 4;

const int PUMP_ONE_BUTTON_PIN = 6;
const int PUMP_TWO_BUTTON_PIN = 5;

const int PUMP_ONE = 2;
const int PUMP_TWO = 3;

const int SENSOR_ONE = A4;
const int SENSOR_TWO = A5;

const int DEFAULT_PUMP_STATE = LOW;

// The number of “frames (50 ms for one frame) before the water is shut off
const int FRAMES_BEFORE_SHUTOFF = 200;

// The number of frames total before the pump is allowed to restart again
const int FRAMES_BEFORE_START_ALLOWED = 200 + FRAMES_BEFORE_SHUTOFF;

// Wilting point of soil (where it is too dry, dependent on analog read from the sensor)
const int THRESHOLD_SATURATION = 432;

// Limited storage data in Arduino, reset frame variable to prevent overflow
const int RESTART_FRAME = 32000;

// Initializing and delcaring button states
int systemState = 0;

int pumpOneState = 0;
int pumpTwoState = 0;

int sensorOneValue = 0;
int sensorTwoValue = 0;

int currentSystemButtonState;
int currentPumpOneButtonState;
int currentPumpTwoButtonState;

int lastSystemButtonState;
int lastPumpOneButtonState;
int lastPumpTwoButtonState;

int frame = 0;
int pumpOneLastFrameRan = -1;
int pumpTwoLastFrameRan = -1;
int pumpOnePinState = DEFAULT_PUMP_STATE;
int pumpTwoPinState = DEFAULT_PUMP_STATE;

// Setting up the system power outputs and reads of the system
void setup() {
	Serial.begin(9600);
	pinMode(SYSTEM_BUTTON_PIN, INPUT_PULLUP);
	pinMode(PUMP_ONE_BUTTON_PIN, INPUT_PULLUP);
	pinMode(PUMP_TWO_BUTTON_PIN, INPUT_PULLUP);

	pinMode(PUMP_ONE, OUTPUT);
	pinMode(PUMP_TWO, OUTPUT);

	pinMode(SENSOR_ONE, INPUT);
	pinMode(SENSOR_TWO, INPUT);

	currentSystemButtonState = digitalRead(SYSTEM_BUTTON_PIN);
	currentPumpOneButtonState = digitalRead(PUMP_ONE_BUTTON_PIN);
	currentPumpTwoButtonState = digitalRead(PUMP_TWO_BUTTON_PIN);
}

// Main function, adds a singular frame with each loop
void loop() {

	frame++;

	lastSystemButtonState = currentSystemButtonState;
	lastPumpOneButtonState = currentPumpOneButtonState;
	lastPumpTwoButtonState = currentPumpTwoButtonState;

	// Reading button pins for any changes
	currentSystemButtonState = digitalRead(SYSTEM_BUTTON_PIN);
	currentPumpOneButtonState = digitalRead(PUMP_ONE_BUTTON_PIN);
	currentPumpTwoButtonState = digitalRead(PUMP_TWO_BUTTON_PIN);

	// Reading sensor values for any changes
	sensorOneValue = analogRead(SENSOR_ONE);
	sensorTwoValue = analogRead(SENSOR_TWO);
	// If the system state button is pressed, change the status of the system state
	if (lastSystemButtonState == HIGH && currentSystemButtonState == LOW) {
		systemState = !systemState;
		if (systemState == 1) {
			Serial.print("SYSTEM IS ON . . .\n");
		}
		if (systemState == 0) {
			Serial.print("SYSTEM SHUTTING DOWN . . .\n");
		}
	}

	// Pump button state
	if (lastPumpOneButtonState == HIGH && currentPumpOneButtonState == LOW) {
		pumpOneState = !pumpOneState;
		Serial.print("PUMP ONE ENABLED . . .\n");
	}

	// Pump button state
	if (lastPumpTwoButtonState == HIGH && currentPumpTwoButtonState == LOW) {
		pumpTwoState = !pumpTwoState;
		Serial.print("PUMP TWO ENABLED . . .\n");

	}

	// If the system is off then the pump states should not be functioning
	if (systemState == 0) {
		//  Serial.print("systemState is 0\n"); // Code for debugging
		digitalWrite(PUMP_ONE, LOW);
		digitalWrite(PUMP_TWO, LOW);
		pumpOnePinState = LOW;
		pumpTwoPinState = LOW;
	}
	// If the system is on and frames have surpassed the pump active timer then the pumps(s) will shut off
	else {
		if (pumpOnePinState == HIGH && frame - pumpOneLastFrameRan > FRAMES_BEFORE_SHUTOFF) {
			Serial.print("SHUTTING OFF PUMP ONE . . .\n");
			digitalWrite(PUMP_ONE, LOW);
			pumpOnePinState = LOW;
		}
		if (pumpTwoPinState == HIGH && frame - pumpTwoLastFrameRan > FRAMES_BEFORE_SHUTOFF) {
			Serial.print("SHUTTING OFF PUMP TWO . . .\n");
			digitalWrite(PUMP_TWO, LOW);
			pumpTwoPinState = LOW;
		}

		// Debugging code
	  // Serial.print("\nSENSOR CHECK: \n");
	  // Serial.print(sensorOneValue > THRESHOLD_SATURATION);
	  // Serial.print("\nFRAME CHECK: \n");
	  // Serial.print(frame - pumpOneLastFrameRan > FRAMES_BEFORE_START_ALLOWED);
	  // Serial.print("\n");

		// Turn the pump on if the button is on and sensor value reads a soil level that is more dry than our threshold wilting point and if there are available frames for the pump to run
		if (pumpOnePinState != HIGH && sensorOneValue > THRESHOLD_SATURATION && frame - pumpOneLastFrameRan > FRAMES_BEFORE_START_ALLOWED) {
			if (pumpOneState != 0) {
				Serial.print("RUNNING PUMP ONE . . .\n");
				//Serial.print(sensorOneValue); // Debugging code
				//Serial.print("\n"); // Debugging Code
				digitalWrite(PUMP_ONE, HIGH);
				pumpOnePinState = HIGH;
				pumpOneLastFrameRan = frame;
			}
		}
		// Same as above but for pump two
		if (pumpTwoPinState != HIGH && sensorTwoValue > THRESHOLD_SATURATION && frame - pumpTwoLastFrameRan > FRAMES_BEFORE_START_ALLOWED) {
			if (pumpTwoState != 0) {
				Serial.print("RUNNING PUMP TWO . . .\n");
				digitalWrite(PUMP_TWO, HIGH);
				pumpTwoPinState = HIGH;
				pumpTwoLastFrameRan = frame;
			}
		}
	}

	// Reintializing frame variables to 0
	if (frame == RESTART_FRAME) {
		Serial.print("RESTARTING SYSTEM TO PREVENT VARIABLE OVERFLOW.");
		frame = 0;
		pumpOneLastFrameRan = 0;
		pumpTwoLastFrameRan = 0;
	}

	// This sets the duration of each frame
	delay(50);
}




