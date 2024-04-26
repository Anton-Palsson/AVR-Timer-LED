#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

const int ledPin = 3;
int ledPower = 0; // Defa
int onTime = 200; //on time ms
int offTime = 200; //off time
volatile bool ledState = LOW;

void setup() {
  DDRD |= (1 << DDD3);

  // Setup Timer1
  TCCR1A = 0; 
  TCCR1B = 0;
  TCNT1 = 0; //counter
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 &= ~(1 << OCIE1A);

  //Interrupts
  sei();
  Serial.begin(9600);
}

void processSerialCommand() {
  if (Serial.available() > 0) {
    char command[50]; // Maximal längd på kommandot
    if (Serial.readBytesUntil('\n', command, sizeof(command)) > 0) {
      Serial.println("Received command: " + String(command));
      if (strncmp(command, "ledpowerfreq", 12) == 0) {
        int ledPowerInput, secondValue;
        if (sscanf(command, "ledpowerfreq %d %d", &ledPowerInput, &secondValue) == 2) {
          ledPower = ledPowerInput;
          Serial.println("LED Power: " + String(ledPower)); // debugging
          Serial.println("On/Off Time: " + String(secondValue)); // debugging
          if (secondValue >= 200 && secondValue <= 5000) {
            onTime = secondValue;
            offTime = secondValue;
            if (ledPower == 0 && secondValue == 0) {
              Serial.println("Turning off LED"); // debugging
              PORTD &= ~(1 << PORTD3);
              return; 
            }
           
            TIMSK1 |= (1 << OCIE1A);
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
  
  
  if (ledPower == 0 && onTime == 0 && offTime == 0) {
    TIMSK1 &= ~(1 << OCIE1A);
    PORTD &= ~(1 << PORTD3);
    OCR1A = 0;
    return;
  }
  
  if (ledState == LOW) {
    if (currentTime - lastTime >= offTime) {
      ledState = HIGH;
      lastTime = currentTime;
      PORTD |= (1 << PORTD3); // Inge digitalwrite
      OCR1A = (ledPower * 255) / 100;
    }
  } else {
    if (currentTime - lastTime >= onTime) {
      ledState = LOW;
      lastTime = currentTime;
      PORTD &= ~(1 << PORTD3);
      OCR1A = 0;
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  processTimerInterrupt();
}

