
#include <SoftwareSerial.h>

#define TRIG_PIN 9
#define ECHO_PIN 10

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("Arduino with HC-SR04 ready");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "start") {
      Serial.println("Start command received");
      float sum = 0.0;
      int validMeasurements = 0;
      const int numMeasurements = 10;
      
      for (int i = 0; i < numMeasurements; i++) {
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        long duration = pulseIn(ECHO_PIN, HIGH);
        float distanceCm = duration * 0.034 / 2;
        
        if (distanceCm > 2 && distanceCm < 400) {
          sum += distanceCm;
          validMeasurements++;
        }
        delay(50);
      }
      
      if (validMeasurements > 0) {
        float averageCm = sum / validMe asurements;
        int averageMm = (int)(averageCm * 10);
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%d", averageMm); 
        Serial.println(buffer);
        Serial.print("Sent measurement: ");
        Serial.println(buffer);
      } else {
        Serial.println("0");
        Serial.println("No valid measurements, sent 0");
      }
    }
  }
}