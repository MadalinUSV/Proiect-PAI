
/*
  SMART ANTI-THEFT BOX V4
  Controller: Teensy 4.1

  Reed Switch  -> Pin 2
  Shock Sensor -> Pin 3
  LED          -> Pin 13

  Alarm remains ON while an event is active.
  Alarm turns OFF 4 seconds after all events stop.
*/

const int REED_PIN = 2;
const int SHOCK_PIN = 3;
const int LED_PIN = 13;

const unsigned long debounceDelay = 50;
const unsigned long alarmOffDelay = 4000;

int reedState = HIGH;
int lastReedReading = HIGH;

int shockState = HIGH;
int lastShockReading = HIGH;

unsigned long lastReedDebounce = 0;
unsigned long lastShockDebounce = 0;

unsigned long lastEventTime = 0;

bool alarmActive = false;

void setup() {

  Serial.begin(115200);

  pinMode(REED_PIN, INPUT);
  pinMode(SHOCK_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  Serial.println("============================");
  Serial.println(" SMART ANTI-THEFT BOX V4");
  Serial.println("============================");

  Serial.println("System ready.");

}

void loop() {

  checkReedSensor();
  checkShockSensor();

  // Check if an event is still active
  bool reedTriggered = (reedState == HIGH);

  // Shock sensor is an impulse sensor:
  // a detected shock refreshes lastEventTime.

  bool eventActive = reedTriggered;

  // Alarm turns off only after 4 seconds
  // without any detected event.

  if (alarmActive) {

    if (eventActive) {

      lastEventTime = millis();

    }

    if (millis() - lastEventTime >= alarmOffDelay
        && !eventActive) {

      alarmActive = false;

      Serial.println("[SYSTEM] Alarm stopped.");

    }

  }

  digitalWrite(LED_PIN, alarmActive ? HIGH : LOW);

}

// =================================
// ACTIVATE / REFRESH ALARM
// =================================

void activateAlarm() {

  if (!alarmActive) {
    Serial.println("[ALARM] Alarm activated!");
  }

  alarmActive = true;
  lastEventTime = millis();

}

// =================================
// REED SWITCH
// =================================

void checkReedSensor() {

  int reading = digitalRead(REED_PIN);

  if (reading != lastReedReading) {
    lastReedDebounce = millis();
  }

  if (millis() - lastReedDebounce > debounceDelay) {

    if (reading != reedState) {

      reedState = reading;

      if (reedState == LOW) {

        Serial.println("[REED] BOX CLOSED");

      } else {

        Serial.println("[ALARM] BOX OPENED!");
        activateAlarm();

      }

    }

  }

  lastReedReading = reading;

}

// =================================
// SHOCK SENSOR
// =================================

void checkShockSensor() {

  int reading = digitalRead(SHOCK_PIN);

  if (reading != lastShockReading) {
    lastShockDebounce = millis();
  }

  if (millis() - lastShockDebounce > debounceDelay) {

    if (reading != shockState) {

      shockState = reading;

      if (shockState == LOW) {

        Serial.println("[ALARM] SHOCK DETECTED!");

        activateAlarm();

      }

    }

  }

  lastShockReading = reading;

}
