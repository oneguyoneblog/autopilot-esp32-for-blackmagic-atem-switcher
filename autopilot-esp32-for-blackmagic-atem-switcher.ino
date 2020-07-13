/*****************
  test


******************/

#include <M5StickC.h>
#include "WiFi.h"
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>

IPAddress clientIp(192, 168, 178, 170);        // <= SETUP!  IP address of the Arduino
IPAddress switcherIp(192, 168, 178, 173);
ATEMstd AtemSwitcher;

// http://www.barth-dev.de/online/rgb565-color-picker/
#define GRAY  0x0020 //   8  8  8
#define GREEN 0x0200 //   0 64  0
#define RED   0xF800 // 255  0  0

const char* ssid = "yournetwork";
const char* password =  "yourpassword";

int cameras = 4;
int ledPin = 10;

int minTime = 3;     //3
int maxTime = 10;    //50

int programInput = 1;
int previewInput = 2;
int timeout = 1000;

void setup() {

  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // initialize the M5StickC object
  M5.begin();

  pinMode(ledPin, OUTPUT);  // LED: 1 is on Program (Tally)
  digitalWrite(ledPin, HIGH); // LED off

  // Initialize a connection to the switcher:
  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(true);
  AtemSwitcher.connect();
}

void loop() {

  // Check for packets, respond to them etc. Keeping the connection alive!
  AtemSwitcher.runLoop();

  static unsigned long start_time = millis();

  if (millis() - start_time >= timeout) {

    digitalWrite(ledPin, LOW);
    programInput = previewInput;
    previewInput = randomNumber();

    AtemSwitcher.changePreviewInput(previewInput);
    AtemSwitcher.changeProgramInput(programInput);

    drawLabel(BLACK, GRAY, HIGH);

    Serial.print("Program: ");
    Serial.println(programInput);

    start_time = millis();
    timeout = 1000 * random(minTime, maxTime + 1);
    digitalWrite(ledPin, HIGH);

  }
}

void drawLabel(unsigned long int screenColor, unsigned long int labelColor, bool ledValue) {
  digitalWrite(ledPin, ledValue);
  M5.Lcd.fillScreen(screenColor);
  M5.Lcd.setTextColor(labelColor, screenColor);
  M5.Lcd.drawString(String(programInput), 15, 40, 8);
}

int randomNumber() { // pseudo random non-consecutive number
  while (previewInput == programInput) {
    previewInput = random(1, cameras + 1);
  }

  return previewInput;
}
