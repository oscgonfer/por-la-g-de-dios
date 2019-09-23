#include <Arduino.h>
#include "Keypad.h"
#include "Key.h"
#include <EEPROM.h>
#include <jled.h>

//Commands
#define MAX_NUM_CHARS 11
#define NUM_CHARS_TYPE_1 7
#define NUM_CHARS_TYPE_2 11
#define NUM_CHARS_TYPE_3 9
#define NUM_CHARS_RESET 4
#define MAX_TIME_JLED 60000
#define MAX_NUM_LAMP 16

char keyPressed;
byte data_count = 0;
char cmd[MAX_NUM_CHARS];

//INDICATORS
int greenLED = 34;
int redLED = 32;
int indicatorTime = 1000;

// enum led_list {
//   pin_22, pin_23, pin_24, pin_2, pin_3,
//   pin_4, pin_5, pin_6, pin_7, pin_8,
//   pin_9, pin_10, pin_11, pin_12, pin_13, pin_14
// };

// Define leds
JLed leds[35] = { JLed(0).On(), JLed(13).On(), JLed(12).On(),
  JLed(11).On(), JLed(10).On(), JLed(9).On(), JLed(8).On(),
  JLed(7).On(), JLed(6).On(), JLed(5).On(), JLed(4).On(),
  JLed(3).On(), JLed(2).On(), JLed(22).On(), JLed(24).On(),
  JLed(26).On(), JLed(28).On(), JLed(0).On(), JLed(0).On(),
  JLed(0).On(), JLed(0).On(), JLed(0).On(), JLed(0).On(),
  JLed(0).On(), JLed(0).On(), JLed(0).On(), JLed(0).On(), 
  JLed(0).On(), JLed(0).On(), JLed(0).On(), JLed(0).On(), 
  JLed(0).On(), JLed(32).Off(), JLed(0).On(), JLed(34).Off()
};

// Keypad
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {

  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {52, 50, 48, 46};
byte colPins[COLS] = {44, 42, 40};

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

JLedSequence sequence(JLedSequence::eMode::PARALLEL, leds);
JLedSequence led_breathe(JLedSequence::eMode::PARALLEL, leds);

// Usage menu
const char usageText[] PROGMEM = R"=====(
Manual:

Opcion 1: Lampara unica con flasheo continuo
  XX [LAMPARA] * YYY [TIEMPO_ON_On (ms) - maximo 250ms] #  
  // E.G. 12*023# Lampara: 12, Tiempo_ON_OFF: 23 ms

Opcion 2: Lampara unica con flasheo ocasional
  XX [LAMPARA] * YYY[TIEMPO_ON (ms) - maximo 250ms] * ZZZ[TIEMPO_OFF (s) - máximo 60s] #
  // E.G. 03*050*060# Lampara 03 con flasheos cada 60s de 50ms

Opcion 3: Dos lamparas con flasheo continuo, sincronizado
  XX [LAMPARA1] YY [LAMPARA2] * ZZZ [TIEMPO_ON_OFF (ms) - maximo 250ms ] #;
  // E.G. 0408*050# Lampara 04 y Lampara 08 con flasheo de 50ms alternando

Opcion 4: RESET de memoria
  ***#


)=====";


void printUsage() {
  Serial.println((const __FlashStringHelper *)usageText);
}

int lamp_eeprom_addr(int _lamp_number) {
  return (_lamp_number-1)*6;
}

void save_lamp(int _eeprom_init_addr, uint16_t _on_time, uint16_t _off_time, byte _synced_lamp) { //updates addr with values 0-255
    // Pointless check
    if (_eeprom_init_addr + 5 > EEPROM.length()) {
      return;
    }
    
    // Time can be up to uint16_t, or two bytes - Double check this with prints
    // Get first part of _on_time
    byte _on_time_part_1 = (_on_time >> 8) & 0xff;
    // Get second part of _on_time
    byte _on_time_part_2 = _on_time & 0xff;
    // Get first part of _off_time
    byte _off_time_part_1 = (_off_time >> 8) & 0xff;
    // Get second part of _off_time
    byte _off_time_part_2 = _off_time & 0xff;

    // Write all three things
    EEPROM.update(_eeprom_init_addr, _on_time_part_1);
    delay(100);
    EEPROM.update(_eeprom_init_addr + 1 , _on_time_part_2);
    delay(100);
    EEPROM.update(_eeprom_init_addr + 2, _off_time_part_1);
    delay(100);
    EEPROM.update(_eeprom_init_addr + 3, _off_time_part_2);
    delay(100);
    EEPROM.update(_eeprom_init_addr + 4, _synced_lamp);
    delay(100);

    // Update addr for initialised eeprom if necessary
    EEPROM.update(_eeprom_init_addr + 5, 0xff);
}

