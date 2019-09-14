#include <Arduino.h>
#include "Keypad.h"
#include "Key.h"
#include <EEPROM.h>
#include <jled.h>

//Commands
#define MAX_NUM_CHARS 11
#define NUM_CHARS_TYPE_1 7
#define NUM_CHARS_TYPE_2 11
#define NUM_CHARS_TYPE_3 8
#define MAX_TIME_JLED 60000
#define MAX_NUM_LAMP 16

char keyPressed;
byte data_count = 0;
char cmd[MAX_NUM_CHARS];

//EEPROM
int addr = 0;
byte value;

//INDICATORS
int greenLED = 51;
int redLED = 53;
int indicatorTime = 100;

enum led_list {
  pin_22, pin_23, pin_24, pin_2, pin_3, 
  pin_4, pin_5, pin_6, pin_7, pin_8, 
  pin_9, pin_10, pin_11, pin_12, pin_13, pin_14
};

// Define leds
JLed leds[25] = { JLed(0).Off(), JLed(0).Off(), JLed(2).Off(), 
  JLed(3).Off(), JLed(4).Off(), JLed(5).Off(), JLed(6).Off(), 
  JLed(7).Off(), JLed(8).Off(), JLed(9).Off(), JLed(10).Off(), 
  JLed(11).Off(), JLed(12).Off(), JLed(13).Off(), JLed(14).Off(), 
  JLed(0).Off(), JLed(0).Off(), JLed(0).Off(), JLed(0).Off(), 
  JLed(0).Off(), JLed(0).Off(), JLed(0).Off(), JLed(22).Off(), 
  JLed(23).Off(), JLed(24).Off()
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

// Usage menu
const char usageText[] PROGMEM = R"=====(
Manual:

Opcion 1: Lampara unica con flasheo continuo
  XX [LAMPARA] * YYY [TIEMPO_ON_OFF (ms)] #  
  // E.G. 12*23# Lampara: 12, Tiempo_ON_OFF: 23

Opcion 2: Lampara unica con flasheo ocasional
  XX [LAMPARA] * YYY[TIEMPO_ON (ms)] * ZZZ[TIEMPO_OFF (s) - máximo 60s] # 
  // E.G. 03*050*060# Lampara 03 con flasheos cada 60s de 50ms

Opcion 3: Dos lamparas con flasheo continuo, sincronizado
  XX [LAMPARA1] YY [LAMPARA2] * ZZ [TIEMPO_ON_OFF (ms)] #; 
  // E.G. 0408*50# Lampara 04 y Lampara 08 con flasheo de 50ms alternando

)=====";

void printUsage() {
  Serial.println((const __FlashStringHelper *)usageText);
}

void save() { //updates addr with values 0-255
    EEPROM.update(addr, value);
    addr = addr + 1;
    if (addr == 200) {
      addr = 0;
    }
}

void testwrite() { //for testing of writing to EEPROM , Update is better for long life EEPROM
  for (int i = 0; i < 255; i++) {
    EEPROM.update(i, i);
  }
}

