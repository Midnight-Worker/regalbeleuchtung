#include <Adafruit_NeoPixel.h>

// -------------------------
// Grundeinstellungen
// -------------------------

#define LED_PIN     2
#define LED_COUNT   72
#define BRIGHTNESS  40

Adafruit_NeoPixel strip(
  LED_COUNT,
  LED_PIN,
  NEO_GRB + NEO_KHZ800
);

// -------------------------
// LED-Objekt
// -------------------------

struct LedObjekt {
  uint8_t nr;      // logische LED-Nummer: 1 bis 72
  uint8_t pixel;   // echte NeoPixel-Nummer: 0 bis 71
  uint8_t x;       // Spalte
  uint8_t y;       // Reihe
};

// -------------------------
// Mapping deiner LED-Anordnung
// -------------------------

const LedObjekt leds[] = {
  // Linker Block, oben nach unten
  {  1, 66, 0, 0 },
  {  2, 67, 1, 0 },
  {  3, 68, 2, 0 },
  {  4, 69, 3, 0 },
  {  5, 70, 4, 0 },
  {  6, 71, 5, 0 },

  {  7, 65, 0, 1 },
  {  8, 64, 1, 1 },
  {  9, 63, 2, 1 },
  { 10, 62, 3, 1 },
  { 11, 61, 4, 1 },
  { 12, 60, 5, 1 },

  { 13, 54, 0, 2 },
  { 14, 55, 1, 2 },
  { 15, 56, 2, 2 },
  { 16, 57, 3, 2 },
  { 17, 58, 4, 2 },
  { 18, 59, 5, 2 },

  { 19, 53, 0, 3 },
  { 20, 52, 1, 3 },
  { 21, 51, 2, 3 },
  { 22, 50, 3, 3 },
  { 23, 49, 4, 3 },
  { 24, 48, 5, 3 },

  { 25, 42, 0, 4 },
  { 26, 43, 1, 4 },
  { 27, 44, 2, 4 },
  { 28, 45, 3, 4 },
  { 29, 46, 4, 4 },
  { 30, 47, 5, 4 },

  { 31, 41, 0, 5 },
  { 32, 40, 1, 5 },
  { 33, 39, 2, 5 },
  { 34, 38, 3, 5 },
  { 35, 37, 4, 5 },
  { 36, 36, 5, 5 },

  // Rechter Block, oben nach unten
  { 37, 5, 0, 6 },
  { 38, 4, 1, 6 },
  { 39, 3, 2, 6 },
  { 40, 2, 3, 6 },
  { 41, 1, 4, 6 },
  { 42, 0, 5, 6 },

  { 43, 6, 0, 7 },
  { 44, 7, 1, 7 },
  { 45, 8, 2, 7 },
  { 46, 9, 3, 7 },
  { 47, 10, 4, 7 },
  { 48, 11, 5, 7 },

  // Korrigierte Reihe mit vergessener LED zwischen 12 und 13
  { 49, 17, 0, 8 },
  { 50, 16, 1, 8 },
  { 51, 15, 2, 8 },
  { 52, 14, 3, 8 },
  { 53, 13, 4, 8 },
  { 54, 12, 5, 8 },

  { 55, 18, 0, 9 },
  { 56, 19, 1, 9 },
  { 57, 20, 2, 9 },
  { 58, 21, 3, 9 },
  { 59, 22, 4, 9 },
  { 60, 23, 5, 9 },

  { 61, 29, 0, 10 },
  { 62, 28, 1, 10 },
  { 63, 27, 2, 10 },
  { 64, 26, 3, 10 },
  { 65, 25, 4, 10 },
  { 66, 24, 5, 10 },

  { 67, 30, 0, 11 },
  { 68, 31, 1, 11 },
  { 69, 32, 2, 11 },
  { 70, 33, 3, 11 },
  { 71, 34, 4, 11 },
  { 72, 35, 5, 11 }
};

// -------------------------
// UART-Befehlspuffer
// -------------------------

char commandBuffer[16];
uint8_t commandIndex = 0;

// -------------------------
// Setup
// -------------------------

