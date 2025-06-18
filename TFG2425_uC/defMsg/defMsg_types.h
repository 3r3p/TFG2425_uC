#ifndef MSGTYPES_H
#define MSGTYPES_H

struct msg_BME280 {
  float temperatura;
  float humitat;
  float pressio;
};

struct msg_LEDs {
  int r;
  int g;
  int b;
  String special;  
};

#endif
