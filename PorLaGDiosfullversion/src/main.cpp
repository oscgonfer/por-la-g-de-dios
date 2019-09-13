#include <Arduino.h>
#include "Keypad.h"
#include "Key.h"
#include "Password.h"
#include <EEPROM.h>
#include <jled.h>

//KEYPAD AND COMMAND
#define MAX_NUM_CHARS 8
char customKey;
byte data_count = 0;
char cmd[MAX_NUM_CHARS];
boolean cmd_complete = false;

// non blocking
char alternate;

//EEPROM
int addr = 0;
byte value;

//INDICATORS
int greenLED = 51;
int redLED = 53;


enum led_list {
  pin_22,
  pin_23,
  pin_24,
  pin_2,
  pin_3,
  pin_4,
  pin_5,
  pin_6,
  pin_7,
  pin_8,
  pin_9,
  pin_10,
  pin_11,
  pin_12,
  pin_13,
  pin_14,
  pin_count
};

JLed leds[pin_count] = {
  JLed(0).Off().Forever(), //correct pins
  JLed(1).Off().Forever(),
  JLed(2).Off().Forever(),
  JLed(3).Off().Forever(),
  JLed(4).Off().Forever(),
  JLed(5).Off().Forever(),
  JLed(6).Off().Forever(),
  JLed(7).Off().Forever(),
  JLed(8).Off().Forever(),
  JLed(9).Off().Forever(),
  JLed(10).Off().Forever(),
  JLed(11).Off().Forever(),
  JLed(12).Off().Forever(),
  JLed(13).Off().Forever(),
  JLed(14).Off().Forever(),
  JLed(15).Off().Forever(),
};

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

JLedSequence sequence(JLedSequence::eMode::SEQUENCE, leds);

/*
 * Prints a usage menu.
 */
const char usageText[] PROGMEM = R"=====(
Usage:
input: int lamp int speed #  //always end with hashtag; E.G. 1223# lamp 12 speed of 23
input range: int firstlamp int lastlamp int speedlamp // EG: 001200# lamp 00 till 12 speed of 0


SYNC AND NOT: Concept
Input is lamp,speed,
speed <30 is 0-30s
use last number to sync or not SYNC


for example: 0112105#
lamp 00 till 12 at speed 10s delay, sync mode 5;
so you get 10 options of sync modes.
Have a nice day.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
)=====";

void printUsage() {
  Serial.println((const __FlashStringHelper *)usageText);
}

void save(void) { //updates addr with values 0-255
    EEPROM.update(addr, value);
    addr = addr + 1;
    if (addr == 200) {
      addr = 0;
    }
}

void testwrite(void) { //for testing of writing to EEPROM , Update is better for long life EEPROM
  for (int i = 0; i < 255; i++) {
    EEPROM.update(i, i);
  }
}


void read(void) { //reads everything from EEPROM
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
  return;
}

void red(int wait) {
  digitalWrite(redLED, HIGH);
  delay(wait);
  digitalWrite(redLED, LOW);
  delay(wait);
}

void green(int wait) {
  digitalWrite(greenLED, HIGH);
  delay(wait);
  digitalWrite(greenLED, LOW);
  delay(wait);
}




void process_command(void) { //handler for input
  String readString2 = cmd;
  readString2.replace("#","");
  String readString = readString2;

  if (cmd[MAX_NUM_CHARS] < 8){
    // Long speed is in secs
    String lamp = readString.substring(0, 2);
    String speed = readString.substring(2, 4);
    String mode = readString.substring(4, 5);
    Serial.println("lamp:" + lamp);
    Serial.println("speed:" + speed);
    Serial.println("mode:" + mode);
    // int mode1  = atoi(speed.c_str());
      if (mode == "1") {
      //   int lamp1  = atoi(lamp.c_str());
      //   int speed1  = atoi(speed.c_str());
      //   speed1 = speed1 * 1000;
      //   JLed leds1[] = {
      //       JLed(lamp1).Blink(600, (speed1)).Forever(),
      //   };
      //   JLedSequence sequence1(JLedSequence::eMode::PARALLEL, leds1);
      // led1 = JLed(lamp1).Blink(speed1, 60).DelayAfter(speed1).Forever();
      } else if (mode == NULL) {
          int lamp0  = atoi(lamp.c_str());
          int speed0  = atoi(speed.c_str());
          // JLed(leds[lamp0]).Blink(600,speed0).Forever();
          leds[0].Blink(600,600).Forever();
      } else if (cmd[MAX_NUM_CHARS] == 7) {
        //Alternate speed is in ms
        String lampfirst = readString.substring(0, 2);
        String lampsecond = readString.substring(2, 4);
        String speed = readString.substring(4, 6);
        String mode = readString.substring(6, 7);
        Serial.println("lampfirst=" + lampfirst);
        Serial.println("lampsecond=" + lampsecond);
        Serial.println("speed=" + speed);
        Serial.println("mode=" + mode);
        // int lampfirst2  = atoi(lampfirst.c_str());
        // int lampsecond2  = atoi(lampsecond.c_str());
            Serial.print("lamps: ");
            Serial.println(lampfirst + " " + lampsecond);
            Serial.print("speed: ");
            Serial.println(speed);
            Serial.println("mode: ");
            Serial.println(mode);
        } else {
          red(50);
          red(50);
          red(50);
          red(50);
      }
    }
}



//----------SETUP-&-LOOP-----------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  printUsage();
  // read(); // should load previous config from eeprom
}

void loop() {
  customKey = customKeypad.getKey();

  if (customKey){
    Serial.print(customKey); //print input
  }

  if(data_count == MAX_NUM_CHARS || customKey == '#') {
    // Serial.println(cmd);
    process_command();
    clearData();
  }

  if (customKey){
    cmd[data_count] = customKey;
    data_count++;
    }

sequence.Update();

}