void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();

  // kurzer Starttest
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.show();
  delay(250);
  strip.clear();
  strip.show();

  printHelp();
}

// -------------------------
// Loop
// -------------------------

void loop() {
  while (Serial.available() > 0) {
    char zeichen = Serial.read();
    verarbeiteZeichen(zeichen);
  }
}

// -------------------------
// Hilfe ausgeben
// -------------------------

void printHelp() {
  Serial.println();
  Serial.println("WS2812 UART Steuerung bereit.");
  Serial.println();
  Serial.println("Einzelne LEDs:");
  Serial.println("r27  = LED 27 rot");
  Serial.println("g1   = LED 1 gruen");
  Serial.println("b5   = LED 5 blau");
  Serial.println("w72  = LED 72 weiss");
  Serial.println("t10  = LED 10 tuerkis");
  Serial.println("y32  = LED 32 gelb");
  Serial.println("o32  = LED 32 aus");
  Serial.println();
  Serial.println("Alle LEDs:");
  Serial.println("ar   = alle rot");
  Serial.println("ag   = alle gruen");
  Serial.println("ab   = alle blau");
  Serial.println("aw   = alle weiss");
  Serial.println("at   = alle tuerkis");
  Serial.println("ay   = alle gelb");
  Serial.println("ao   = alle aus");
  Serial.println();
  Serial.println("Demo:");
  Serial.println("s    = Strandtest starten");
  Serial.println();
}

// -------------------------
// Zeichen sammeln
// -------------------------

void verarbeiteZeichen(char zeichen) {
  zeichen = tolower(zeichen);

  // Enter, Komma, Leerzeichen usw. führen den Befehl aus
  if (zeichen == '\n' || zeichen == '\r' || zeichen == ',' || zeichen == ' ') {
    if (commandIndex > 0) {
      commandBuffer[commandIndex] = '\0';
      verarbeiteBefehl(commandBuffer);
      commandIndex = 0;
    }
    return;
  }

  // Zeichen in Buffer schreiben
  if (commandIndex < sizeof(commandBuffer) - 1) {
    commandBuffer[commandIndex] = zeichen;
    commandIndex++;
  } else {
    Serial.println("Fehler: Befehl zu lang.");
    commandIndex = 0;
  }
}

// -------------------------
// Komplette Befehle auswerten
// -------------------------

void verarbeiteBefehl(const char* befehl) {
  Serial.print("Befehl empfangen: ");
  Serial.println(befehl);

  // Strandtest
  if (befehl[0] == 's' && befehl[1] == '\0') {
    Serial.println("Starte Strandtest...");
    strandTest();
    Serial.println("Strandtest fertig.");
    return;
  }

  // Alle LEDs setzen: ar, ag, ab, aw, at, ay, ao
  if (befehl[0] == 'a') {
    char farbBefehl = befehl[1];

    if (!istFarbBefehl(farbBefehl)) {
      Serial.println("Fehler: ungueltiger Alle-LEDs-Farbbefehl.");
      Serial.println("Beispiele: ar, ag, ab, aw, at, ay, ao");
      return;
    }

    uint32_t farbe = farbeAusBefehl(farbBefehl);
    setAllLeds(farbe);
    strip.show();

    Serial.print("OK: alle LEDs gesetzt auf ");
    Serial.println(farbBefehl);
    return;
  }

  // Einzelne LED setzen: r27, g1, y32, o32 usw.
  char farbBefehl = befehl[0];

  if (!istFarbBefehl(farbBefehl)) {
    Serial.println("Fehler: unbekannter Befehl.");
    return;
  }

  int nummer = atoi(&befehl[1]);

  if (nummer < 1 || nummer > 72) {
    Serial.print("Fehler: LED-Nummer ungueltig: ");
    Serial.println(nummer);
    return;
  }

  uint32_t farbe = farbeAusBefehl(farbBefehl);

  setLedByNumber(nummer, farbe);
  strip.show();

  Serial.print("OK: LED ");
  Serial.print(nummer);
  Serial.print(" gesetzt auf ");
  Serial.println(farbBefehl);
}

// -------------------------
// Prüfen, ob Zeichen ein Farbbefehl ist
// -------------------------

