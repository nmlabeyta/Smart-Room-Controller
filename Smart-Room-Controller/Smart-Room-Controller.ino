/*
  Project: Smart Room Controller
  Description: Control Hue lights and Wemo outlets with one device
  Author: Lee Abeyta
  Date:7/28/20
*/


#include <Encoder.h>
#include <SPI.h>
#include <Ethernet.h>
#include <mac.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include "WEMO.h"
#include <hue.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)

bool status;
bool autO = true;
bool screeN = true;
bool menU = false;
bool modE = false;
bool settingS = false;
bool bF = false;
bool nighT = false;
bool lighT = false;
bool wemO = false;
bool thermO = false;
bool thermoSet = false;
bool sleepTime = false;

float pos;
float tempC;
float tempF;

int timeSet;
int setSleep;
int thermoTemp;
int tempSet;
int mainSelect;
int buttonOut = 23;
int buttonIn = 22;
int buttoN;
int daY;
int montH;
int yeaR;
int c = 0;
int wemo;
unsigned long currentMil;
unsigned long currentMil2;
unsigned long lastMil;
unsigned long lastMil2;
unsigned long timeNow;
unsigned long Interval = 200;


String incomingValue = "";

/*bool timerset = false;    //Boolean variables for the click functions and the timerSet() function
  bool doubleclick = false;
  bool buttonState = true;
  bool stopTime = false;
  String incomingValue = "";
  long double i;
  unsigned long currentMil;
  unsigned long lastMil;
  int Second = 1000;
  unsigned int m;
  unsigned int s;
  int y = 0;
  int mm = 0;
  unsigned int ss = 0;
*/

Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Encoder myEnc(5, 6);
IPAddress ip(192, 168, 1, 19); // Teensy 
WEMO weMO(wemo);


void setup() {            //Starts the Serial monitor
  Serial.begin(9600);
  while (!Serial);

  Ethernet.begin(mac, ip);
  Serial.println("connecting...");

  status = bme.begin(0x76);
  Serial.printf("Initializing BME...\n");

  if (status == false) {
    Serial.printf("BME initialization has failed\n");
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  pinMode(buttonOut, OUTPUT);
  pinMode(buttonIn, INPUT);

  timeNow = 3780000;
  daY = 1;
  montH = 8;
  yeaR = 2020;

   // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
}

void loop() {
  tempC = bme.readTemperature();
  tempF = map(tempC, -273.15, 1000.0, -459.67, 1832.0);

  digitalWrite(buttonOut, HIGH);
  buttoN = digitalRead(buttonIn);

  mainFunc();

}

void mainFunc() {
  currentMil = millis();

  if (currentMil - lastMil >= Interval) {
    if (buttoN == HIGH) {
      c++;
    }
    screen();
    menu();
    settings();
    thermoset();
    sleeptime();
    mode();
    automatic();
    light();
    Wemo();
    lastMil = currentMil;
  }
}

void screen() {
  if (screeN == true) {
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner
    display.printf("%u:%u AM", timeNow / 360000, timeNow / 126000);

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(80, 0);           // Start at top-left corner
    display.printf("%i/%i/%i", montH, daY, yeaR);

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(44, 12);            // Start at top-left corner
    display.printf("%0.2f F", tempF);

    if (autO == true) {
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(36, 24);            // Start at top-left corner
      display.printf("Mode:Auto");
    }
    else {
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(34, 24);            // Start at top-left corner
      display.printf("Mode:Manual");
    }
    display.display();
  }
  if (c == 1) {
    autO = false;
    screeN = false;
    menU = true;
    display.clearDisplay();
    display.display();
  }
}

void menu() {
  if (menU == true) {
    int Mode = 0;
    int Settings = 1;
    int Back = 2;
    if (c == 1) {
      pos = myEnc.read();
      if (pos >= 96) {
        pos = 96;
        myEnc.write(96);
      } else if (pos <= 0) {
        pos = 0;
        myEnc.write(0);
      }
      mainSelect = map(pos, 0, 96, 0, 2);

      if (Mode == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Mode");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-Settings");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Back");

        display.display();

      }
      if (Settings == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Mode");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-Settings");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Back");

        display.display();

      }
      if (Back == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Mode");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-Settings");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Back");

        display.display();

      }

    }
    if (c == 2) {
      if (Mode == mainSelect) {
        menU = false;
        modE = true;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }
      if (Settings == mainSelect) {
        menU = false;
        settingS = true;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }
      if (Back == mainSelect) {
        menU = false;
        screeN = true;
        c = 0;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }

    }
  }
}

