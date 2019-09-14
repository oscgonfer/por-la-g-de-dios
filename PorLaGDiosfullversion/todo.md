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
Mode 0: normal
Mode 1: flash long sometimes
Mode 2: alternating


in EEPROM:
lamp(addr)  speed(value)
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
13          10
22          10
23          10
24          10
24          10

lamp(addr)  mode(value)
102          1
103          1
104          1
105          1
106          1
107          1
108          0
109          0
110          0
111          0
112          2
113          2
122          1
123          1
124          1
124          1
