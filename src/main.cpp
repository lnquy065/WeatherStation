#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

#include "lib/SharpGP2Y10.h"
#include "lib/ML8511.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <MQ135.h>
#include <SSD1306.h>

String NODE_ID = "N01";

// DEEP SLEEP MODE SETTING
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

// OLED
SSD1306 display(0x3c, 4, 15);
byte OLED_FONT_SIZE =  12;
byte OLED_LINE_OFFSET = 2;

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
#define BOOT_LED 12
#define SENSOR_PIN_DUST_VO A0
#define SENSOR_PIN_DUST_LEDPIN 2

#define SENSOR_PIN_UV_VREF A1
#define SENSOR_PIN_UV_VO A3
#define SENSOR_PIN_CO2 A2

#define BOOT_FAN 22

SharpGP2Y10 sensorDust(SENSOR_PIN_DUST_VO, SENSOR_PIN_DUST_LEDPIN);
MQ135 sensorCo2(SENSOR_PIN_CO2);
ML8511 sensorUV(SENSOR_PIN_UV_VO, SENSOR_PIN_UV_VREF);

String co2String, uvString, dustString;
int lora_counter;

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

void oledPrint(int, int, String);
void print_wakeup_reason();
void run();

//============= BODY =======================
void setup() {
  initSerial();
  initPinMode();
  initOLED();
  initLoRa();
  delay(1000);
  digitalWrite(BOOT_LED, HIGH);
  sensorCo2.calibrate();
  Serial.println(bootCount);
  //on fan
   digitalWrite(BOOT_FAN, HIGH);
  delay(3000);
  run();
  delay(1000);
  

  //go to sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {

}

void run() {
  bootCount+=1;
  co2String = String(sensorCo2.readCO2()) ;
  dustString = String(sensorDust.getDustDensity());
  uvString = String(sensorUV.readUVIntensity());

  loraSend( NODE_ID + "/" + co2String + "/" + dustString + "/" + uvString);

  // Serial.println("C02: " + co2String);
  // Serial.println("UV: " +uvString);
  // Serial.println("Dust: " +dustString);
  // Serial.println("Boot: " +bootCount);

  // display.clear();
  // display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // display.setFont(Arimo_Regular_12);
  // display.drawString(0, 0, "Co2: " + co2String + " ppm");
  // display.drawString(0, 12 + 3, "UV: " + uvString + " mW/cm²");
  // display.drawString(0, 25 + 3, "Dust: " + dustString + " mg/m³");
  // display.drawString(0, 39 + 3 , "LoRa sent: " + String(bootCount) + " pkgs");
  // display.display();
}


//============== FUNCTION BODY

void oledPrint(int x, int y, String s) {
  char __s[sizeof(s+1)];
  s.toCharArray(__s, sizeof(__s));
  // oled.drawStr(0,1, __s );
}

void sendHTTP() {
   HTTPClient http;   
   http.begin("http://qstation.herokuapp.com/node/N01/200/0.2/0");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain");             //Specify content-type header
   http.GET();
}

void loraSend(String data) {
 LoRa.beginPacket() ;
  LoRa.print(data);
  LoRa.endPacket();
}




void initSerial() {
  Serial.begin(9600);
  while (!Serial);
}
void initPinMode() {
  pinMode(25, OUTPUT); //Onboard LED
  pinMode(16, OUTPUT);
  pinMode(BOOT_LED, OUTPUT);
  pinMode(BOOT_FAN, OUTPUT);
}

void initOLED() {
  digitalWrite(16, LOW);
  delay(50);
   digitalWrite(16, HIGH);
  display.init();
  display.flipScreenVertically();  
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

