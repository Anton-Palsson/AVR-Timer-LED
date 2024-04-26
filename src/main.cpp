#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <setup.h>

int main() {

  init();

  setup();
  
  while (true) {
    processSerialCommand();
  }
  return 0;
}
