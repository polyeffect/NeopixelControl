#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//#define PIN           6    // Arduino Pin
#define DIN_PIN       D2   // Wemos D1 Pin
#define NUMPIXELS     288

int wait = 100;
int incoming;
int r = 100, g = 100, b = 100;
String s;
char temp[255];
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, DIN_PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  Serial.begin(57600);

  strip.begin();
  strip.show();
}

void loop() {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();

  // Serial
  while (Serial.available() > 0) {
    byte leng = Serial.readBytesUntil('k', temp, 255);
    temp[leng] = 0;


    //    char* incoming = temp;

    String brightness;
    String temperature;

    String s = temp;
    //    int stringLength = s.length;
    //    Serial.println(s);

    if (-1 != s.indexOf('t')) {
      String t[3];
      temperature = s.substring(s.indexOf('t') + 1, s.length());
      //      Serial.println(temperature);
      String rs = getValue(temperature, ',', 0);
      String gs = getValue(temperature, ',', 1);
      String bs = getValue(temperature, ',', 2);
      Serial.print(rs);
      Serial.print(gs);
      Serial.println(bs);
      r = rs.toInt();
      g = gs.toInt();
      b = bs.toInt();
    }

    if (-1 != s.indexOf('b')) {
      brightness = s.substring(s.indexOf('b') + 1, s.length());
      Serial.println(brightness);
      int b = brightness.toInt();
      strip.setBrightness(b);
    }

    //    int brightness = atoi(incoming);
    //
    //
    //
    //    Serial.println(brightness);
    //    strip.setBrightness(brightness);
  }

  delay(1);
}

void lightOn() {
  for (int i = 0; i < 22; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }
  strip.show();
}

void lightOnFull() {
  for (int i = 0; i <= 250; i += 10) {
    for (int j = 0; j < 22; j++) {
      strip.setPixelColor(j, strip.Color(i, i, i));
    }
    strip.show();
    delay(wait);
  }
}

void lightOff() {
  for (int i = 250; i >= 0; i -= 10) {
    for (int j = 0; j < 22; j++) {
      strip.setPixelColor(j, strip.Color(i, i, i));
    }
    strip.show();
    delay(wait);
  }
}

String spliter(String sData, char cSeparator)
{
  int nCount = 0;
  int nGetIndex = 0 ;

  String sTemp = "";

  String sCopy = sData;

  while (true)
  {
    nGetIndex = sCopy.indexOf(cSeparator);

    if (-1 != nGetIndex)
    {
      sTemp = sCopy.substring(0, nGetIndex);

      return sTemp;

      sCopy = sCopy.substring(nGetIndex + 1);
    }
    else
    {
      return sCopy;
      break;
    }

    ++nCount;
  }

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

