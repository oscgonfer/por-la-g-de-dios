#include <Arduino.h>
#include <Keypad.h>
#include <EEPROM.h>
#define MAX_NUM_CHARS 8
char customKey;
char cmd[MAX_NUM_CHARS];
boolean cmd_complete = false;
int addr = 0;
byte value;
int greenPin = 51;
int redPin = 53;

long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)

#define Password_Length 5
char Data[Password_Length];
char Master[Password_Length] = "1235";
byte data_count = 0, master_count = 0;
bool Pass_is_good;

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {

  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};


byte rowPins[ROWS] = {52, 50, 48, 46};
byte colPins[COLS] = {44, 42, 40};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


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
in EEPROM:
BUT EEPROM values go up to 255 :-1:
that means that mode work till the speed around 24
bc if value is 249 is okay
but if value is 259 is not okay :(

lamp  speed  mode
00    10     5
01    10     5
02 10 5
03 10 5
04 10 5
05 10 5
06 10 5
07 10 5
08 10 5
09 10 5
10 10 5
11 10 5
12 10 5
Have a nice day.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
)=====";

void printUsage() {
  Serial.println((const __FlashStringHelper *)usageText);
}

void save(void){ //updates addr with values 0-255
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


void clearData(){ //resets string of keypad input
  while(data_count !=0){
    cmd[data_count--] = 0;
    Data[data_count--] = 0;
  }
  return;
}

void process_command() { //handler for input
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
      // cout << "addressed lamp: " << lamp << endl;
      Serial.print("lamp: ");
      Serial.println(lamp);
      Serial.print("speed: ");
      Serial.println(speed);
    }
  }
}

void setup(){
  Serial.begin(9600);
  printUsage();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  //printlastsaved();
  //CHECK String readString = "##";
  read(); // should load previous config
  Serial.print("Enter Password:\n");
}

void green(int wait) {
  digitalWrite(greenPin, HIGH);
  delay(wait);
  digitalWrite(greenPin, LOW);
  delay(wait);

}

void red(int wait){
  digitalWrite(redPin, HIGH);
  delay(wait);
  digitalWrite(redPin, LOW);
  delay(wait);

}
//----------------------------------------------------------------------------------
void passwordCheck(void){
    customKey = customKeypad.getKey();
    if (customKey){
      Data[data_count] = customKey;
      data_count++;
    }

    if(data_count == Password_Length-1){
      if(!strcmp(Data, Master)){
        green(50); // flash green LED 3 times to confirm password
        green(50);
        green(50);
        Serial.print("Correct");
        //------------------------------------------------------------------------------Correct password
        Pass_is_good = true;
        clearData();
        }

      else {
        red(50); // flash red LED 3 times to deny password
        red(50); // also indicator that password is needed
        red(50);
        Serial.print("incorrect");
        //------------------------------------------------------------------------------Incorrect passwords
        Pass_is_good = false;
        clearData();
        }
    }
}
//----------------------------------------------------------------------------------

void loop() {
  passwordCheck();
  customKey = customKeypad.getKey();
  if (Pass_is_good = true) {
    if (customKey) {
      cmd[data_count] = customKey;
      data_count++;
    }
    if (customKey) {
      Serial.print(customKey); //print input
    }
    if(data_count == MAX_NUM_CHARS || customKey == '#') {
      // Serial.println(cmd);
      process_command();
      clearData();
    }
  }
}
