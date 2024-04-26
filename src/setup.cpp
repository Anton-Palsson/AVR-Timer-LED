#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

const int ledPin = 3;
int ledPower = 0; // Defa
int onTime = 200; //on time ms
int offTime = 200; //off time
volatile bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);

  // Setup Timer1
  TCCR1A = 0; 
  TCCR1B = 0;
  TCNT1 = 0; //counter
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);

  //Interrupts
  sei();
  Serial.begin(9600);
}

void processSerialCommand() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    Serial.println("Received command: " + command); // Print received command for debugging
    if (command.startsWith("ledpowerfreq")) {
      int spaceIndex = command.indexOf(' ');
      if (spaceIndex != -1) {
        int spaceIndex2 = command.indexOf(' ', spaceIndex + 1);
        if (spaceIndex2 != -1) {
          ledPower = command.substring(spaceIndex + 1, spaceIndex2).toInt();
          int secondValue = command.substring(spaceIndex2 + 1).toInt();
          Serial.println("LED Power: " + String(ledPower)); //debugging
          Serial.println("On/Off Time: " + String(secondValue)); //debugging
          if (secondValue >= 200 && secondValue <= 5000) {
            onTime = secondValue;
            offTime = secondValue;


            unsigned long ocrValue = (16000000UL / (1024UL * secondValue)) - 1;
            OCR1A = ocrValue;
          }
        }
      }
    }
  }
}

void processTimerInterrupt() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  if (ledState == LOW) {
    if (currentTime - lastTime >= offTime) {
      ledState = HIGH;
      lastTime = currentTime;
      analogWrite(ledPin, ledPower);
    }
  } else {
    if (currentTime - lastTime >= onTime) {
      ledState = LOW;
      lastTime = currentTime;
      analogWrite(ledPin, 0);
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  processTimerInterrupt();
}