void settings() {
  if (settingS == true) {
    int ThermoSet = 0;
    int SleepTime = 1;
    int Back = 2;
    if (c == 2) {
      pos = myEnc.read();
      if (pos >= 96) {
        pos = 96;
        myEnc.write(96);
      } else if (pos <= 0) {
        pos = 0;
        myEnc.write(0);
      }
      mainSelect = map(pos, 0, 96, 0, 2);

      if (ThermoSet == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(24, 0);            // Start at top-left corner
        display.printf("-Set Thermostat");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(24, 12);            // Start at top-left corner
        display.printf("-Set Sleep Time");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(24, 24);            // Start at top-left corner
        display.printf("-Back");

        display.display();

      }
      if (SleepTime == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(24, 0);            // Start at top-left corner
        display.printf("-Set Thermostat");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(24, 12);            // Start at top-left corner
        display.printf("-Set Sleep Time");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(24, 24);            // Start at top-left corner
        display.printf("-Back");

        display.display();

      }
      if (Back == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(24, 0);            // Start at top-left corner
        display.printf("-Set Thermostat");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(24, 12);            // Start at top-left corner
        display.printf("-Set Sleep Time");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(24, 24);            // Start at top-left corner
        display.printf("-Back");

        display.display();

      }

    }
    if (c == 3) {
      if (ThermoSet == mainSelect) {
        settingS = false;
        thermoSet = true;
        display.clearDisplay();
        display.display();
      }
      if (SleepTime == mainSelect) {
        settingS = false;
        sleepTime = true;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }
      if (Back == mainSelect) {
        settingS = false;
        menU = true;
        c = 1;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }

    }
  }
}

void thermoset() {
  if (thermoSet == true) {
    if (c == 3) {
      pos = myEnc.read();
      if (pos >= 96) {
        pos = 96;
        myEnc.write(96);
      } else if (pos <= 0) {
        pos = 0;
        myEnc.write(0);
      }
      tempSet = map(pos, 0, 96, 60, 85);

      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Set Temperature");



      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      display.printf("%i F", tempSet);

      display.display();

    }
    if (c == 4) {
      thermoTemp = tempSet;
      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Themostat set to");



      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      display.printf("%i F", thermoTemp);

      display.display();

      delay(1500);
      thermoSet = false;
      settingS = true;
      c = 2;
      pos = 0;
      myEnc.write(0);
      display.clearDisplay();
      display.display();

    }
  }
}

void sleeptime() {
  if (sleepTime == true) {
    int m;
    int s;
    if (c == 3) {
      pos = myEnc.read();
      if (pos >= 96) {
        pos = 96;
        myEnc.write(96);
      } else if (pos <= 0) {
        pos = 0;
        myEnc.write(0);
      }
      timeSet = map(pos, 0, 96, 0, 600);


      if (timeSet / 60 < 1) {  //If the time is less than 60 seconds, set the interger m to zero
        m = 0;                //else set m to 1
      }
      else {
        m = timeSet / 60;
      }

      if (timeSet / 60 > 1) {             //if the time is greater than 1 minute,
        s = timeSet % 60;
      }
      else if (timeSet >= 60) {      //if the time is set to a minute e.g. 5:00,
        s = 0;                          //sets s to zero in the case of 5:60
      }
      else {                  //else sets s to i/1000
        s = timeSet;
      }

      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Set Sleep Time");

      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      if (s < 10) {
        display.printf("%i:0%i", m, s);
      } else {
        display.printf("%i:%i", m, s);
      }

      display.display();


    }
    if (c == 4) {
      setSleep = timeSet;

      if (timeSet / 60 < 1) {  //If the time is less than 60 seconds, set the interger m to zero
        m = 0;                //else set m to 1
      }
      else {
        m = timeSet / 60;
      }

      if (timeSet / 60 > 1) {             //if the time is greater than 1 minute,
        s = timeSet % 60;
      }
      else if (timeSet >= 60) {      //if the time is set to a minute e.g. 5:00,
        s = 0;                          //sets s to zero in the case of 5:60
      }
      else {                  //else sets s to i/1000
        s = timeSet;
      }
      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Sleep Time set to");



      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      if (s < 10) {
        display.printf("%i:0%i", m, s);
      } else {
        display.printf("%i:%i", m, s);
      }
      display.display();

      delay(1500);
      sleepTime = false;
      settingS = true;
      c = 2;
      pos = 0;
      myEnc.write(0);
      display.clearDisplay();
      display.display();

    }

  }
}

