//https://programmingelectronics.com/tutorial-24-multi-dimensional-arrays-aka-matrix-old-version/

#include <Arduino.h>
#include <Keypad.h>

#define MAX_NUM_CHARS 8
char customKey;
char cmd[MAX_NUM_CHARS]; 
boolean cmd_complete = false;
byte data_count = 0, master_count = 0;

const byte ROWS = 4; 
const byte COLS = 3; 

int pinMatrix[14][2] = {
  {0,  0}, // actually read from eeprom
  {1,  0},
  {2,  0},
  {3,  0},
  {4,  0},
  {6,  0},
  {7,  0},
  {8,  0},
  {9,  0},
  {10,  0},
  {11,  0},
  {12,  0},
  {13,  0},
  {14,  0}

};



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

Have a nice day.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
)=====";

void printUsage() {
  Serial.println((const __FlashStringHelper *)usageText);
}

void clearData(){
  while(data_count !=0){
    cmd[data_count--] = 0; 
  }
  return;
}

/*
 * Checks received command and calls corresponding functions.
 */
void process_command() {
  // cmd[MAX_NUM_CHARS] = 4;

  String readString2 = cmd;
  readString2.replace("#","");
  String readString = readString2;
  if (cmd[MAX_NUM_CHARS] < 6){
    String lamp = readString.substring(0, 2);
    String speed = readString.substring(2, 4);
    Serial.println("lamp:" + lamp);
    Serial.println("speed:" + speed);
  } else {
    String lampbegin = readString.substring(0, 2);
    String lampend = readString.substring(2, 4);
    String speed = readString.substring(4, 6);
    Serial.println("lampbegin=" + lampbegin);
    Serial.println("lampend=" + lampend);
    Serial.println("speed=" + speed);
   }  
  }


  // write to eeprom..


// void printlastsaved(void){
//   // read from eeprom and print in serial
// }


void setup(){
  Serial.begin(9600);
  printUsage();
  //printlastsaved();
  String readString = "##";
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

}