#include <Arduino.h>
#include <BleGamepad.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

// --- Wi-Fi Credentials ---
// Replace with your Wi-Fi credentials before uploading
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// --- BLE Gamepad Object ---
BleGamepad bleGamepad("ESP32 TX Controller", "Custom", 100);

// --- PIN DEFINITIONS ---
const int pinThrottle = 34; 
const int pinYaw      = 35; 
const int pinPitch    = 32; 
const int pinRoll     = 33; 
const int pinPot1     = 39; 
const int pinPot2     = 36; 

const int pinArm      = 13;
const int pinModeSw   = 12;
const int pinSwA      = 25;
const int pinSwB      = 26;
const int pinOTA      = 15; 
const int pinBat      = 14;

// Naye Buttons
const int pinBtn1     = 1;  
const int pinBtn2     = 0;  
const int pinBtn3     = 2;  

// --- TFT Object ---
TFT_eSPI tft = TFT_eSPI(); 

bool otaModeActive = false; 
bool lastBtState = false;

// --- Battery Smoothing Variables ---
float smoothedVoltage = -1.0; 
const float alpha = 0.05; // Smoothing factor (5% nayi value, 95% purani value)

// UI Frame Draw Function
void drawStaticUI() {
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_DARKCYAN);
  tft.drawFastHLine(0, 30, tft.width(), TFT_DARKCYAN);
  tft.drawFastVLine(tft.width()/2, 30, 85, TFT_DARKCYAN);
  tft.drawFastHLine(0, 115, tft.width(), TFT_DARKCYAN); 
  tft.setTextSize(1);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setCursor(5, 35); tft.print("LEFT STICK & POT1");
  tft.setCursor(tft.width()/2 + 5, 35); tft.print("RIGHT STICK & POT2");
  tft.setCursor(5, 120); tft.print("SWITCHES & BUTTONS");
}

void setup() {
  pinMode(pinArm, INPUT_PULLUP);
  pinMode(pinModeSw, INPUT_PULLUP);
  pinMode(pinSwA, INPUT_PULLUP);
  pinMode(pinSwB, INPUT_PULLUP);
  pinMode(pinOTA, INPUT_PULLUP);
  pinMode(pinBtn1, INPUT_PULLUP);
  pinMode(pinBtn2, INPUT_PULLUP);
  pinMode(pinBtn3, INPUT_PULLUP);

  tft.init();
  tft.setRotation(1); 
  tft.invertDisplay(false); 
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, tft.height()/2 - 10);
  tft.println("SYSTEM BOOTING...");
  delay(1000);

  BleGamepadConfiguration bleConfig;
  bleConfig.setAutoReport(false); 
  bleConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  bleConfig.setAxesMin(0);
  bleConfig.setAxesMax(4095); 
  bleConfig.setButtonCount(14); 
  bleGamepad.begin(&bleConfig);
  
  drawStaticUI();
}

void startOTAMode() {
  otaModeActive = true;
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_BLUE);
  tft.setCursor(10, 20);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  tft.println("OTA MODE ACTIVE");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  tft.setCursor(10, 50);
  tft.setTextSize(1);
  tft.print("Connecting to Wi-Fi...");
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
  }

  ArduinoOTA.setHostname("ESP32-TX-Controller");
  
  ArduinoOTA.onStart([]() {
    tft.fillRect(10, 70, tft.width()-20, 40, TFT_BLACK);
    tft.setCursor(10, 70);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("UPDATING FIRMWARE...");
  });
  
  ArduinoOTA.onEnd([]() {
    tft.setCursor(10, 100);
    tft.println("UPDATE SUCCESS! REBOOTING...");
    delay(2000);
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int percent = (progress / (total / 100));
    tft.setCursor(10, 90);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("Progress: %3d%%", percent);
    tft.fillRect(10, 110, percent * 2, 10, TFT_GREEN); 
  });
  
  ArduinoOTA.begin();

  tft.fillRect(10, 50, tft.width()-20, 20, TFT_BLACK); 
  tft.setCursor(10, 50);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.print("IP: ");
  tft.println(WiFi.localIP());
  tft.setCursor(10, 70);
  tft.println("Waiting for upload...");
}