void init_lamps_eeprom() {

  // Iterate each saved lamp
  for (int i = 0; i <(MAX_NUM_LAMP-1)*6; i+=6) {
    int lamp = i/6 + 1;

    // We check if the i + 5 byte is a known value, otherwise skip lamp
    if (EEPROM.read(i + 5) == 0xff){
      byte _on_time_part_1 = EEPROM.read(i);
      byte _on_time_part_2 = EEPROM.read(i + 1);
      byte _off_time_part_1 = EEPROM.read(i + 2);
      byte _off_time_part_2 = EEPROM.read(i + 3);
      byte _synced_lamp = EEPROM.read(i + 4);

      // Parse _on_time
      uint16_t _on_time = _on_time_part_1 << 8;
      _on_time += _on_time_part_2;

      // Parse off_time
      uint16_t _off_time = _off_time_part_1 << 8;
      _off_time += _off_time_part_2;

      // Update LEDs (off time is in ms already)
      Serial.println("Initialising EEPROM value for lamp " + String(lamp));
      Serial.println("ON Time (ms): " + String(_on_time));
      Serial.println("OFF (ms): " + String(_off_time));  
      // Make first lamp blink
      leds[lamp].Blink(_off_time, _on_time).Forever();
      // Check if there is a synced lamp
      if (_synced_lamp != 0xff) {
        int lamp_2 = int(_synced_lamp);
        Serial.println("Initialising EEPROM value for synced lamp " + String(lamp_2));
        leds[lamp_2].Blink(_off_time, _on_time).Forever().DelayBefore(_on_time);
      }
    } else {
      Serial.println("Lamp " + String(lamp) + " has never been init, skipping");
    }
  }
  Serial.println("Init Lamps done");
}

void clearData(){
  while(data_count !=0){
    cmd[data_count--] = 0;
  }
}

void indicate(int _LED, int _wait) {
  // JLed(_LED).Blink(_wait, _wait).Repeat(3);
  leds[_LED].Breathe(_wait).Repeat(3);
}

bool validate_input(int _lamp, int _time) {
  bool valid_input;

  if (_lamp <= MAX_NUM_LAMP && _time <= MAX_TIME_JLED) {
    Serial.println("Entrada valida");
    valid_input = true;
  } else if (_lamp > MAX_NUM_LAMP) {
    Serial.println("Numero de lampara no valido");
    valid_input = false;
  } else if (_time > MAX_TIME_JLED) {
    Serial.println("Duracion no valida");
    valid_input = false;
  }

  return valid_input;
}

