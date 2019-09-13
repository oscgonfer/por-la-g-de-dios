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

//SSR
unsigned long previousMillis = 0;        // will store last time LED was updated
long OnTime = 250;           // milliseconds of on-time
long OffTime = 750;          // milliseconds of off-time
int ledState1 = LOW;             // ledState used to set the LED
int ledState2 = LOW;             // ledState used to set the LED


//TEST
int testpin = 45;      // the number of the TEST LED

//NON BLOCKING
// long previousMillis = 0;        // will store last time LED was updated
// long interval = 1000;           // interval at which to blink (milliseconds)

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
auto led1 = JLed(0).Off().Forever();
auto led0 = JLed(0).Off().Forever();


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
          JLed leds0[] = {
              JLed(lamp0).Blink(600,speed0).Forever(),
          };
          JLedSequence sequence0(JLedSequence::eMode::PARALLEL, leds0);

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


// void mode0(int ledPin){ // Single
//   // if (mode == "2") {
//   //   unsigned long currentMillis = millis();
//   //   //TODO MAKE VARIABLE/DYNAMIC , all lamps with mode .. should run in here
//   //   if(currentMillis - previousMillis > interval) {
//   //     previousMillis = currentMillis;
//   //
//   //     if (ledState == LOW)
//   //       ledState = HIGH;
//   //     else
//   //       ledState = LOW;
//   //
//   //     digitalWrite(ledPin, ledState);
//   //   }
//   // }
// }
//
// void mode2(int ledPin){ // Long
//   // if (mode == "1") {
//   //   unsigned long currentMillis = millis();
//   //   //TODO MAKE VARIABLE/DYNAMIC , all lamps with mode .. should run in here
//   //   if(currentMillis - previousMillis > interval) {
//   //     previousMillis = currentMillis;
//   //
//   //     if (ledState == LOW)
//   //       ledState = HIGH;
//   //     else
//   //       ledState = LOW;
//   //
//   //     digitalWrite(ledPin, ledState);
//   //   }
//   // }
// }
//
// void mode1(int ledPin1, int ledPin2){ // Alternate
//   //   unsigned long currentMillis = millis();
//   // //TODO MAKE VARIABLE/DYNAMIC , all lamps with mode .. should run in here
//   //   if ((ledState1 == HIGH) && (currentMillis - previousMillis >= OnTime)) {
//   //     ledState1 = LOW;  // Turn it off
//   //     ledState2 = HIGH;  // Turn it off
//   //     previousMillis = currentMillis;  // Remember the time
//   //     digitalWrite(ledPin1, ledState1);  // Update the actual LED
//   //     digitalWrite(ledPin2, ledState2);  // Update the actual LED
//   //   } else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime)) {
//   //     ledState1 = HIGH;  // turn it on
//   //     ledState2 = LOW;  // turn it on
//   //     previousMillis = currentMillis;   // Remember the time
//   //     digitalWrite(ledPin1, ledState1);	  // Update the actual LED
//   //     digitalWrite(ledPin2, ledState2);	  // Update the actual LED
//   //   }
// }


//----------SETUP-&-LOOP-----------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  printUsage();
  read(); // should load previous config from eeprom
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


    sequence0.Update();
    // sequence1.Update();
    led0.Update();
    // led1.Update();
}
