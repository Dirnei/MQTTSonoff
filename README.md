# MQTTSonoff
A small and simple firmware for the Sonoff Devices to add MQTT functionallity to it.

# Motivation
I started this project because the other solutions i found were to big for that what i wan't to use them. This should contain only the essentials for what these devices are for.

# Flashing
I'm flashing the boards via the Arduino IDE. If you also wan't to do that you have to insert the following URL to your Boardmanager:
http://arduino.esp8266.com/stable/package_esp8266com_index.json
After that you can download the ESP8266 (current verison is 2.3.0). After installing it, choose the Generic ESP8266 Module and apply following setting:

- Flash Mode    DIO
- Flash Freq.   40Mhz
- Flash Size    1M (64K SPIFFS)
- Reset Method: ck

## Touch and 4CH
To flash these devices you have to choose the Generic ESP8285 Module with the following setting:

- Flash Mode    DIO
- CPU Freq.     80Mhz
- Flash Size    1M (64K SPIFFS)

## Notes
This is a rewrite of my original code to look nice again, because it was a mess after the first tests etc... You may know what i'm talking about :)
I will continue to add more devices like Sonoff Touch, 4CH and 2CH Sonoff, but they not here yet. For know only the 1CH is working so far.

If someone wan't to help to imporove this little library, feel free to do so :)

## Other source
If you wan't a bit more features you may want to check the folling project:
- https://github.com/arendst/Sonoff-Tasmota
