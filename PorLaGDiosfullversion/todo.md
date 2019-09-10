# to Do
- led void
- password check order
  - password led
- sync modes
  - create alternating pulses using non blocking loop
- protocol
- ssr

- sudo chmod 666 /dev/ttyACM0
- platformio run -t upload
- platformio device monitor --port /dev/ttyACM0


- [password auth](https://www.instructables.com/id/Arduino-password-lock/)
- [non blocking examples](https://learn.adafruit.com/multi-tasking-the-arduino-part-1/using-millis-for-timing)


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
103
104
105
106
107
108
109
110