void read() { //reads everything from EEPROM
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

void clearData(){
  while(data_count !=0){
    cmd[data_count--] = 0;
  }
}

void indicate(int _LED, int _wait) {
  JLed(_LED).Blink(_wait, _wait).Repeat(3);
}

bool validate_input(int _lamp, int _time) {
  bool valid_input;

  if (_lamp <= MAX_NUM_LAMP && _time < MAX_TIME_JLED) {
    Serial.println("Valid input");
    valid_input = true;
  } else if (_lamp > MAX_NUM_LAMP) {
    Serial.println("Number of lamp not valid");
    valid_input = false;
  } else if (_time > MAX_TIME_JLED) {
    Serial.println("Number of lamp not valid");
    valid_input = false;
  }

  return valid_input;
}

void process_command() { //handler for input
  // Remove end character
  String string_raw = cmd;
  string_raw.replace("#","");
  String readString = string_raw;
  Serial.println("Comando recibido:" + readString);

  // Check which mode we are at
  if (data_count == NUM_CHARS_TYPE_1){
    // XX [LAMPARA] * YYY [TIEMPO_ON_OFF (ms)] #  (7)
    // E.G. 12*23# Lampara: 12, Tiempo_ON_OFF: 23
    Serial.println("Modo 1. Flasheo continuo");
    
    int lamp = atoi(readString.substring(0, 2).c_str());
    int on_off_time = atoi(readString.substring(3, 5).c_str());

    if (validate_input(lamp, on_off_time)){
      Serial.println("Lampara:" + String(lamp));
      Serial.println("Tiempo ON-OFF (ms):" + String(on_off_time));
      // Double check this
      JLed(leds[lamp]).Blink(on_off_time,on_off_time).Forever();
      // Save new confing in EEPROM
      // ...
      clearData();
    } else {
      clearData();
      return;
    }

  } else if (data_count == NUM_CHARS_TYPE_2) {
    // XX [LAMPARA] * YYY[TIEMPO_ON (ms)] * ZZZ[TIEMPO_OFF (s)] # (11)
    // E.G. 03*050*060# Lampara 03 con flasheos cada 60s de 50ms
    Serial.println("Mode 2. Flasheo ocasional");
    int lamp = atoi(readString.substring(0, 2).c_str());
    int on_time = atoi(readString.substring(3, 6).c_str());
    int off_time = atoi(readString.substring(7, 10).c_str());
    
    if (validate_input(lamp, max(on_time, off_time*1000))) {
      Serial.println("Lampara:" + String(lamp));
      Serial.println("Tiempo ON (ms):" + String(on_time));
      // Off time is in seconds
      Serial.println("Tiempo OFF (s):" + String(off_time));

      JLed(leds[lamp]).Blink(on_time, off_time*1000).Forever();
      // Save new confing in EEPROM
      // ...
      clearData();
    } else {
      clearData();
      return;
    }

  } else if (data_count == NUM_CHARS_TYPE_3){
    // XX [LAMPARA1] YY [LAMPARA2] * ZZ [TIEMPO_ON_OFF (ms)] # (8)
    // E.G. 0408*50# Lampara 04 y Lampara 08 con flasheo de 50ms alternando
    Serial.println("Mode 3. Sync mode");
    int lamp_1 = atoi(readString.substring(0, 2).c_str());
    int lamp_2 = atoi(readString.substring(2, 4).c_str());
    int on_off_time = atoi(readString.substring(5, 7).c_str());

    if (validate_input(max(lamp_1, lamp_2), on_off_time)) {
      Serial.println("Lampara 01:" + String(lamp_1));
      Serial.println("Lampara 02:" + String(lamp_2));
      Serial.println("Tiempo ON-OFF (ms):" + String(on_off_time));

      // Let see if this works
      JLed(leds[lamp_1]).Blink(on_off_time, on_off_time).Forever();
      delay(on_off_time);
      JLed(leds[lamp_2]).Blink(on_off_time, on_off_time).Forever();
      // Save new confing in EEPROM
      // ...
      clearData();
    } else {
      clearData();
      return;
    }

  } else {
    Serial.println("Unkown mode!");
    clearData();
    indicate(redLED, indicatorTime);
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

  printUsage();

  // leds[8] = JLed(8).Off();

  // Read previously saved EEPROM commands
  // read(); // should load previous config from eeprom
}

void loop() {
  // Read the key
  keyPressed = customKeypad.getKey();
  // If we have something in key, do something with it
  if (keyPressed){
    // Print the key
    Serial.println(keyPressed); //print input
    // Add it to the command
    cmd[data_count] = keyPressed;
    // Increase the count
    data_count++;
    // Check our 'master-key'
    if(keyPressed == '#') {
      // We can only finish a command with a #
      process_command();
      clearData();
    }

    // If we surpass max number of chars, clear data
    if (data_count > MAX_NUM_CHARS) {
      clearData();
    }
  }

  sequence.Update();
  // delay(1);
  // leds.Update();
  // for (auto& led : leds) {led.Update();}
}
