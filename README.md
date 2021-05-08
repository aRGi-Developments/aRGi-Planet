# Planet

An esp32/esp8266 espnow slave

prerequisites:
  vs-code
  platformio
  esp32 arduino
  esp8266 arduino

clone this repository, use platformio.ini.sample as a base for you platformio.ini

adjust platformio.ini
uncomment the environment you want to compile for
[platformio]
default_envs = esp8266
;default_envs = esp32

[env]
monitor_speed = 115200
lib_ldf_mode = chain+
monitor_filters = 
	direct
monitor_flags = --raw

change the com port
[env:esp32]
platform = espressif32
framework = arduino
board = pico32
monitor_port = COM19
upload_port = COM19
build_flags = -D ESP32

change the com ports
[env:esp8266]
platform = espressif8266
framework = arduino
board = esp12e
monitor_port = COM14
upload_port = COM14
build_flags = -D ESP8266

compile and upload

Created by aRGi (info@argi.mooo.com).