void loop() {
  if (!otaModeActive && digitalRead(pinOTA) == LOW) {
    delay(50); 
    if (digitalRead(pinOTA) == LOW) startOTAMode(); 
  }

  if (otaModeActive) {
    ArduinoOTA.handle(); 
    return; 
  }
  
  // ==========================================
  // --- TX DATA READING & MAPPING ---
  // ==========================================
  
  int thrRaw   = 4095 - analogRead(pinThrottle); 
  int pitchRaw = 4095 - analogRead(pinPitch); 
  int yawRaw   = analogRead(pinYaw);
  int rollRaw  = analogRead(pinRoll);
  int pot1Raw  = analogRead(pinPot1);
  int pot2Raw  = analogRead(pinPot2);

  bool armSt   = (digitalRead(pinArm) == LOW);
  bool modeSt  = (digitalRead(pinModeSw) == LOW);
  bool swASt   = (digitalRead(pinSwA) == LOW);
  bool swBSt   = (digitalRead(pinSwB) == LOW);
  
  bool b1St    = (digitalRead(pinBtn1) == LOW);
  bool b2St    = (digitalRead(pinBtn2) == LOW);
  bool b3St    = (digitalRead(pinBtn3) == LOW);

  if (bleGamepad.isConnected()) {
    bleGamepad.setAxes(yawRaw, thrRaw, rollRaw, pitchRaw, pot1Raw, pot2Raw);

    if(armSt) bleGamepad.press(BUTTON_1); else bleGamepad.release(BUTTON_1);
    if(modeSt) bleGamepad.press(BUTTON_2); else bleGamepad.release(BUTTON_2);
    if(swASt) bleGamepad.press(BUTTON_3); else bleGamepad.release(BUTTON_3);
    if(swBSt) bleGamepad.press(BUTTON_4); else bleGamepad.release(BUTTON_4);
    
    if(b1St) bleGamepad.press(BUTTON_5); else bleGamepad.release(BUTTON_5);
    if(b2St) bleGamepad.press(BUTTON_6); else bleGamepad.release(BUTTON_6);
    if(b3St) bleGamepad.press(BUTTON_7); else bleGamepad.release(BUTTON_7);

    bleGamepad.sendReport();
  }
  
  // ==========================================
  // --- DYNAMIC UI UPDATES ---
  // ==========================================
  
  tft.setTextSize(1); 

  // 1. Bluetooth Status
  bool currentBtState = bleGamepad.isConnected();
  if (currentBtState != lastBtState || millis() < 100) { 
    tft.setCursor(5, 10);
    if (currentBtState) {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.print("BT: LINKED     ");
    } else {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.print("BT: SEARCHING..");
    }
    lastBtState = currentBtState;
  }

  // 2. Battery Monitoring, Smoothing & Percentage
  float calibFactor = 1.138; // Pichli baar ka calibration factor (zaroorat padne par adjust karein)
  int batRaw = analogRead(pinBat);
  float rawVoltage = (batRaw / 4095.0) * 3.3 * 2.0; 
  float currentVoltage = rawVoltage * calibFactor;

  // Smoothing Logic (Pehli baar seedha value lega, uske baad smooth karega)
  if (smoothedVoltage < 0) {
    smoothedVoltage = currentVoltage;
  } else {
    smoothedVoltage = (currentVoltage * alpha) + (smoothedVoltage * (1.0 - alpha));
  }

  // Percentage Math (3.2V to 4.2V map karna)
  int batPercent = map(smoothedVoltage * 100, 320, 420, 0, 100);
  batPercent = constrain(batPercent, 0, 100); // 0 se kam ya 100 se zyada na ho
  
  // Display update
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(tft.width() - 95, 10); 
  // Ye "%3d%%" ka matlab hai ki 3 digit space lega, isse text hilega nahi
  tft.printf("BAT:%3d%%(%1.1fv)", batPercent, smoothedVoltage);
  
  // 3. Left Stick & Pot
  int thrPct = map(thrRaw, 0, 4095, 0, 100); 
  int yawPct = map(yawRaw, 0, 4095, 0, 100);
  int p1Pct  = map(pot1Raw, 0, 4095, 0, 100);
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, 55); tft.printf("THR: %3d%% ", thrPct);
  tft.setCursor(10, 75); tft.printf("YAW: %3d%% ", yawPct);
  tft.setCursor(10, 95); tft.printf("PT1: %3d%% ", p1Pct);

  // 4. Right Stick & Pot
  int pitPct = map(pitchRaw, 0, 4095, 0, 100);
  int rolPct = map(rollRaw, 0, 4095, 0, 100);
  int p2Pct  = map(pot2Raw, 0, 4095, 0, 100);
  
  int rOffset = tft.width()/2 + 10;
  tft.setCursor(rOffset, 55); tft.printf("PIT: %3d%% ", pitPct);
  tft.setCursor(rOffset, 75); tft.printf("ROL: %3d%% ", rolPct);
  tft.setCursor(rOffset, 95); tft.printf("PT2: %3d%% ", p2Pct);

  // 5. Switches & Buttons
  int swY_row1 = 132;
  int swY_row2 = 145;
  int colWidth = tft.width() / 4;

  auto drawItem = [&](int x, int y, const char* label, bool state) {
    tft.setCursor(x, y);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.print(label);
    tft.print(":");
    if(state) {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.print("ON ");
    } else {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.print("OFF");
    }
  };

  drawItem(5, swY_row1, "ARM", armSt);
  drawItem(5 + colWidth, swY_row1, "MOD", modeSt);
  drawItem(5 + (colWidth*2), swY_row1, "SWA", swASt);
  drawItem(5 + (colWidth*3), swY_row1, "SWB", swBSt);

  drawItem(5, swY_row2, "BT1", b1St);
  drawItem(5 + colWidth, swY_row2, "BT2", b2St);
  drawItem(5 + (colWidth*2), swY_row2, "BT3", b3St);

  delay(20); 
}