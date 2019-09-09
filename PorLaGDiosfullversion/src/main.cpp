#include <Arduino.h>
#include "Keypad.h"
#include "Password.h"
#include <EEPROM.h>
#define MAX_NUM_CHARS 8
char command;
char cmd[MAX_NUM_CHARS];
boolean cmd_complete = false;
int addr = 0;
byte value;
int greenLED = 51;
int redLED = 53;

long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)

Password password = Password( "4155" );
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

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


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
    if (addr == 16) {
      addr = 0;
    }
}

void testwrite(void) { //for testing of writing to EEPROM
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


void process_command(void) { //handler for input
  String readString2 = cmd;
  readString2.replace("#","");
  String readString = readString2;
  if (cmd[MAX_NUM_CHARS] < 6){
    String lamp = readString.substring(0, 2);
    String speed = readString.substring(2, 4);
    // String mode = readString.substring(4, 6);
    Serial.println("lamp:" + lamp);
    Serial.println("speed:" + speed);
    // Serial.println("mode:" + mode);
    // Set pin no.## to ###
    // Set addr ## to ###
  } else {
    String lampbegin = readString.substring(0, 2);
    String lampend = readString.substring(2, 4);
    String speed = readString.substring(4, 6);
    //String mode = readString.substring(6, 8);
    Serial.println("lampbegin=" + lampbegin);
    Serial.println("lampend=" + lampend);
    Serial.println("speed=" + speed);
    // Serial.println("mode=" + mode);
    int lamp0  = atoi(lampbegin.c_str());
    int lamp1  = atoi(lampend.c_str());
    for (int lamp = lamp0 ; lamp < lamp1; lamp++){
      Serial.print("lamp: ");
      Serial.println(lamp);
      Serial.print("speed: ");
      Serial.println(speed);
    }
  }
}



void displayCodeEntryScreen()
{
  password.reset();
  Serial.println("Enter Code:");
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
  //setup and turn off both LEDs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
}


void green(int wait) {
  digitalWrite(greenLED, HIGH);
  delay(wait);
  digitalWrite(greenLED, LOW);
  delay(wait);
}

void unlockdoor(){ //controls servo that locks the door
  // myservo.write(90);
  digitalWrite(greenLED, HIGH);
  delay(5000);
}

void red(int wait) {
  digitalWrite(redLED, HIGH);
  delay(wait);
  digitalWrite(redLED, LOW);
  delay(wait);
}

void checkPassword(){
  if (password.evaluate()){
    digitalWrite(greenLED, HIGH);
    delay(30);
    // lcd.print("Acces Granted");
    // lcd.print("Welcome");
    unlockdoor();
    delay(2500);
    displayCodeEntryScreen();
  }
  else{
    loop(); {
      red(50);
      red(50);
      red(50);
    }
    delay(10);
    // lcd.print("Acces Denied");
    delay(2500);
    displayCodeEntryScreen();
  }
}

//take care of some special events
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
  case PRESSED:
    // lcd.print(eKey);
    switch (eKey){
    case '#':
      checkPassword();
      break;
    case '*':
         displayCodeEntryScreen(); break;
    default:
      password.append(eKey);
    }
 switch (keypad.getState()){
   case PRESSED:
   switch (eKey){
   case 'D': displayCodeEntryScreen();
    }
  }
  }
}




//----------SETUP-&-LOOP-----------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  keypad.setDebounceTime(50);
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);

  printUsage();
  read(); // should load previous config
}

void loop() {
  keypad.getKey();
  command = keypad.getKey();

  // if(data_count == MAX_NUM_CHARS || command == '#') {
  //   // Serial.println(cmd);
  //   process_command();
  //   clearData();
  // }
}
