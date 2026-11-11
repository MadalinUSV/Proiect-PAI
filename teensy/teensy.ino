
const int REED_PIN = 2;
const int SHOCK_PIN = 3;
const int TILT_PIN = 4;
const int LED_PIN = 13;

const unsigned long alarmOffDelay = 4000;
const unsigned long debounceDelay = 50;

int reedState = LOW;
int shockState = HIGH;
int tiltState = HIGH;

int lastReed = LOW;
int lastShock = HIGH;
int lastTilt = HIGH;

unsigned long reedTimer = 0;
unsigned long shockTimer = 0;
unsigned long tiltTimer = 0;

unsigned long lastEventTime = 0;

bool alarmActive = false;

void setup() {
  Serial.begin(115200);

  pinMode(REED_PIN, INPUT);
  pinMode(SHOCK_PIN, INPUT);
  pinMode(TILT_PIN, INPUT);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("SMART ANTI-THEFT BOX V6");
}

void loop() {
  unsigned long now = millis();

  // REED
  int r = digitalRead(REED_PIN);

  if (r != lastReed) {
    lastReed = r;
    reedTimer = now;
  }

  if (now - reedTimer >= debounceDelay && r != reedState) {
    reedState = r;

    if (reedState == HIGH) {
      alarmActive = true;
      lastEventTime = now;
      Serial.println("BOX OPENED!");
    }
  }

  // SHOCK
  int s = digitalRead(SHOCK_PIN);

  if (s != lastShock) {
    lastShock = s;
    shockTimer = now;
  }

  if (now - shockTimer >= debounceDelay && s != shockState) {
    shockState = s;

    if (shockState == LOW) {
      alarmActive = true;
      lastEventTime = now;
      Serial.println("SHOCK DETECTED!");
    }
  }

  // TILT
  int t = digitalRead(TILT_PIN);

  if (t != lastTilt) {
    lastTilt = t;
    tiltTimer = now;
  }

  if (now - tiltTimer >= debounceDelay && t != tiltState) {
    tiltState = t;

    if (tiltState == LOW) {
      alarmActive = true;
      lastEventTime = now;
      Serial.println("TILT DETECTED!");
    }
  }

  // ALARM
  bool eventActive = (reedState == HIGH) || (tiltState == LOW);

  if (alarmActive && !eventActive &&
      now - lastEventTime >= alarmOffDelay) {
    alarmActive = false;
    Serial.println("ALARM OFF");
  }

  // LED
  if (alarmActive) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