void mode() {
  if (modE == true) {
    int Auto = 0;
    int BF = 1;
    int Night = 2;
    int Light = 3;
    int Wemo = 4;
    int Thermo = 5;
    int Back = 6;
    if (c == 2) {
      pos = myEnc.read();
      if (pos >= 96) {
        pos = 96;
        myEnc.write(96);
      } else if (pos <= 0) {
        pos = 0;
        myEnc.write(0);
      }
      mainSelect = map(pos, 0, 96, 0, 6);

      if (Auto == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Auto");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-BF");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Night");

        display.display();

      }
      if (BF == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Auto");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-BF");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Night");

        display.display();

      }
      if (Night == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Auto");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-BF");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Night");

        display.display();

      }
      if (Light == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Light");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-Wemo");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Thermo");

        display.display();

      }

      if (Wemo == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Light");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-Wemo");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Thermo");

        display.display();
      }
      if (Thermo == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Light");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 12);            // Start at top-left corner
        display.printf("-Wemo");

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 24);            // Start at top-left corner
        display.printf("-Thermo");

        display.display();

      }
      if (Back == mainSelect) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
        display.setCursor(34, 0);            // Start at top-left corner
        display.printf("-Back");

        display.display();

      }
    }
    if (c == 3) {
      if (Auto == mainSelect) {
        modE = false;
        autO = true;
        display.clearDisplay();
        display.display();
      }
      if (BF == mainSelect) {
        modE = false;
        bF = true;
        display.clearDisplay();
        display.display();
      }
      if (Night == mainSelect) {
        modE = false;
        nighT = true;
        display.clearDisplay();
        display.display();
      }
      if (Light == mainSelect) {
        modE = false;
        lighT = true;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }
      if (Wemo == mainSelect) {
        modE = false;
        wemO = true;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }
      if (Thermo == mainSelect) {
        modE = false;
        thermO = true;
        display.clearDisplay();
        display.display();
      }
      if (Back == mainSelect) {
        modE = false;
        menU = true;
        c = 1;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }
    }
  }
}

void automatic() {
  if (autO == true) {
    if (tempF > (thermoTemp + 1) || tempF < (thermoTemp - 1)) {

    }
  }
}

void light() {

  if (lighT == true) {
    static int onOff;
    static int lightState;
    static int lightSelect;
    static int lightN;
    static int color;
    static int i;
    static int intensity;
    static int hueColor;
    static int brightNess;

    pos = myEnc.read();
    if (pos >= 96) {
      pos = 96;
      myEnc.write(96);
    } else if (pos <= 0) {
      pos = 0;
      myEnc.write(0);
    }
    if (c == 3) {
      lightSelect = map(pos, 0, 96, 1, 5);

      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Selected Light");

      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(48, 16);            // Start at top-left corner
      if (lightSelect < 5) {
        display.printf("%i", lightSelect);
      }
      else {
        display.printf("All");
      }

      display.display();
    }
    if (c == 4) {
      lightN = lightSelect;
      int On = 1;
      int OfF = 0;

      onOff = map(pos, 0, 96, 0, 1);

      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Turn Light");



      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      if (On == onOff) {
        display.printf("ON");
      }
      else if (OfF == onOff) {
        display.printf("OFF");
      }
      display.display();

    }
    if (c == 5) {
      lightState = onOff;

      if (lightState == 0) {
        if (lightN < 5) {
          setHue(lightN, false, 0, 0);
          getHue(lightN);
          lighT = false;
          modE = true;
          c = 2;
          pos = 0;
          myEnc.write(0);
          display.clearDisplay();
          display.display();
        }
        else if (lightN == 5) {
          for (i = 1; i < 5; i++) {
            setHue(i, false, 0, 0);
            getHue(i);
          }
          lighT = false;
          modE = true;
          c = 2;
          pos = 0;
          myEnc.write(0);
          display.clearDisplay();
          display.display();
        }
      }
      else {

        color = map(pos, 0, 96, 0, 6);

        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(16, 0);            // Start at top-left corner
        display.printf("Light Color");



        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 16);            // Start at top-left corner
        if (color == 0) {
          display.printf("RED");
        } else if (color == 1) {
          display.printf("ORANGE");
        } else if (color == 2) {
          display.printf("YELLOW");
        } else if (color == 3) {
          display.printf("GREEN");
        } else if (color == 4) {
          display.printf("BLUE");
        } else if (color == 5) {
          display.printf("INDIGO");
        } else if (color == 6) {
          display.printf("VIOLET");
        }
        display.display();

      }
    }

    if (c == 6) {
      hueColor = color;

      intensity = map(pos, 0, 96, 0, 255);

      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Brightness");



      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      if (intensity <= 86) {
        display.printf("LOW");
      }
      else if (intensity > 86 && intensity <= 171 ) {
        display.printf("MEDIUM");
      }
      else if (intensity > 171) {
        display.printf("HIGH");
      }
      display.display();

    }
    if (c == 7) {
      brightNess = intensity;

      if (lightN < 5) {
        if (brightNess <= 86) {
          setHue(lightN,true,HueRainbow[hueColor] , 86);
          getHue(lightN);
        }
        else if (brightNess > 86 && brightNess <= 171 ) {
          setHue(lightN, true,HueRainbow[hueColor] , 171);
          getHue(lightN);
        }
        else if (brightNess > 171) {
          setHue(lightN, true,HueRainbow[hueColor] , 255);
          getHue(lightN);
        }
      }
      else if (lightN == 5) {
        if (brightNess <= 86) {
          for (i = 1; i < 5; i++) {
            setHue(i, true,HueRainbow[hueColor], 86);
            getHue(i);
          }
        }
        else if (brightNess > 86 && brightNess <= 171 ) {
          for (i = 1; i < 5; i++) {
            setHue(i, true,HueRainbow[hueColor], 171);
            getHue(i);
          }
        }
        else if (brightNess > 171) {
          for (i = 1; i < 5; i++) {
            setHue(i, true,HueRainbow[hueColor], 255);
            getHue(i);
          }
        }
      }

      lighT = false;
      modE = true;
      c = 2;
      pos = 0;
      myEnc.write(0);
      display.clearDisplay();
      display.display();
    }
  }
}