bool istFarbBefehl(char zeichen) {
  return zeichen == 'r' ||
         zeichen == 'g' ||
         zeichen == 'b' ||
         zeichen == 'w' ||
         zeichen == 't' ||
         zeichen == 'y' ||
         zeichen == 'o';
}

// -------------------------
// Farbe aus Buchstabe erzeugen
// -------------------------

uint32_t farbeAusBefehl(char farbe) {
  switch (farbe) {
    case 'r':
      return strip.Color(255, 0, 0);       // Rot

    case 'g':
      return strip.Color(0, 255, 0);       // Gruen

    case 'b':
      return strip.Color(0, 0, 255);       // Blau

    case 'w':
      return strip.Color(255, 255, 255);   // Weiss

    case 't':
      return strip.Color(0, 255, 255);     // Tuerkis

    case 'y':
      return strip.Color(255, 255, 0);     // Gelb

    case 'o':
      return strip.Color(0, 0, 0);         // Off / Aus

    default:
      return strip.Color(0, 0, 0);
  }
}

// -------------------------
// Einzelne logische LED setzen
// -------------------------

void setLedByNumber(uint8_t nummer, uint32_t farbe) {
  uint8_t index = nummer - 1;
  uint8_t pixel = leds[index].pixel;

  Serial.print("Mapping: logisch ");
  Serial.print(nummer);
  Serial.print(" -> Pixel ");
  Serial.println(pixel);

  strip.setPixelColor(pixel, farbe);
}

// -------------------------
// Alle logischen LEDs setzen
// -------------------------

void setAllLeds(uint32_t farbe) {
  for (uint8_t nummer = 1; nummer <= 72; nummer++) {
    uint8_t index = nummer - 1;
    uint8_t pixel = leds[index].pixel;
    strip.setPixelColor(pixel, farbe);
  }
}

// -------------------------
// Strandtest / Demo
// -------------------------

void strandTest() {
  colorWipe(strip.Color(255, 0, 0), 30);     // Rot
  colorWipe(strip.Color(0, 255, 0), 30);     // Gruen
  colorWipe(strip.Color(0, 0, 255), 30);     // Blau
  colorWipe(strip.Color(255, 255, 0), 30);   // Gelb
  colorWipe(strip.Color(0, 255, 255), 30);   // Tuerkis
  colorWipe(strip.Color(255, 255, 255), 30); // Weiss

  theaterChase(strip.Color(127, 127, 127), 50);
  theaterChase(strip.Color(127, 0, 0), 50);
  theaterChase(strip.Color(0, 0, 127), 50);

  rainbow(5);

  strip.clear();
  strip.show();
}

// -------------------------
// Strandtest-Helfer: Farbwischer
// -------------------------

void colorWipe(uint32_t farbe, int wartezeit) {
  strip.clear();

  for (uint8_t nummer = 1; nummer <= 72; nummer++) {
    uint8_t index = nummer - 1;
    uint8_t pixel = leds[index].pixel;

    strip.setPixelColor(pixel, farbe);
    strip.show();
    delay(wartezeit);
  }
}

// -------------------------
// Strandtest-Helfer: Theater Chase
// -------------------------

void theaterChase(uint32_t farbe, int wartezeit) {
  for (int durchlauf = 0; durchlauf < 10; durchlauf++) {
    for (int offset = 0; offset < 3; offset++) {
      strip.clear();

      for (int pixel = offset; pixel < LED_COUNT; pixel += 3) {
        strip.setPixelColor(pixel, farbe);
      }

      strip.show();
      delay(wartezeit);
    }
  }
}

// -------------------------
// Strandtest-Helfer: Regenbogen
// -------------------------

void rainbow(int wartezeit) {
  for (long firstPixelHue = 0; firstPixelHue < 3 * 65536; firstPixelHue += 256) {
    for (int pixel = 0; pixel < LED_COUNT; pixel++) {
      int pixelHue = firstPixelHue + (pixel * 65536L / LED_COUNT);
      strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(pixelHue)));
    }

    strip.show();
    delay(wartezeit);
  }
}
