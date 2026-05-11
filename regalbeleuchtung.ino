#include <Adafruit_NeoPixel.h>

// -------------------------
// Grundeinstellungen
// -------------------------

#define LED_PIN     2
#define LED_COUNT   72
#define BRIGHTNESS  40

#define COMMAND_TIMEOUT_MS 150

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
// UART-Eingabepuffer
// -------------------------

char befehlFarbe = 0;
int befehlNummer = 0;
bool leseNummer = false;
unsigned long letzteEingabeZeit = 0;

// -------------------------
// Setup
// -------------------------

void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();

  // Kurzer LED-Test beim Start:
  // Wenn das nicht leuchtet, liegt das Problem NICHT am UART,
  // sondern eher an Pin, Verdrahtung, Stromversorgung oder LED-Typ.
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.show();
  delay(500);

  strip.clear();
  strip.show();

  Serial.println();
  Serial.println("WS2812 UART Steuerung bereit.");
  Serial.println("Befehle:");
  Serial.println("r27 = LED 27 rot");
  Serial.println("g1  = LED 1 gruen");
  Serial.println("b5  = LED 5 blau");
  Serial.println("w72 = LED 72 weiss");
  Serial.println("t10 = LED 10 tuerkis");
  Serial.println("y32 = LED 32 gelb");
  Serial.println("o32 = LED 32 aus");
  Serial.println();
  Serial.println("Trennzeichen wie Komma oder Enter sind erlaubt, aber nicht mehr zwingend.");
}

// -------------------------
// Loop
// -------------------------

void loop() {
  while (Serial.available() > 0) {
    char zeichen = Serial.read();
    verarbeiteZeichen(zeichen);
  }

  // Automatisch ausführen, wenn nach der letzten Ziffer kurz nichts mehr kam.
  if (leseNummer && befehlNummer > 0) {
    if (millis() - letzteEingabeZeit > COMMAND_TIMEOUT_MS) {
      fuehreBefehlAus();
      resetBefehl();
    }
  }
}

// -------------------------
// Einzelnes UART-Zeichen verarbeiten
// -------------------------

void verarbeiteZeichen(char zeichen) {
  zeichen = tolower(zeichen);
  letzteEingabeZeit = millis();

  // Debug-Ausgabe: zeigt, dass überhaupt Zeichen ankommen
  Serial.print("Empfangen: ");
  if (zeichen == '\n') {
    Serial.println("\\n");
  } else if (zeichen == '\r') {
    Serial.println("\\r");
  } else {
    Serial.println(zeichen);
  }

  // Neuer Farbbefehl beginnt
  if (istFarbBefehl(zeichen)) {
    befehlFarbe = zeichen;
    befehlNummer = 0;
    leseNummer = true;

    Serial.print("Farbe erkannt: ");
    Serial.println(befehlFarbe);
    return;
  }

  // Ziffern nach dem Farbbefehl sammeln
  if (leseNummer && isDigit(zeichen)) {
    befehlNummer = befehlNummer * 10 + (zeichen - '0');

    Serial.print("Nummer bisher: ");
    Serial.println(befehlNummer);
    return;
  }

  // Trennzeichen: Befehl sofort ausführen
  if (leseNummer && befehlNummer > 0) {
    fuehreBefehlAus();
    resetBefehl();
    return;
  }

  // Irgendwas anderes ignorieren
}

// -------------------------
// Befehl zurücksetzen
// -------------------------

void resetBefehl() {
  befehlFarbe = 0;
  befehlNummer = 0;
  leseNummer = false;
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
// Befehl ausführen
// -------------------------

void fuehreBefehlAus() {
  if (!istFarbBefehl(befehlFarbe)) {
    Serial.println("Fehler: Kein gueltiger Farbbefehl.");
    return;
  }

  if (befehlNummer < 1 || befehlNummer > 72) {
    Serial.print("Fehler: LED-Nummer ungueltig: ");
    Serial.println(befehlNummer);
    return;
  }

  uint32_t farbe = farbeAusBefehl(befehlFarbe);

  setLedByNumber(befehlNummer, farbe);
  strip.show();

  Serial.print("OK: LED ");
  Serial.print(befehlNummer);
  Serial.print(" gesetzt auf ");
  Serial.println(befehlFarbe);
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
// Logische LED-Nummer setzen
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
