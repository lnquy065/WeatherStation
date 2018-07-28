#include <SPI.h>
#include <LoRa.h>
#include <SSD1306.h>
#include <Arduino.h>

#include "lib/SharpGP2Y10.h"
#include "lib/ML8511.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <MQ135.h>



// DEEP SLEEP MODE SETTING
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3000        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

// OLED SETTING
SSD1306 display(0x3c, 4, 15);

// LORA SETTING
#define SS 18
#define RST 14
#define DI0 26
#define BAND 433E6 //915E6
 
// WIFI SETTING
const char* ssid     = "Quy";
const char* password = "0937437982";

// SENSOR SETTING
int counter = 0;
#define SENSOR_PIN_DUST_VO A0
#define SENSOR_PIN_DUST_LEDPIN 2

#define SENSOR_PIN_UV_VREF A1
#define SENSOR_PIN_UV_VO A3
#define SENSOR_PIN_CO2 A2

SharpGP2Y10 sensorDust(SENSOR_PIN_DUST_VO, SENSOR_PIN_DUST_LEDPIN);
MQ135 sensorCo2(SENSOR_PIN_CO2);
ML8511 sensorUV(SENSOR_PIN_UV_VO, SENSOR_PIN_UV_VREF);

// FUNCTION DECLARE
void loraSend(String);
int averageAnalogRead(int, int);
int readUVDensity(int, int);

void initSerial();
void initPinMode();
void initOLED();
void initLoRa();
void initWiFi();
void initSensors();
void sendHTTP();

void print_wakeup_reason();

//============= BODY =======================
void setup() {
  initSerial();
  initOLED();
  // initPinMode();
  // initWiFi();
  // // digitalWrite(25, HIGH);
  // sendHTTP();
  sensorCo2.calibrate();
}

void loop() {
  Serial.println("C02: " + String(sensorCo2.readCO2()) );
  Serial.println("UV: "+ String(sensorUV.readUVIntensity()) );
  Serial.println("Dust: "+ String(sensorDust.getDustDensity()) );
  // float dustVal = sensorDust.getDustDensity();
  // display.clear();
  // display.setFont(ArialMT_Plain_10);
  // display.drawString(3, 5, "Dust density: ");
  // display.drawString(70, 5, String(dustVal));
  // display.drawString(3, 15, "Co2 density: ");
  // display.drawString(70, 15, String(dustVal));
  // display.display();

  delay(2000);
}



//============== FUNCTION BODY

void sendHTTP() {
   HTTPClient http;   
   http.begin("http://qstation.herokuapp.com/node/N01/200/0.2/0");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain");             //Specify content-type header
   http.GET();
}

void loraSend(String data) {
 LoRa.beginPacket() ;
  LoRa.print("Hello ") ;
  LoRa.print(counter) ;
  LoRa.endPacket();
}




void initSerial() {
  Serial.begin(9600);
  while (!Serial);
}
void initPinMode() {
  pinMode(25, OUTPUT); //Onboard LED
  pinMode(16, OUTPUT);
}

void initOLED() {
  digitalWrite(16, LOW); //Reset OLED
  delay(50);
  display.init();
  display.display();
}

void initLoRa() {
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);
  LoRa.setSpreadingFactor(12);
  LoRa.setCodingRate4(4/5);
  LoRa.setSignalBandwidth(123E3);
  LoRa.setPreambleLength(5);
  delay(1000);
  Serial.println("LoRa Initial");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  delay(2000);
}

void initWiFi() {
WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}