#ifdef PLANET_DEBUGON
  #define DEBUG_BEGIN Serial.begin(115200);
  #define DEBUG_STRLN(str) Serial.println(str);
  #define DEBUG_CARLN(str) Serial.println(str);
  #define DEBUG_CHRLN(str) Serial.println(str);
  #define DEBUG_UCHLN(str,base) Serial.println(str,base);
  #define DEBUG_INTLN(str) Serial.println(str);
  #define DEBUG_UINLN(str,base) Serial.println(str,base);
  #define DEBUG_LNGLN(str) Serial.println(str);
  #define DEBUG_ULNLN(str,base) Serial.println(str,base);
  #define DEBUG_DBLLN(str) Serial.println(str,3);

  #define DEBUG_STR(str) Serial.print(str);
  #define DEBUG_CAR(str) Serial.print(str);
  #define DEBUG_CHR(str) Serial.print(str);
  #define DEBUG_UCH(str,base) Serial.print(str,base);
  #define DEBUG_INT(str) Serial.print(str);
  #define DEBUG_UIN(str,base) Serial.print(str,base);
  #define DEBUG_LNG(str) Serial.print(str);
  #define DEBUG_ULN(str,base) Serial.print(str,base);
  #define DEBUG_DBL(str) Serial.print(str,3);
#else
  #define DEBUG_BEGIN 
  #define DEBUG_STRLN(str)
  #define DEBUG_CARLN(str)
  #define DEBUG_CHRLN(str)
  #define DEBUG_UCHLN(str,base)
  #define DEBUG_INTLN(str)
  #define DEBUG_UINLN(str,base)
  #define DEBUG_LNGLN(str) 
  #define DEBUG_ULNLN(str,base)
  #define DEBUG_DBLLN(str)

  #define DEBUG_STR(str) 
  #define DEBUG_CAR(str)
  #define DEBUG_CHR(str) 
  #define DEBUG_UCH(str,base) 
  #define DEBUG_INT(str) 
  #define DEBUG_UIN(str,base) 
  #define DEBUG_LNG(str) 
  #define DEBUG_ULN(str,base)
  #define DEBUG_DBL(str) 
#endif
