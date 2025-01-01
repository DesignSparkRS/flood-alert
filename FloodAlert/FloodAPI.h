#ifndef _FLOOD_API_H_
#define _FLOOD_API_H_

#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "config.h"
#include "led.h"
#include "buzzer.h"

#define DATESTR_LEN 17     // "2022-12-19T15:20:31" -> "2022-12-19 15:20"
#define FLOOD_AREA_LEN 20  // Flood area description

typedef enum { INIT = -1,
                      NONE,
                      SEVERE_FLOOD_WARNING,
                      FLOOD_WARNING,
                      FLOOD_ALERT,
                      NO_LONGER } warning_levels;

typedef enum { DEMO_MODE,
             STD_MODE,
             REPLAY_MODE } modes;

struct floodWarning {
  char time_raised[DATESTR_LEN] = { '\0' };
  warning_levels severityLevel = NONE;
  char flood_area_id[FLOOD_AREA_LEN] = { '\0' };
};

class FloodAPI {
public:
  floodWarning warning;  // Flood warning data
  warning_levels state;
  char error_status[64];  // String buffer to hold error status
  FloodAPI();
public:
  void init();
  void doUpdate();
  int updateState();
  int sendRequest();
  int getResponse();
  void demo(modes m);
};

#endif
