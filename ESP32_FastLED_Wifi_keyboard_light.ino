#include "FastLED.h"
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiUdp.h>

#define N 126
#define ledPin 26
// Params for width and height
const uint8_t kMatrixWidth = 18;
const uint8_t kMatrixHeight = 7;
const bool softWall = false;
#define NCOLORS 148
//const uint16_t colors[NCOLORS] = {CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Cyan, CRGB::Purple};
uint8_t gHue = 0;

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = false;
const bool    kMatrixVertical = false;
#define N (kMatrixWidth * kMatrixHeight)
CRGB leds[N];
unsigned char BRIGHTNESS = 100;

int status = WL_IDLE_STATUS;
char packetBuffer[256]; 
unsigned int localPort = 2880;  // local port to listen on
const char * ssid = "ESP32NeoFlash";
WiFiUDP Udp;

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * kMatrixWidth) + x;
    } else {
      i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
    }
  }

  if( kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if( y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (kMatrixWidth - 1) - x;
        i = (y * kMatrixWidth) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * kMatrixWidth) + x;
      }
    } else { // vertical positioning
      if ( x & 0x01) {
        i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
      } else {
        i = kMatrixHeight * (kMatrixWidth - x) - (y+1);
      }
    }
  }
  
  return i;
}

void setup() {
  FastLED.addLeds<NEOPIXEL,ledPin>(leds,N);
  FastLED.setBrightness(BRIGHTNESS);
  leds[XY(kMatrixWidth /2, kMatrixHeight / 2)] = CRGB::Red;
  FastLED.show();
  Serial.begin(115200);
  //while(!Serial);
  status = WiFi.softAP(ssid,"87654321");
  delay(100);  // wait a second
  Udp.begin(localPort);  
  Serial.println("");
  Serial.print(F("Started SSID: "));
  Serial.println(ssid);
  Serial.print(F("IP: "));
  Serial.print(WiFi.softAPIP());  
  Serial.print(F(" UDP Port: "));
  Serial.println(localPort);
  Serial.println(F("Display keys types on NeoFlash"));
}

void loop() {
  static int hue = 0;
  static byte dothue = 0;
  int command = 0;
  //confetti();
  fadeToBlackBy(leds, N, 5);
  //bpm();
  //juggle();
  int packetSize = Udp.parsePacket();
  if(packetSize){
    Udp.read(packetBuffer, 255);  
    command = packetBuffer[0];
    Serial.println(command);
  }
  if(command){
    if(command == 1){
      fill_solid(leds, N, CRGB::Black);
    } else if(command == 27){
      BRIGHTNESS = BRIGHTNESS + 10 > 255 ? 255 : BRIGHTNESS + 10;
      FastLED.setBrightness(BRIGHTNESS);
      Serial.print(F("Brightness: ")); Serial.println(BRIGHTNESS);
    } else if(command == 26){
      BRIGHTNESS = BRIGHTNESS - 10 < 0 ? 0 : BRIGHTNESS - 10;
      FastLED.setBrightness(BRIGHTNESS);
      Serial.print(F("Brightness: ")); Serial.println(BRIGHTNESS);
    } else if(command == 72 || command == 75 || command == 77 || command == 80 || command == 28){
      dothue = 0;
      for(int i = 0; i < 4; i++) {
        leds[beatsin16( 2*i+7, 0, N-1 )] |= CHSV(dothue, 200, 255);
        dothue += 64;
      }
    } else if(command == 14 || command == 42 || command == 54){
      for(int i = 0; i < 30; i++){
        sinelon();
        FastLED.show();
      }
    } else {
      int pos = random16(N);
      leds[pos] += CHSV(gHue + random8(64), 200, 255);
      gHue = gHue + 32 > 360 ? gHue + 32 - 360 : gHue + 32;
    }
  }
  FastLED.show();
}

void confetti(){
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, N, 10);
  int pos = random16(N);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void bpm(){
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for(int i = 0; i < N; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, N, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, N-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void sinelon(){
  // a colored dot sweeping back and forth, with fading trails
  static uint8_t myHue = 0;
  fadeToBlackBy( leds, N, 20);
  int pos = beatsin16(13, 0, N-1 );
  leds[pos] += CHSV(myHue++, 255, 192);
  if(myHue >= 360) myHue = 0;
}
