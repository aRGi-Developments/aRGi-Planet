#define PLANET_DEBUGON
#ifdef ESP32
#endif  
#ifdef ESP8266
  #define esp_err_t int
#endif  

#include <arduino.h>
#include "planet.h"
#include "debug.h"

#define PLANET_MAX_SATELLITES  20


planet_t    planet;
satellite_t satellites[PLANET_MAX_SATELLITES];

extern esp_err_t planetStartup(void);
extern esp_err_t planetLoop(void);

void setup() {
  DEBUG_BEGIN
  DEBUG_STRLN("");
  DEBUG_STR("Starting Up ");
  esp_err_t retcode;
  
  retcode = planetStartup();
}

void loop() {

  planetLoop();

}
  