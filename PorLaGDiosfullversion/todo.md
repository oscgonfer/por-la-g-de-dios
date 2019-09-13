# to Do
- sync modes
  - create alternating pulses using non blocking loop
- ssr

- avoid lamp > 16


- sudo chmod 666 /dev/ttyACM0
- platformio run -t upload
- platformio device monitor --port /dev/ttyACM0


- [password auth](https://www.instructables.com/id/Arduino-password-lock/)
- [non blocking examples](https://learn.adafruit.com/multi-tasking-the-arduino-part-1/using-millis-for-timing)

- [Sorting numbers? which mode where](https://www.arduino.cc/reference/en/language/structure/control-structure/goto/)

# Sorting for alternate mode etc


void changePinState(int pin)
// if pin reads LOW, set it HIGH
// if pin reads HIGH, set it LOW
{
  int state=digitalRead(pin);
  if (state == HIGH) goto skip_1;
  if (state == LOW) goto skip_2;
  goto error;
  skip_1:
  state = LOW;
  goto done;
  skip_2:
  state = HIGH;
  goto done;
  error:
  Serial.println("Internal Function Error");
  done:
  digitalWrite(pin, state);
}  

# Storing Concept
Mode 1: normal
Mode 2: flash alternatively
Mode 3: sync with all modes 3 lamps


in EEPROM:
lamp(addr)  speed(value)
00          10
01          10
02          10
03          10
04          10
05          10
06          10
07          10
08          10
09          10
10          10
11          10
12          10

lamp(addr)  mode(value)
101         2
102         2
103         2
104
105
106
107
108
109
110
