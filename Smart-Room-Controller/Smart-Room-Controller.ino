/*
  Project: Smart Room Controller
  Description: Control Hue lights and Wemo outlets with one device
  Author: Lee Abeyta
  Date:7/28/20
*/

#include <Adafruit_NeoPixel.h>
#include "colors.h"
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
//#include <TimeLib.h>
#include <DS1307RTC.h>


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
bool morning = true;
bool evening = false;

float pos;
float tempF;


int thermoTemp = 74;
int mainSelect;
int buttonOut = 22;
int buttonIn = 21;
int buttoN;
int counter = 0;
int wemo;
unsigned int setSleep = 10000;
unsigned int nowMil;
unsigned int sec;
unsigned int oldMil;
unsigned long currentMil;
unsigned long lastMil;
unsigned long Interval = 200;

Adafruit_NeoPixel pixel(14,20, NEO_GRB + NEO_KHZ800);
tmElements_t tm;
Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Encoder myEnc(6, 7);
IPAddress ip(192, 168, 1, 19); // Teensy
WEMO weMO(wemo);


void setup() {            //Starts the Serial monitor
  Serial.begin(9600);
  while (!Serial);

  Ethernet.begin(mac, ip);
  Serial.println("connecting...");

  status = bme.begin(0x76);
  Serial.printf("Initializing BME...\n");

  if (!status) {
    Serial.printf("BME initialization has failed\n");
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  pinMode(buttonOut, OUTPUT);
  pinMode(buttonIn, INPUT_PULLUP);

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
  nowMil = millis();

  if (nowMil - oldMil >= sec) {
    rtc();
    oldMil = nowMil;
  }
  temp();

  digitalWrite(buttonOut, HIGH);
  buttoN = digitalRead(buttonIn);

  mainFunc();

}

void temp() {
  float tempC;
  tempC = bme.readTemperature();
  tempF = map(tempC, -273.15, 1000.0, -459.67, 1832.0);

}

void mainFunc() {

  currentMil = millis();

  if (currentMil - lastMil >= Interval) {
    if (buttoN == HIGH) {
      counter++;
    }

    screen();
    menu();
    settings();
    thermoset();
    sleeptime();
    mode();
    automatic();
    Wakeup();
    goodnight();
    light();
    Wemo();
    thermoStat();
    lastMil = currentMil;
  }
}

void screen() {
  if (screeN == true) {
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner
    if (tm.Hour < 12) {
      if (tm.Hour >= 0 && tm.Hour < 10) {
        if (tm.Minute >= 0 && tm.Minute < 10) {
          display.printf("0%i:0%i AM", tm.Hour, tm.Minute + 1);
        }
        else {
          display.printf("0%i:%i AM", tm.Hour, tm.Minute + 1);
        }
      }
      else {
        display.printf("%i:%i AM", tm.Hour, tm.Minute + 1);
      }
    }
    else if (tm.Hour >= 12) {

      if (tm.Minute >= 0 && tm.Minute < 10) {
        display.printf("%i:0%i PM", tm.Hour - 20, tm.Minute + 1);
      }
      else {
        display.printf("%i:%i PM", tm.Hour - 20, tm.Minute + 1);
      }
    }

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(80, 0);           // Start at top-left corner
    display.printf("%i/%i/%i", tm.Month, tm.Day, tmYearToCalendar(tm.Year));

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
  if (counter == 1) {
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
    if (counter == 1) {
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
    if (counter == 2) {
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
        counter = 0;
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
    if (counter == 2) {
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
    if (counter == 3) {
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
        counter = 1;
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
    static int tempSet;
    if (counter == 3) {
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
    if (counter == 4) {
      thermoTemp = tempSet;
      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Thermostat set to");



      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      display.printf("%i F", thermoTemp);

      display.display();

      delay(1500);
      thermoSet = false;
      settingS = true;
      counter = 2;
      pos = 0;
      myEnc.write(0);
      display.clearDisplay();
      display.display();

    }
  }
}

void sleeptime() {
  if (sleepTime == true) {
    static int timeSet;
    int m;
    int s;
    if (counter == 3) {
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
    if (counter == 4) {
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
      counter = 2;
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
    if (counter == 2) {
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
    if (counter == 3) {
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
        counter = 1;
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
    static int i = 0;

    if (tempF > thermoTemp + 1) {
      weMO.switchON(3);
    }
    else if (tempF <= thermoTemp) {
      weMO.switchOFF(3);
    }

    if (morning == true) {
      unsigned int lastMill = 0;
      unsigned int currentMill;
      int sec = 1000;
      static int light;
      
      weMO.switchON(1);
      weMO.switchON(2);

      currentMill = millis();
      if (currentMill - lastMill >= sec) {


        for (light = 1; light < 5; light++) {
          setHue(light, true, HueRainbow[2], i);
          getHue(light);
        }

        i = i + 16;
        lastMill = currentMill;
      }

      if (i == 256) {
        for (light = 1; light < 5; light++) {
          setHue(light, false, HueRainbow[0], 0);
          getHue(light);
        }
        weMO.switchOFF(1);
        weMO.switchOFF(2);
        morning = false;
        evening = true;
      }

    }

    if (evening == true) {
      unsigned int lastMill;
      unsigned int currentMill;
      int sec = 1000;
      static int light;
      
      weMO.switchON(0);

      currentMill = millis();
      if (currentMill - lastMill >= sec) {


        if (i > 128) {
          for (light = 1; light < 5; light++) {
            setHue(light, true, HueRainbow[1], i);
            getHue(light);
          }
        }
        else {
          for (light = 1; light < 5; light++) {
            setHue(light, true, HueRainbow[0], i);
            getHue(light);
          }
        }

        i = i - 16;
        lastMill = currentMill;
      }

      if (i <= 0) {
        for (light = 1; light < 5; light++) {
          setHue(light, false, HueRainbow[0], 0);
          getHue(light);
        }
        weMO.switchOFF(0);
        evening = false;
        morning = true;
      }
    }
  }
}

void Wakeup() {
  if (bF == true) {
    static unsigned int currentMill;
    static unsigned int lastMill;
    int sec = 1000;
    static int i = 0;
    static int Light;

    currentMill = millis();

    if (counter == 3) {

      weMO.switchON(1);
      weMO.switchON(2);
      while (i < 256 ) {

        currentMill = millis();
        if (currentMill - lastMill >= sec) {

          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE);        // Draw white text
          display.setCursor(34, 12);            // Start at top-left corner
          display.printf("!!WAKE UP!!");

          display.display();

          delay(200);

          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
          display.setCursor(34, 12);            // Start at top-left corner
          display.printf("!!WAKE UP!!");

          display.display();

          delay(200);


          for (Light = 1; Light < 5; Light++) {
            setHue(Light, true, HueRainbow[2], i);
            getHue(Light);
          }

          i = i + 16;
          lastMill = currentMill;
        }
      }

      if (i == 256) {
        for (Light = 1; Light < 5; Light++) {
          setHue(light, false, HueRainbow[0], 0);
          getHue(light);
        }
        weMO.switchOFF(1);
        weMO.switchOFF(2);
        bF = false;
        modE = true;
        counter = 2;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }
    }
  }
}

void goodnight() {
  if (nighT == true) {
    unsigned int currentMill;
    unsigned long lastMill;
    int sec = 1000;
    static int i = 256;
    static int light;


    if (counter == 3) {

      weMO.switchON(0);
      while (i  > 0 ) {

        currentMill = millis();
        if (currentMill - lastMill >= sec) {

          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE);        // Draw white text
          display.setCursor(34, 12);            // Start at top-left corner
          display.printf("~Good Night~");

          display.display();

          delay(200);

          display.clearDisplay();
          display.setTextSize(1);             // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
          display.setCursor(34, 12);            // Start at top-left corner
          display.printf("~Good Night~");

          display.display();

          delay(200);


          for (light = 1; light < 5; light++) {
            setHue(light, true, HueRainbow[2], i);
            getHue(light);
          }

          i = i - 16;
          lastMill = currentMill;
        }
      }

      if (i == 0) {
        for (light = 1; light < 5; light++) {
          setHue(light, false, HueRainbow[0], 0);
          getHue(light);
        }
        weMO.switchOFF(0);
        nighT =  false;
        modE = true;
        counter = 2;
        pos = 0;
        myEnc.write(0);
        display.clearDisplay();
        display.display();
      }
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
    if (counter == 3) {
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
    if (counter == 4) {
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
    if (counter == 5) {
      lightState = onOff;

      if (lightState == 0) {
        if (lightN < 5) {
          setHue(lightN, false, 0, 0);
          getHue(lightN);
          lighT = false;
          modE = true;
          counter = 2;
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
          counter = 2;
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

    if (counter == 6) {
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
    if (counter == 7) {
      brightNess = intensity;

      if (lightN < 5) {
        if (brightNess <= 86) {
          setHue(lightN, true, HueRainbow[hueColor] , 86);
          getHue(lightN);
        }
        else if (brightNess > 86 && brightNess <= 171 ) {
          setHue(lightN, true, HueRainbow[hueColor] , 171);
          getHue(lightN);
        }
        else if (brightNess > 171) {
          setHue(lightN, true, HueRainbow[hueColor] , 255);
          getHue(lightN);
        }
      }
      else if (lightN == 5) {
        if (brightNess <= 86) {
          for (i = 1; i < 5; i++) {
            setHue(i, true, HueRainbow[hueColor], 86);
            getHue(i);
          }
        }
        else if (brightNess > 86 && brightNess <= 171 ) {
          for (i = 1; i < 5; i++) {
            setHue(i, true, HueRainbow[hueColor], 171);
            getHue(i);
          }
        }
        else if (brightNess > 171) {
          for (i = 1; i < 5; i++) {
            setHue(i, true, HueRainbow[hueColor], 255);
            getHue(i);
          }
        }
      }

      lighT = false;
      modE = true;
      counter = 2;
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
    if (counter == 3) {
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
    if (counter == 4) {
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
    if (counter == 5) {
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
      counter = 2;
      pos = 0;
      myEnc.write(0);
      display.clearDisplay();
      display.display();

    }
  }
}

void thermoStat() {
  if (thermO == true) {
    static int thermoset;
    static int setTemp;
    static int p = 0;
    if (counter == 3) {

      pos = myEnc.read();
      if (pos >= 96) {
        pos = 96;
        myEnc.write(96);
      } else if (pos <= 0) {
        pos = 0;
        myEnc.write(0);
      }
      setTemp = map(pos, 0, 96, 60.0, 85.0);

      display.clearDisplay();

      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(16, 0);            // Start at top-left corner
      display.printf("Set Temperature");



      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);  // Draw white text
      display.setCursor(34, 16);            // Start at top-left corner
      display.printf("%i F", setTemp);

      display.display();

    }
    if (counter == 4) {
      thermoset = setTemp;
      if (p == 0) {
        display.clearDisplay();

        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(16, 0);            // Start at top-left corner
        display.printf("Thermostat set to");



        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);  // Draw white text
        display.setCursor(34, 16);            // Start at top-left corner
        display.printf("%i F", thermoset);

        display.display();
        delay(1500);
        p = 1;
      }

      if (tempF > thermoSet + 1) {
        weMO.switchON(3);
      }
      else if (tempF <= thermoSet) {
        weMO.switchOFF(3);
      }
    }
    if (counter == 5) {
      thermO = false;
      modE = true;
      counter = 2;
      pos = 0;
      myEnc.write(0);
      display.clearDisplay();
      display.display();

    }

  }
}

void rtc() {
  RTC.read(tm);

}
