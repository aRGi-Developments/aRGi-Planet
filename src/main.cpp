/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* CHANGELOG
* 2021 05 07 
*    initial release
*/

/**
 * @file main.cpp
 *
 * this is the main planet file
 * 
 * Copyright (c) 2021 aRGi <info@argi.mooo.com>
 *
 */


// comment out the line below to stop logging to terminal
#define PLANET_DEBUGON

#ifdef ESP32
#endif  
#ifdef ESP8266
  #define esp_err_t int
  #define ESP_OK 0
#endif  

#include <arduino.h>
#include "planet.h"
#include "debug.h"

#define PLANET_MAX_SATELLITES  20


planet_t    planet; // planet configuration data
satellite_t satellites[PLANET_MAX_SATELLITES]; // satellites configuration data

/**
 * startup and loop functions
 * declare external because we want to compile this program for both esp32 and esp8266
 * use platformio.ini to change the target architecture
*/ 
extern esp_err_t planetStartup(void);
extern esp_err_t planetLoop(void);

/**
 * standard arduino style setup function
 *
 */
void setup() {
  esp_err_t retcode = 1;

  DEBUG_BEGIN
  DEBUG_STRLN("");
  DEBUG_STR("Planet starting Up ");
  while (retcode != ESP_OK)
    retcode = planetStartup();
}

/**
 * standard arduino style loop function
 *
 */
void loop() {

  planetLoop();

}