void EEPROM_clear(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void process_command() {
  // Remove end character
  String string_raw = cmd;
  string_raw.replace("#","");
  String readString = string_raw;
  Serial.println("Comando recibido: " + readString);

  // Check which mode we are at
  if (data_count == NUM_CHARS_TYPE_1){
    // XX [LAMPARA] * YYY [TIEMPO_ON_OFF (ms)] #  (7)
    // E.G. 12*23# Lampara: 12, Tiempo_ON_OFF: 23
    Serial.println("Modo 1. Flasheo continuo");

    int lamp = atoi(readString.substring(0, 2).c_str());
    uint16_t on_off_time = atoi(readString.substring(3, 6).c_str());

    if (validate_input(lamp, on_off_time)){
      Serial.println("Lampara: " + String(lamp));
      Serial.println("Tiempo ON-OFF (ms): " + String(on_off_time));
      // Double check this
      leds[lamp].Blink(on_off_time, on_off_time).Forever();
      // Save new config in EEPROM
      save_lamp(lamp_eeprom_addr(lamp), on_off_time, on_off_time, 0xff);
      clearData();
      indicate(greenLED, indicatorTime);
    } else {
      Serial.println("Entrada no valida");
      indicate(redLED, indicatorTime);
      clearData();
      return;
    }

  } else if (data_count == NUM_CHARS_TYPE_2) {
    // XX [LAMPARA] * YYY[TIEMPO_ON (ms)] * ZZZ[TIEMPO_OFF (s)] # (11)
    // E.G. 03*050*060# Lampara 03 con flasheos cada 60s de 50ms
    Serial.println("Modo 2. Flasheo ocasional");
    int lamp = atoi(readString.substring(0, 2).c_str());

    uint16_t on_time = atoi(readString.substring(3, 6).c_str());
    uint16_t off_time = atoi(readString.substring(7, 10).c_str());

    if (validate_input(lamp, max(on_time, off_time*1000))) { // Off time is in seconds

      Serial.println("Lampara: " + String(lamp));
      Serial.println("Tiempo ON (ms): " + String(on_time));
      // Off time is in seconds
      Serial.println("Tiempo OFF (s): " + String(off_time));

      leds[lamp].Blink(off_time*1000, on_time).Forever();
      // Save new confing in EEPROM
      save_lamp(lamp_eeprom_addr(lamp), on_time, off_time*1000, 0xff);
      clearData();
      indicate(greenLED, indicatorTime);
    } else {
      Serial.println("Entrada no valida");
      indicate(redLED, indicatorTime);
      clearData();
      return;
    }

  } else if (data_count == NUM_CHARS_TYPE_3){
    // XX [LAMPARA1] YY [LAMPARA2] * ZZ [TIEMPO_ON_OFF (ms)] # (8)
    // E.G. 0408*50# Lampara 04 y Lampara 08 con flasheo de 50ms alternando
    Serial.println("Modo 3. Mode alternado");
    int lamp_1 = atoi(readString.substring(0, 2).c_str());
    int lamp_2 = atoi(readString.substring(2, 4).c_str());
    uint16_t on_off_time = atoi(readString.substring(5, 8).c_str());

    if (validate_input(max(lamp_1, lamp_2), on_off_time)) {
      Serial.println("Lampara 01: " + String(lamp_1));
      Serial.println("Lampara 02: " + String(lamp_2));
      Serial.println("Tiempo ON-OFF (ms): " + String(on_off_time));

      // Let see if this works
      leds[lamp_1].Blink(on_off_time, on_off_time).Forever();
      leds[lamp_2].Blink(on_off_time, on_off_time).Forever().DelayBefore(on_off_time);

      // Save new confing in EEPROM (always save the )
      save_lamp(lamp_eeprom_addr(min(lamp_1, lamp_2)), on_off_time, on_off_time, byte(max(lamp_1, lamp_2)));
      clearData();
      indicate(greenLED, indicatorTime);
    } else {
      Serial.println("Entrada no valida");
      indicate(redLED, indicatorTime);
      clearData();
      return;
    }
  } else if (data_count == NUM_CHARS_RESET) {
    // Memory erase
    if (readString == "***") {
      Serial.println("Borrando memoria");
      EEPROM_clear();
      indicate(greenLED, indicatorTime);
      clearData();
      return;
    }  
  } else {
    Serial.println("Modo desconocido!");
    indicate(redLED, indicatorTime);
    clearData();
    return;
  }
}

//----------SETUP-&-LOOP-----------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // Init Leds
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);

  delay(1000);

  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, HIGH);

  delay(1000);

  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);

  printUsage();

  // Read previously saved EEPROM commands
  init_lamps_eeprom();

}

void loop() {
  // Read the key
  keyPressed = customKeypad.getKey();
  // If we have something in key, do something with it
  if (keyPressed){
    // Print the key
    Serial.print(keyPressed); //print input
    // Add it to the command
    cmd[data_count] = keyPressed;
    // Increase the count
    data_count++;
    // Check our 'master-key'
    if(keyPressed == '#') {
      Serial.println("");
      // We can only finish a command with a #
      process_command();
      clearData();
    }

    // If we surpass max number of chars, clear data
    if (data_count > MAX_NUM_CHARS) {
      Serial.println("");
      Serial.println("Comando demasiado largo sin caracter #");
      indicate(redLED, indicatorTime);
      clearData();
    }
  }
  // Update the sequence
  sequence.Update();
}