void Wemo() {
  if (wemO == true) {
    static int wemoSelect;
    static int onOff;
    static int wemoState;
    static int i;

    pos = myEnc.read();
    if (pos >= 96) {
      pos = 96;
      myEnc.write(96);
    } else if (pos <= 0) {
      pos = 0;
      myEnc.write(0);
    }
    if (c == 3) {
      wemoSelect = map(pos, 0, 96, 0, 4);

      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Selected Wemo");

      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(48, 16);            // Start at top-left corner
      if (wemoSelect < 4) {
        display.printf("%i", wemoSelect);
      }
      else {
        display.printf("All");
      }

      display.display();
    }
    if (c == 4) {
      int On = 1;
      int OfF = 0;
      wemo = wemoSelect;

      onOff = map(pos, 0, 96, 0, 1);

      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Turn Wemo");

      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      if (On == onOff) {
        display.printf("ON");
      }
      else if (OfF == onOff) {
        display.printf("OFF");
      }
      display.display();
    }
    if (c == 5) {
      wemoState = onOff;

      if (wemoState == 0) {
        if (wemo < 4) {
          weMO.switchOFF(wemo);
        }
        else if (wemo == 4) {
          for (i = 0; i <= 3; i++) {
            weMO.switchOFF(i);
          }
        }
      }
      else {
        if (wemo < 4) {
          weMO.switchON(wemo);
        }
        else if (wemo == 4) {
          for (i = 0; i <= 3; i++) {
            weMO.switchON(i);
          }
        }
      }

      wemO = false;
      modE = true;
      c = 2;
      pos = 0;
      myEnc.write(0);
      display.clearDisplay();
      display.display();

    }
  }
}

/*
  void longPressStart1() {
  doubleclick = false;
  buttonState = true;
  stopTime = false;
  timerset = false;
  y = 0;
  mm = 0;
  ss = 0;
  incomingValue = "";
  int oldTime = -1;
  int clockTime = 0;

  if (clockTime != oldTime) {
    Serial.println("Reset");
  }
  }*/


