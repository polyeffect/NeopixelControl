#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define DIN_PIN       D2   // Wemos D1 Pin
#define NUMPIXELS     144
#define BUFFERSIZE    255

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, DIN_PIN, NEO_RGB + NEO_KHZ800);

const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int average = 0;
int distance = 0;
int border = 15;
int cm = 0;
int wait = 10;
int r = 220, g = 255, b = 255;
int brightness = 0;
int maxBrightness = 160;
char temp[BUFFERSIZE + 1];
boolean isInit = false, isLoop = false;

uint32_t low = strip.Color(0, 0, 0);

void setup() {
  Serial.begin(57600);
  pinMode(BUILTIN_LED, OUTPUT);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  strip.begin();
  strip.show();
}

void loop() {
  if (!isInit) initLight();

  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();



  // Calculate Distance
  cm = (int) calculateDistance();
  Serial.print(cm);
  Serial.print("cm, ");
  Serial.println(isLoop);

  // Check Distance
  if (cm < border) {
    digitalWrite(BUILTIN_LED, LOW);
    if (!isLoop) lightOnFull();
  }
  else if (cm >= border) {
    digitalWrite(BUILTIN_LED, HIGH);
    if (isLoop) lightOff();
  }

  // Serial Communication
  while (Serial.available() > 0) {
    byte leng = Serial.readBytesUntil('k', temp, BUFFERSIZE);
    temp[leng] = 0;

    String s = temp;

    if (-1 != s.indexOf('t')) {
      String k = s.substring(s.indexOf('t') + 1, s.length());
      String rs = getValue(k, ',', 0);
      String gs = getValue(k, ',', 1);
      String bs = getValue(k, ',', 2);
      Serial.print(rs);
      Serial.print(gs);
      Serial.println(bs);
      r = rs.toInt();
      g = gs.toInt();
      b = bs.toInt();
    }

    if (-1 != s.indexOf('b')) {
      String b = s.substring(s.indexOf('b') + 1, s.length());
      if (isLoop) {
        brightness = b.toInt();
        
        if (brightness >= maxBrightness) brightness = maxBrightness;
        strip.setBrightness(brightness);
      }

      maxBrightness = b.toInt();
    }
  }

  delay(10);
}

void initLight() {
  isInit = true;
  strip.setBrightness(0);
//  strip.show();
}

void lightOnFull() {
  brightness += 5;
  if (brightness >= maxBrightness) {
    isLoop = true;
    brightness = maxBrightness;
  }
  strip.setBrightness(brightness);
//  strip.show();
}

void lightOff() {
  brightness -= 5;
  if (brightness <= 0) {
    brightness = 0;
    isLoop = false;
  }
  strip.setBrightness(brightness);
//  strip.show();
}


/*
  void lightOnHalf() {
  for (int i = 0; i <= 200; i += 5) {
    for (int j = 0; j < NUMPIXELS; j++) {
      strip.setPixelColor(j, strip.Color(i, i, i));

    }
    if (i >= 180) isLoop = true;
    strip.show();
    delay(wait);
  }
  }

  void lightOnFull() {
  for (int i = 0; i <= 200; i += 5) {
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
*/

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

