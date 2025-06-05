// ============= НАЛАШТУВАННЯ =============
// -------- КНОПКА -------
#define USE_BUTTON 1    

// -------- ЧАС -------
#define GMT 3              
#define NTP_ADDRESS  "europe.pool.ntp.org"    

// -------- СХІД -------
#define DAWN_BRIGHT 200      
#define DAWN_TIMEOUT 1       

// ---------- МАТРИЦЯ ---------
#define BRIGHTNESS 40         
#define CURRENT_LIMIT 2000    

#define WIDTH 16              
#define HEIGHT 16             

#define COLOR_ORDER GBR       

#define MATRIX_TYPE 0         
#define CONNECTION_ANGLE 0    
#define STRIP_DIRECTION 0     

#define LED_PIN D4             
#define BTN_PIN D2
#define MODE_AMOUNT 18

#define NUM_LEDS WIDTH * HEIGHT
#define SEGMENTS 1            

// ---------------- БІБЛІОТЕКИ -----------------
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define NTP_INTERVAL 60 * 1000    

#include "timer2Minim.h"
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <GyverButton.h>
#include "fonts.h"

// ------------------- ТИПИ --------------------
CRGB leds[NUM_LEDS];
WiFiServer server(80);
WiFiUDP Udp;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, GMT * 3600, NTP_INTERVAL);
timerMinim timeTimer(1000);
timerMinim timeStrTimer(120);
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

// ----------------- ЗМІННІ ------------------
const char* autoConnectSSID = AC_SSID;
const char* autoConnectPass = AC_PASS;
const char AP_NameChar[] = AP_SSID;
const char WiFiPassword[] = AP_PASS;
unsigned int localPort = AP_PORT;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; 
String inputBuffer;
static const byte maxDim = max(WIDTH, HEIGHT);
struct {
  byte brightness = 50;
  byte speed = 30;
  byte scale = 40;
} modes[MODE_AMOUNT];

struct {
  boolean state = false;
  int time = 0;
} alarm[7];

const byte dawnOffsets[] = {5, 10, 15, 20, 25, 30, 40, 50, 60};
byte dawnMode;
boolean dawnFlag = false;
float thisTime;
boolean manualOff = false;
boolean sendSettings_flag = false;

int8_t currentMode = 0;
boolean loadingFlag = true;
boolean ONflag = true;
uint32_t eepromTimer;
boolean settChanged = false;

unsigned char matrixValue[8][16];
String lampIP = "";
byte hrs, mins, secs;
byte days;
String timeStr = "00:00";

void setup() {
  ESP.wdtDisable();

  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  FastLED.show();

  touch.setStepTimeout(100);
  touch.setClickTimeout(500);

  Serial.begin(115200);
  Serial.println();
  delay(1000);
}

void loop() {
  parseUDP();
  effectsTick();
  eepromTick();
  timeTick();
#if (USE_BUTTON == 1)
  buttonTick();
#endif
  ESP.wdtFeed();   
  yield();  
}

void eeWriteInt(int pos, int val) {
  byte* p = (byte*) &val;
  EEPROM.write(pos, *p);
  EEPROM.write(pos + 1, *(p + 1));
  EEPROM.write(pos + 2, *(p + 2));
  EEPROM.write(pos + 3, *(p + 3));
  EEPROM.commit();
}

int eeGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}
