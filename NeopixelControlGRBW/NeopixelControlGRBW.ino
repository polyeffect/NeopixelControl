#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define DIN_PIN       D2   // Wemos D1 Pin
#define NUMPIXELS     144
#define BUFFERSIZE    255

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, DIN_PIN, NEO_GRBW + NEO_KHZ800);

const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;
int distance = 0;
int border = 15;
int cm = 0;
int wait = 10;
uint32_t Color1, Color2;
int r = 168, g = 168, b = 168;
uint8_t rn = 127, gn = 127, bn = 127, br = 225;
int brightness = 0;
int maxBrightness = 255;
int fadeStep = 0;
char temp[BUFFERSIZE + 1];
boolean isInit = false, isDetected = false, isParty = false, isPrvParty = false;

uint32_t low = strip.Color(0, 0, 0);

void setup() {
  Serial.begin(57600);
  pinMode(BUILTIN_LED, OUTPUT);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  strip.begin();
  strip.show();
  strip.clear();
}

void loop() {
  if (!isInit) initLight();

  // Calculate Distance
  cm = (int) calculateDistance();

  // Check Distance
  if (cm < border) {
    digitalWrite(BUILTIN_LED, LOW);
    if (!isDetected) {
      isDetected = true;
      Serial.print("on#");
    }

    lightOnFull();
  }
  else if (cm >= border) {
    digitalWrite(BUILTIN_LED, HIGH);
    if (isDetected) {
      isDetected = false;
      Serial.print("off#");
    }

    lightOff();
  }

  // Serial Communication
  while (Serial.available() > 0) {
    byte leng = Serial.readBytesUntil('k', temp, BUFFERSIZE);
    temp[leng] = 0;

    String s = temp;

    if (-1 != s.indexOf('t')) {
      isParty = false;
      isPrvParty = false;
      String k = s.substring(s.indexOf('t') + 1, s.length());
      String rs = getValue(k, ',', 0);
      String gs = getValue(k, ',', 1);
      String bs = getValue(k, ',', 2);
      rn = rs.toInt();
      gn = gs.toInt();
      bn = bs.toInt();
    }

    if (-1 != s.indexOf('m')) {
      isParty = false;
      String k = s.substring(s.indexOf('m') + 1, s.length());
      String rs = getValue(k, ',', 0);
      String gs = getValue(k, ',', 1);
      String bs = getValue(k, ',', 2);
      r = rs.toInt();
      g = gs.toInt();
      b = bs.toInt();
      fadeUpdate(strip.Color(r, g, b));
    }

    if (-1 != s.indexOf('p')) {
      isParty = true;
      isPrvParty = true;
      fadeUpdate(strip.Color(r, g, b));
    }

    if (-1 != s.indexOf('b')) {
      String bs = s.substring(s.indexOf('b') + 1, s.length());
      brightness = bs.toInt();

      if (brightness >= maxBrightness) brightness = maxBrightness;
      strip.setBrightness(brightness);
      strip.show();
      maxBrightness = bs.toInt();
    }
  }
  delay(1);
}

void initLight() {
  isInit = true;
  Color1 = strip.Color(r, g, b, 225);
  strip.setBrightness(0);
}

void lightOnFull() {
  brightness += 5;
  if (brightness >= maxBrightness) {
    brightness = maxBrightness;
  }
  for (int i = 0; i < NUMPIXELS; i++) {
    if (!isParty) strip.setPixelColor(i, strip.Color(rn, gn, bn, 225));
    else strip.setPixelColor(i, Wheel(i & 255));
  }
  strip.show();
  strip.setBrightness(brightness);
}

void lightOff() {
  brightness -= 5;
  if (brightness <= 0) {
    brightness = 0;
  }
  strip.setBrightness(brightness);
  strip.show();
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

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

uint8_t Red(uint32_t color)
{
  return (color >> 16) & 0xFF;
}

uint8_t Green(uint32_t color)
{
  return (color >> 8) & 0xFF;
}

uint8_t Blue(uint32_t color)
{
  return color & 0xFF;
}

uint8_t Brightness(uint32_t color) {
  return color & 0xFF;
}

void fadeUpdate(uint32_t color) {
  int n = 10;
  for (int i = 0; i < n; i++)
  {
    if (!isParty) {
      if (isPrvParty) {
        Serial.println("preve party");
        for (int j = 0; j < NUMPIXELS; j++) {
          rn = ((Red(Wheel(j & 255)) * (n - i)) + (Red(color) * i)) / n;
          gn = ((Green(Wheel(j & 255)) * (n - i)) + (Green(color) * i)) / n;
          bn = ((Blue(Wheel(j & 255)) * (n - i)) + (Blue(color) * i)) / n;
          br = ((68 * (n - i)) + (225 * i)) / n;
          if (j == NUMPIXELS - 1 && i == n - 1) if (isPrvParty) isPrvParty = false;
          strip.setPixelColor(j, strip.Color(rn, gn, bn, br));
        }

      } else {
        rn = ((Red(Color1) * (n - i)) + (Red(color) * i)) / n;
        gn = ((Green(Color1) * (n - i)) + (Green(color) * i)) / n;
        bn = ((Blue(Color1) * (n - i)) + (Blue(color) * i)) / n;
        for (int j = 0; j < NUMPIXELS; j++) {
          strip.setPixelColor(j, strip.Color(rn, gn, bn, 225));
        }
      }
    } else {
      for (int j = 0; j < NUMPIXELS; j++) {
        rn = ((Red(Color1) * (n - i)) + (Red(Wheel(j & 255)) * i)) / n;
        gn = ((Green(Color1) * (n - i)) + (Green(Wheel(j & 255)) * i)) / n;
        bn = ((Blue(Color1) * (n - i)) + (Blue(Wheel(j & 255)) * i)) / n;
        br = ((225 * (n - i)) + (68 * i)) / n;
        strip.setPixelColor(j, strip.Color(rn, gn, bn, br));
      }
    }

    strip.show();
    delay(10);
  }
  Color1 = strip.Color(rn, gn, bn);
}

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 145) {
    return strip.Color(255 - WheelPos * 1.7, 0, WheelPos * 1.7, 68);
  }
}