/*void timerSet() {
  unsigned int mod;


  if (doubleclick == true) {
    incomingValue = "";     //Empties the string incomingValue

      Serial.println("Please set timer ");


    while (incomingValue == "") {                     //While the value of the string is empty,
     button1.tick();
      incomingValue = Serial.readStringUntil('\f');   //the Serial buffer will be read for characters
      if (button1.isLongPressed()) {                  //and then insert them into the string
        break;
      }
    }

    i = incomingValue.toInt();     //Parses the character string for integer values and then inserts them into the float i

    mod = i;   //Sets the value of i to the integer mod

    if (i / 60000 < 1) {    //If the time is less than 60 seconds, set the interger m to zero
      m = 0;                //else set m to 1
    }
    else {
      m = i / 60000;
    }

    if (i / 60 > 1) {             //if the time is greater than 1 minute,
      s = (mod / 1000) % 60;      //sets the integer s to the modulo of (mod/1000)/60
    } else if (i / 1000 == 60) {      //if the time is set to a minute e.g. 5:00,
      s = 0;                          //sets s to zero in the case of 5:60
    }
    else {                  //else sets s to i/1000
      s = i / 1000;
    }

    if (s < 10) {                                         //Adds a leading zero to the Serial print
      Serial.printf("Timer is set to %i:0%i\n", m, s );
    } else {                                              //Prints without leading zero
      Serial.printf("Timer is set to %i:%i\n", m, s );
    }

    timerset = !timerset;   //Inverts the timerset boolean
  }
  }
*/

/*
  void timer() {
  static int sM;
  static unsigned int sS;
  static int w = 0;
  static int oldTime = -1;
  static int clockTime = 0;




  if (doubleclick == true) {    //If the doubleclick and timerset booleans are true,
    if (timerset == true) {     //the timer runs

      currentMil = millis();  //Sets currentMil to the internal clock millis()

      if (currentMil - lastMil >= Second) {   //Every second, the timer decrements and prints
        //the remaining time until it is finished
        if (s > 0) {
          s--;
          if (s < 10) {                       //Adds a leading zero
            Serial.printf("%i:0%i\n", m, s);
          }
          else {                             //Prints without the leading zero
            Serial.printf("%i:%i\n", m, s);
          }
        }

        if (s == 0 && m > 0) {    //If s reaches zero and m is still greater than 0,
          s =  60;                //s is set to 60 and m decrements by one
          m--;
        }

        if (m == 0 && s == 0) {               //if both m and s are zero, the timer finishes and prints "Timer is Done"
          if (clockTime != oldTime){          //Prints the string only once
            Serial.println("Timer is Done\n");
            oldTime = clockTime;
          }
        }

      lastMil = currentMil;               //Sets lastMil to currentMil to be used in the 1 second loop
    }
  }
  }
  else {                              //If the bool doubleclick is false, the boolbuttonState is checked
  if (buttonState == false) {       //If buttonState is false, the button has been depressed and the stopwatch starts

      w = -1;       //Sets w to -1
      y = 0;        //Sets y to 0

      if (stopTime == false) {    //Checks if the bool stopTime is true. If false, The stopwatch counts up from the
        //initial value of zero
        currentMil = millis();

        if (currentMil - lastMil >= Second) {   //Runs every second
          if (w < 0) {      //If w is set to less than zero the stopwatch starts. W is set to zero when the Teensy boots.
            ss++;           //The time increments while w is set to 0.
            if (ss < 10) {
              Serial.printf("%i:0%i\n", mm, ss);
            }
            else {
              Serial.printf("%i:%i\n", mm, ss);
            }

            if (ss > 59) {  //If ss is greater than 59, its set to 0 and mm increments
              ss = 0;
              mm++;
            }
          }
          lastMil = currentMil;   //Sets lastMil to currentMil
        }
      }
      if (stopTime == true) {   //If stopTime is true, the stopWatch uses the values
        //sS, and sM to count up from
        currentMil = millis();

        if (currentMil - lastMil >= Second) {
          if (w < 0) {
            sS++;
            if (sS < 10) {
              Serial.printf("%i:0%i\n", sM, sS);
            }
            else {
              Serial.printf("%i:%i\n", sM, sS);
            }

            if (sS > 59) {
              sS = 0;
              sM++;
            }
          }
          lastMil = currentMil;
        }
      }
    }

    if (buttonState == true) {   //If buttonState is true, the stopWatch stops
      w = 0;
      if (stopTime == false && y == 1) {  //If stopTime is false and y == 1(y = 0 before the stopWatch is started),
        stopTime == true;                 //stopTime is set to true and the values sM and sS are set to mm and ss
        sM = mm;                          //to save the time when the stopwatch was paused
        sS = ss;
      }

      if (stopTime == true) {           //If the stopwatch is pasued again, the values sM and sS will be saved
        sM = sM;                          //and used when the stopwatch is started again
        sS = sS;
      }
    }
  }
  }
*/
