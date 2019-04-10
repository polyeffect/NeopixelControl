#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//#define PIN           6    // Arduino Pin
#define DIN_PIN       D2   // Wemos D1 Pin
#define NUMPIXELS     144

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, DIN_PIN, NEO_RGB + NEO_KHZ800);

const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;
int distance = 0;
int border = 55;
int cm = 0;
int wait = 10;
boolean isLoop = false;

uint32_t low = strip.Color(0, 0, 0);

void setup() {
  Serial.begin(9600);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  strip.begin();
  strip.show();
}

void loop() {
  // Calculate Distance
  cm = (int) calculateDistance();
  Serial.print(cm);
  Serial.print("cm, ");
  Serial.println(isLoop);

  if (cm < border) {
    if (!isLoop) lightOnHalf();
  }
  else if (cm >= border) {
    if (isLoop) lightOff();
  }

  delay(100);
}

void lightOnHalf() {
  for (int i = 0; i <= 180; i += 10) {
    for (int j = 0; j < NUMPIXELS; j++) {
      strip.setPixelColor(j, strip.Color(i, i, i));
      
    }
    if (i >= 180) isLoop = true;
    strip.show();
    delay(wait);
  }

  
}

void lightOnFull() {
  for (int i = 0; i <= 180; i += 10) {
    for (int j = 0; j < NUMPIXELS; j++) {
      strip.setPixelColor(j, strip.Color(i, i, i));
    }
    if (i >= 180) isLoop = true;
    strip.show();
    delay(wait);
  }

  
}

void lightOff() {
  for (int i = 180; i >= 0; i -= 10) {
    for (int j = 0; j < NUMPIXELS; j++) {
      strip.setPixelColor(j, strip.Color(i, i, i));
      
    }
    if (i <= 0) isLoop = false;
    strip.show();
    delay(wait);
  }

  
}

// Calculate Distance
int calculateDistance() {
  int raw = analogRead(A0);
  total = total - readings[readIndex];
  readings[readIndex] = raw;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) readIndex = 0;
  average = total / numReadings;

  int volt = map(average, 0, 1023, 0, 5000);
  distance = (27.61 / (volt - 0.1696)) * 1000;
  return distance;
}

