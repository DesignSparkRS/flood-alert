#include "FloodAPI.h"

WiFiSSLClient client;

// Flood warning data
//static floodWarning warning;
FloodAPI::FloodAPI() {
}

void FloodAPI::init() {
  state = INIT;
}

int FloodAPI::updateState() {
  // static warning_levels previous_state = INIT;
  static warning_levels previous_state = NONE;
  state = warning.severityLevel;
  if (state != previous_state) {
    previous_state = state;
    switch (state) {
      // case NONE:
      case INIT:
        // led_colour(GREEN);
        break;
      case SEVERE_FLOOD_WARNING:
        // led_colour(RED);
        buzzer_on();
        break;
      case FLOOD_WARNING:
        led_colour(RED);
        buzzer_on();
        break;
      case FLOOD_ALERT:
        led_colour(AMBER);
        buzzer_on();
        break;
      case NO_LONGER:
        led_colour(GREEN);
        break;
      default:
        break;
    }
  }
  return state;
}

// Advance through states and wrap around
void FloodAPI::demo(modes m) {
  // Inject mock timestamp
  memcpy(warning.time_raised, "2025-01-01 00:01:00", DATESTR_LEN - 1);
  static warning_levels state = NONE;
  warning.severityLevel = state;
  Serial.println(warning.severityLevel);
  updateState();
  switch (state) {
    case NONE:
      state = FLOOD_ALERT;
      break;
    case SEVERE_FLOOD_WARNING:
      state = NO_LONGER;
      break;
    case FLOOD_WARNING:
      state = SEVERE_FLOOD_WARNING;
      break;
    case FLOOD_ALERT:
      state = FLOOD_WARNING;
      break;
    case NO_LONGER:
      state = NONE;
      break;
  }
}

int FloodAPI::sendRequest() {
  if (client.connect("environment.data.gov.uk", 443)) {
    // Send HTTP request
    client.println("GET /flood-monitoring/id/floodAreas/" AREA_CODE " HTTP/1.1");
    client.println("Host: environment.data.gov.uk");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Failed to connect to server");
    strcpy(error_status, "API request error");
    return 0;
  }
  return 1;
}

int FloodAPI::getResponse() {
  char http_response[32] = { 0 };
  if (client.available()) {
    // Check status code
    client.readBytesUntil('\r', http_response, sizeof(http_response));
    // should be "HTTP/1.0 200 OK"
    if (memcmp(http_response + 9, "200 OK", 6) != 0) {
      Serial.print("Unexpected HTTP status");
      if (error_status[0] == '\0') {
        strcpy(error_status, "Unexpected HTTP status");
      } else {
        strcpy(error_status, http_response);
        Serial.println(error_status);
      }
      client.stop();
      return 0;
    }
    Serial.println(http_response);

    //   return 1;
    // }
    // Skip response headers
    client.find("\r\n\r\n");

    // Stream& input;
    StaticJsonDocument<128> filter;

    // Filter data objects so the response fits into memory
    JsonObject filter_items = filter.createNestedObject("items");
    filter_items["currentWarning"]["severityLevel"] = true;
    filter_items["currentWarning"]["floodAreaID"] = true;
    filter_items["currentWarning"]["timeRaised"] = true;

    StaticJsonDocument<1024> doc;

    DeserializationError error = deserializeJson(doc, client, DeserializationOption::Filter(filter));

    if (error) {
      error_status[0] = '\0';
      strcpy(error_status, "deserializeJson failed");
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      client.stop();
      return 0;
    }

    // Update warning struct
    // warning.severityLevel = SEVERE_FLOOD_WARNING;
    warning.severityLevel = doc["items"]["currentWarning"]["severityLevel"];                                               // 3
    if (warning.severityLevel) {                                                                                           // only update these items if the level is not zero
      memcpy(warning.flood_area_id, doc["items"]["currentWarning"]["floodAreaID"].as<const char*>(), FLOOD_AREA_LEN - 1);  // "Tributaries between Dorchester and ...

      memcpy(warning.time_raised, doc["items"]["currentWarning"]["timeRaised"].as<const char*>(), DATESTR_LEN - 1);  // "2022-12-19T15:20:31"
      for (int i = 0; i < DATESTR_LEN; i++) {
        if (warning.time_raised[i] == 'T') {
          warning.time_raised[i] = ' ';
        }
      }
    }
    Serial.println("Disconnecting from server...");
    client.stop();

    Serial.println("Flood data received!");
    return 1;  // Success
  } else {
    return -1;  // Skip
  }
}