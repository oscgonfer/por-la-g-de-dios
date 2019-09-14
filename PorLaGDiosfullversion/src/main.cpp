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
bool fHasLooped  = true;

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
  pin_14
};

JLed leds[25] = {
  JLed(0).Off(),
  JLed(0).Off(),
  JLed(2).Off(),
  JLed(3).Off(),
  JLed(4).Off(),
  JLed(5).Off(),
  JLed(6).Off(),
  JLed(7).Off(),
  JLed(8).Off(),
  JLed(9).Off(),
  JLed(10).Off(),
  JLed(11).Off(),
  JLed(12).Off(),
  JLed(13).Off(),
  JLed(14).Off(),
  JLed(0).Off(),
  JLed(0).Off(),
  JLed(0).Off(),
  JLed(0).Off(),
  JLed(0).Off(),
  JLed(0).Off(),
  JLed(0).Off(),
  JLed(22).Off(),
  JLed(23).Off(),
  JLed(24).Off()
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

JLedSequence sequence(JLedSequence::eMode::PARALLEL, leds);

/*
 * Prints a usage menu.
 */
const char usageText[] PROGMEM = R"=====(
Usage:
input: int lamp int speed #  //always end with hashtag; E.G. 1223# lamp 12 speed of 23
input long: int lamp int speedlamp // EG: 12101# lamp 12 speed of 10 secs
input alternate: int firstlamp int secondlamp int speedlamp // EG: 0012502# lamp 00 till 12 speed of 0
Have a nice day.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
)=====";

void printUsage() {
  Serial.println((const __FlashStringHelper *)usageText);
}

// void save(void) { //updates addr with values 0-255
//     EEPROM.update(addr, value);
//     addr = addr + 1;
//     if (addr == 125) {
//       addr = 0;
//     }
// }

// void testwrite(void) { //for testing of writing to EEPROM , Update is better for long life EEPROM
//   for (int i = 0; i < 255; i++) {
//     EEPROM.update(i, i);
//   }
// }


void read(void) { //reads/loads everything from EEPROM
      value = EEPROM.read(addr);
      // Serial.print(addr);
      // Serial.print("\t");
      // Serial.print(value, DEC);
      // Serial.println();

      // addr = addr + 1;
      for (int i = addr; i < 125; i++) {
        int speed = value;

        if (i < 100) {
          leds[i].Blink(speed, speed).Forever();
          Serial.print("lamp ");
          Serial.print(i);
          Serial.print("Speed ");
          Serial.print(speed);
          Serial.println();
        }

          if (i > 100) {
            i = i - 100; // 3rd colomn for eeprom
            int mode = value;
            if (mode == 0) {
              leds[i].Blink(speed, speed).Forever();
              Serial.print("mode ");
              Serial.print(mode);
              Serial.print("lamp ");
              Serial.print(i);
              Serial.print("speed ");
              Serial.print(speed);
              Serial.println();

            } else if (mode == 1) {
              int speedeee =  speed * 1000;
              leds[i].Blink(60, speedeee).Forever();
              Serial.print(mode);
              Serial.print(i);
              Serial.print(speed);
              Serial.println();

            } else if (value == 2 | value == 3) {

              if (mode == 2) {
                leds[i].Blink(speed, speed).Forever().DelayBefore(speed);
                Serial.print(mode);
                Serial.print(i);
                Serial.print(speed);
                Serial.println();
            } else {
              leds[i].Blink(speed, speed).Forever();
              Serial.print(mode);
              Serial.print(i);
              Serial.print(speed);
              Serial.println();
            }
          }
        }
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

    // Long speed is in secs
    if (cmd[MAX_NUM_CHARS] < 8){
    String lamp = readString.substring(0, 2);
    String speed = readString.substring(2, 4);
    String mode = readString.substring(4, 5);
    Serial.println("lamp:" + lamp);
    Serial.println("speed:" + speed);
    Serial.println("mode:" + mode);
    int mode1  = atoi(mode.c_str());
      if (mode1 == 1) {
        int lamp1  = atoi(lamp.c_str());
        int speed1  = atoi(speed.c_str());
        speed1 = speed1 * 1000;
        leds[lamp1].Blink(60, speed1).Forever();
        int speedee = speed1 / 1000;
        EEPROM.update(lamp1, speedee);
        EEPROM.update(lamp1 + 100, mode1);
    } else if (mode == "") {
          int lamp0  = atoi(lamp.c_str());
          int speed0  = atoi(speed.c_str());
          leds[lamp0].Blink(speed0, speed0).Forever();
          EEPROM.update(lamp0, speed0);
          EEPROM.update(lamp0 + 100, 0);
          // int testread = EEPROM.read(lamp0);
          // Serial.println(testread);
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
        int lampfirst2  = atoi(lampfirst.c_str());
        int lampsecond2  = atoi(lampsecond.c_str());
        int speed2  = atoi(speed.c_str());
        // int mode2  = atoi(mode.c_str());
            Serial.print("lamps: ");
            Serial.println(lampfirst + " " + lampsecond);
            Serial.print("speed: ");
            Serial.println(speed);
            Serial.println("mode: ");
            Serial.println(mode);
            if (mode == "2"){
            leds[lampfirst2].Blink(speed2, speed2).Forever().DelayBefore(speed2);
            leds[lampsecond2].Blink(speed2, speed2).Forever();
            EEPROM.update(lampfirst2, speed2); //store lamp + speed
            EEPROM.update(lampsecond2, speed2);
            EEPROM.update(lampfirst2 + 100, 2);
            EEPROM.update(lampsecond2 + 100, 3);
            }
          }

  } else {
    Serial.println("WRONG!");
    red(50);
    red(50);
    red(50);
    red(50);
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
}

void loop() {
  if (fHasLooped == false) {
    for (int x = 0; x != 125; x++) {
      read(); // should load previous config from eeprom
    delay(500);
    }
    fHasLooped = true;
  }



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
