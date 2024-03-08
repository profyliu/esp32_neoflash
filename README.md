# Description

Add lighting effects to key presses on your PC, via an ESP32 board and a NeoFlash board: https://docs.m5stack.com/en/unit/neoflash

# Installation and Usage

Upload the ESP32_FastLED_Wifi_keyboard_light.ino sketch to ESP32 via Arduino IDE. 

On the PC, join the WiFi access point broadcast by the ESP32 (revise the source code to change the SSID and password). Then, run the python code:

```bash
python sendKeyPresses.py
```

Subsequent key presses on the PC will light up the NeoFlash in colorful ways. 

To stop the python program, type Ctrl-C in the terminal. 
