/*
  Arduino Nano 33 IoT client for Gov.co.uk Flood Warning Service

These APIs are provided as open data under the Open Government Licence with no requirement for registration.
If you make use of this data please acknowledge this with the following attribution statement:

"this uses Environment Agency flood and river level data from the real-time data API (Beta)"

  Install the following libraries using the Arduino Libary Manager:
  Arduino WiFiNINA https://github.com/arduino-libraries/WiFiNINA
  Benoît Blanchon ArduinoJson https://arduinojson.org/
  Evert-arias EasyButton https://github.com/evert-arias/EasyButton
  Waveshare EDP2in9 https://github.com/waveshareteam/e-Paper/tree/master/Arduino/epd2in9_V2

  Author: Peter Milne
  Date: 26 October 2024

  MIT License

  Copyright (c) 2024 DesignSparkRS

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#include <SPI.h>
#include <WiFiNINA.h>
#include "config.h"

#include <EasyButton.h>
#include "FloodAPI.h"
#include "FloodAlertDisplay.h"
#include "led.h"
#include "buzzer.h"
#include "version.h"

#define WIFI_TIMEOUT_TRIES 3

// Button connections
#define B1_PIN 21
#define B2_PIN 20
#define B3_PIN 19
#define B4_PIN 18
#define B5_PIN 17
#define B6_PIN 16

modes mode;

const char* soft_version = VERSION;

FloodAPI myFloodAPI = FloodAPI();

FloodAlertDisplay epd = FloodAlertDisplay(&myFloodAPI);

// int status = WL_IDLE_STATUS;

EasyButton button1(B1_PIN);
EasyButton button2(B2_PIN);
EasyButton button3(B3_PIN);
EasyButton button4(B4_PIN);
EasyButton button5(B5_PIN);
EasyButton button6(B6_PIN);

int wifi_status = WL_IDLE_STATUS;

static unsigned long now = 0;
static unsigned long lastApiAttempt = 0;

void setup() {
  led_init();
  buzzer_init();

  // Initialize Serial Port
  Serial.begin(115200);
  // while (!Serial) {
  //   ;  // wait for serial port to connect. Needed for native USB port only
  // }
  delay(2000);

  Serial.print("Starting client version: ");
  Serial.println(soft_version);

  // Initialize buttons
  button1.begin();
  button1.onPressed(dry);
  button2.begin();
  button2.onPressed(rain);  // Place holder
  button3.begin();
  button3.onPressed(flood);  // Place holder
  button4.begin();
  button4.onPressed(replay);
  button4.onPressedFor(2000, buzzer_off);  // Cancel buzzer
  button5.begin();
  button6.begin();
  button6.onPressed(clock_sync_ap_mode);  // Place holder

  // Setup display and show greeting
  epd.initDisplay();
  epd.showGreeting();
  delay(3000);

  // Hold down B5 while pressing reset to enter demo mode
  // Press reset to exit back to standard mode
  if (button5.isPressed()) {
    mode = DEMO_MODE;
    Serial.println("Entering demo mode...");
    doDemo();
  } else {
    mode = STD_MODE;
    Serial.println("Entering standard mode...");
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (1);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  // WiFi.setTimeout(60 * 1000);
  while (wifi_status != WL_CONNECTED) {
    static int timeout_counter = 0;
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    wifi_status = WiFi.begin(SECRET_SSID, SECRET_PASS);
    timeout_counter ++;

    // wait 10 seconds for connection:
    delay(10000);
    if ((timeout_counter >= WIFI_TIMEOUT_TRIES) && (WiFi.status() != WL_CONNECTED)) {
      epd.connectionError();
      buzzer_on();
      while (1);
    }
  }
  rgb_colour(GREEN);
  epd.wifiOn = true;
  Serial.println("Connected to WiFi");
  printWiFiStatus();
  myFloodAPI.init();
  now = millis();
  lastApiAttempt = now;
  doUpdate();
}

void loop() {
  // Continuously update the button states
  button1.read();
  button2.read();
  button3.read();
  button4.read();
  // button5.read(); Read only in setup
  button6.read();

  if (WiFi.status() != WL_CONNECTED) {  // Connect wifi
    delay(5000);
    rgb_colour(RED);
    epd.wifiOn = false;
    buzzer_on();
    reconnectWiFi();
    if (WiFi.status() == WL_CONNECTED) {
      rgb_colour(GREEN);
      epd.wifiOn = true;
      buzzer_off();
      Serial.println("Wifi connected...");
      doUpdate();  // Initial update
    } else {
      epd.connectionError();
    }
  }
  
  now = millis();
  if (WiFi.status() == WL_CONNECTED) {
    if ((now - lastApiAttempt > ALERT_INTERVAL) || (mode == REPLAY_MODE)) {
      mode = STD_MODE;  // Clear replay
      doUpdate();
      lastApiAttempt = now;
    }
  }
}

void doUpdate() {
  int result = myFloodAPI.getData();
  if (result) {
    myFloodAPI.updateState(myFloodAPI.warning.severityLevel);
    printData();
    epd.updateDisplay();
  } else {
    epd.apiError();
  }
}

void doDemo() {
  epd.demoOn = true;
  while (1) {
    button1.read();
    button2.read();
    button3.read();
    button4.read();
    // button5.read(); Read only in setup
    button6.read();
    unsigned long now = millis();
    static unsigned long lastUpdate = 0;
    if (now - lastUpdate > DEMO_INTERVAL) {
      buzzer_off();
      myFloodAPI.demo(DEMO_MODE);
      epd.updateDisplay();
      lastUpdate = millis();
    }
  }
}

int reconnectWiFi() {
  // WL_IDLE_STATUS     = 0
  // WL_NO_SSID_AVAIL   = 1
  // WL_SCAN_COMPLETED  = 2
  // WL_CONNECTED       = 3
  // WL_CONNECT_FAILED  = 4
  // WL_CONNECTION_LOST = 5
  // WL_DISCONNECTED    = 6

  WiFi.disconnect();  // Force a disconnect
  delay(1000);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("Wifi status: ");
  Serial.println(WiFi.status());
  return WiFi.status();
}

// Button callbacks
void dry() {
  Serial.println("B1 button pressed...");
  bip();
}

void rain() {
  Serial.println("B2 button pressed...");
  bip();
}

void flood() {
  Serial.println("B3 button pressed...");
  bip();
}

void replay() {
  Serial.println("B4 button pressed...");
  mode = REPLAY_MODE;
  bip();
}

void buzzerOff() {
  Serial.println("B4 button held...");
  buzzer_off();
}

void clock_sync_ap_mode() {
  Serial.println("B6 button pressed...");
  bip();
}

// Debug output
void printData() {
  Serial.print("Flood Area: https://check-for-flooding.service.gov.uk/target-area/");
  Serial.println(myFloodAPI.warning.flood_area_id);

  Serial.print("Warning Level: ");
  Serial.println(myFloodAPI.warning.severityLevel);

  Serial.print("Time Raised: ");
  Serial.println(myFloodAPI.warning.time_raised);
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}