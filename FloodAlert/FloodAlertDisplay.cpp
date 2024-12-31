#include "FloodAlertDisplay.h"

// Flood warning text
static char w1[5][12] = { "No Flood", "DANGER TO", "Flooding is", "Flooding is", "Warning" };
static char w2[5][12] = { "Warnings", "LIFE", "Expected", "Possible", "Removed" };
static char w3[5][12] = { "", "", "", "", "" };

void FloodAlertDisplay::initDisplay(void) {
  if (_epd.Init() != 0) {
    return;
  }
  Serial.println("EDP attached");

  _epd.ClearFrameMemory(0xFF);  // bit set = white, bit reset = black
  _epd.DisplayFrame();

  delay(2000);

  _epd.SetFrameMemory_Base(RSLOGO);
  _epd.DisplayFrame();
}

void FloodAlertDisplay::showGreeting(void) {
  _paint.SetWidth(120);
  _paint.SetHeight(32);
  _paint.SetRotate(ROTATE_180);

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "  Flood  ", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 140, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "  Alert  ", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 120, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "Concept:", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 80, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "Jude Pullen", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 60, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, "Code:", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 20, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, "Pete Milne", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 0, _paint.GetWidth(), _paint.GetHeight());

  _epd.DisplayFrame_Partial();
}

void FloodAlertDisplay::connectionError(void) {
  _paint.SetWidth(120);
  _paint.SetHeight(32);
  _paint.SetRotate(ROTATE_180);

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "Connection", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 140, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "Error", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 120, _paint.GetWidth(), _paint.GetHeight());

  _epd.DisplayFrame_Partial();
}

void FloodAlertDisplay::apiError(void) {
  _paint.SetWidth(120);
  _paint.SetHeight(32);
  _paint.SetRotate(ROTATE_180);

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "API", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 140, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "Error", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 120, _paint.GetWidth(), _paint.GetHeight());

  _epd.DisplayFrame_Partial();
}

void FloodAlertDisplay::updateDisplay() {
  Serial.println("Updating display...");
  int severityLevel = _magnet->warning.severityLevel;
  // Index warning string based on severity level
  //int warning_idx = severityLevel ? severityLevel : 0;
  //  char single_digit[] = {'0', '\0'};
  //  char double_digit[] = {'0', '0', '\0'};
  //  char three_digit[] = {'0', '/', '0', '\0'};
  //  char four_digit[] = {'0', '/', '0', '0',  '\0'};
  //
  // Set background
  if (_epd.Init() != 0) {
    return;
  }
  _epd.ClearFrameMemory(0xFF);  // bit set = white, bit reset = black
  _epd.DisplayFrame();

  delay(500);
  switch (severityLevel) {
    case NONE:
      _epd.SetFrameMemory_Base(epd_flood_warning_removed);
      break;
    case SEVERE_FLOOD_WARNING:
      _epd.SetFrameMemory_Base(epd_flood_warning_severe);
      break;
    case FLOOD_WARNING:
      _epd.SetFrameMemory_Base(epd_flood_warning);
      break;
    case FLOOD_ALERT:
      _epd.SetFrameMemory_Base(epd_flood_alert);
      break;
    case NO_LONGER:
      _epd.SetFrameMemory_Base(epd_flood_warning_removed);
      break;
    default:
      break;
  }

  _epd.DisplayFrame();

  // Static text
  _paint.SetWidth(120);
  _paint.SetHeight(40);
  _paint.SetRotate(ROTATE_180);

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, w1[severityLevel], &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 120, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, w2[severityLevel], &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 100, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, w3[severityLevel], &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 80, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, AREA_LINE_1, &Font12, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 70, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, AREA_LINE_2, &Font12, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 60, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, AREA_LINE_3, &Font12, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 50, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, "Updated", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 30, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, (_magnet->warning.time_raised ? _magnet->warning.time_raised : ""), &Font12, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 15, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  if (demoOn) {
    _paint.DrawStringAt(0, 0, "", &Font16, COLORED);
  } else if (wifiOn) {
    _paint.DrawStringAt(0, 0, "Wifi", &Font16, COLORED);
  } else {
    _paint.DrawStringAt(0, 0, "", &Font16, COLORED);
  }
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 0, _paint.GetWidth(), _paint.GetHeight());

  _epd.DisplayFrame_Partial();
}
