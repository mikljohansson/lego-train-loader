#include <Arduino.h>
#include <Servo.h>

#define MOTOR_PIN   5
#define SERVO_PIN   9
#define LED_PIN     12

#define SERVO_RAISED_POS  125
#define SERVO_LOADING_POS 75
#define SERVO_DELAY       25
#define HALL_SENSOR_PIN   A0
#define LOADING_DELAY     2500
#define LOADING_DURATION  10000
#define MOTOR_POWER       175

Servo servo1;
long duration = 0;

void setup() {
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(HALL_SENSOR_PIN, INPUT);
}

bool inPosition() {
    float mag = (float)analogRead(HALL_SENSOR_PIN) / 1024;
    bool inpos = mag < 0.1;
    digitalWrite(LED_PIN, inpos ? HIGH : LOW);
    return inpos;
}

void load() {
    // Lower loader into position
    int pos = SERVO_RAISED_POS;
    servo1.attach(SERVO_PIN);
    
    for (; pos >= SERVO_LOADING_POS; pos -= 1) {
        // Abort loading if train moves away
        if (!inPosition()) {
          break;
        }

        servo1.write(pos);
        delay(SERVO_DELAY);
    }

    // Start band moving
    analogWrite(MOTOR_PIN, MOTOR_POWER);

    // Load cargo
    for (int i = 0; i < 100; i++) {
      // Abort loading if train moves away
      if (!inPosition()) {
        break;
      }
      
      delay(LOADING_DURATION / 100);
    }

    // Stop band
    analogWrite(MOTOR_PIN, 0);
   
    // Raise loader
    for (; pos <= SERVO_RAISED_POS; pos += 1) {
      servo1.write(pos);
      delay(SERVO_DELAY);
    }
    servo1.detach();

    // Wait for train to move away
    int measurements = 0;
    while (measurements < 10) {
      // Abort loading if train moves away
      if (!inPosition()) {
        measurements++;
      }
      
      delay(100);
    }
}

void loop() {
    // See if train has been positioned over sensor for long enough to trigger loading
    duration = inPosition() ? duration + 1 : 0;
    
    if (duration > LOADING_DELAY / 100) {
      load();
      duration = 0;
    }
    else {
      delay(LOADING_DELAY / 100);
    }
}
