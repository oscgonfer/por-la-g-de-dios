#include <Arduino.h>
#include <EEPROM.h>
int addr = 0;
int zero = 32;
byte value;

void save(void){
    EEPROM.update(addr, value);
    addr = addr + 1;
    if (addr == EEPROM.length()) {
      addr = 0;
    }
}

void testwrite(void) {
  for (int i = 0; i < 255; i++) {
    EEPROM.update(i, i);
  }
}

void read(void) {
  value = EEPROM.read(addr);

  Serial.print(addr);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();

  addr = addr + 1;
  if (addr == EEPROM.length()) {
    addr = 0;
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // EEPROM.get(addr, zero); //dunno what this does functionally
}

void loop() {
  read();
  delay(3000);
  // testwrite();
}
