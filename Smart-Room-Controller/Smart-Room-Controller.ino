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
int p = 0;
unsigned int setSleep = 10000;
unsigned int nowMil;
unsigned int sec;
unsigned int oldMil;
unsigned long currentMil;
unsigned long lastMil;
unsigned long Interval = 200;

Adafruit_NeoPixel pixel(14, 20, NEO_GRB + NEO_KHZ800);
tmElements_t tm;
Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Encoder myEnc(6, 7);
IPAddress ip(192, 168, 1, 15); // Teensy
WEMO weMO(wemo);


void setup() {            //Starts the Serial monitor
  Serial.begin(9600);
  while (!Serial);

  Ethernet.begin(mac,ip);       //Starts the Ethernet Jack
  Serial.println("connecting...");

  status = bme.begin(0x76); //Starts the BME280 Senso
  Serial.printf("Initializing BME...\n");

  if (!status) {
    Serial.printf("BME initialization has failed\n");
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //Starts and addresses 0x3C for 128x32 OLED
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  pinMode(buttonOut, OUTPUT);         //Button Read 
  pinMode(buttonIn, INPUT_PULLUP);

  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  
  pixel.begin();  //Starts the NeoPixels
  pixel.show();
}

void loop() {

/************ Reads buton output and sets it to the variable buttoN ********/
  digitalWrite(buttonOut, HIGH);
  buttoN = digitalRead(buttonIn);

/***** Runs the temp() and mainFunc() functions *****/  
  temp();
  mainFunc();

}

/******* Reads the temperature from the BME280 and converts it to Farenheight *******/
void temp() {
  
  float tempC;
  tempC = bme.readTemperature();
  tempF = map(tempC, -273.15, 1000.0, -459.67, 1832.0);

}

/**** Container for the Menu functions ****/
void mainFunc() {

  currentMil = millis();

/**** Every 200 ms the State of each function is checked to determine if they will output****/
  if (currentMil - lastMil >= Interval) {

/**** Lights the NeoPixel Green if Automatic mode is on ****/
/**** and Red if Automatic mode is off ****/
    if(autO){
      pixel.clear();
      pixel.setBrightness(16);
      pixel.fill(green,12,1);
      pixel.show();
    }
    else{
       pixel.clear();
      pixel.setBrightness(16);
      pixel.fill(red,12,1);
      pixel.show();
    }
    
/**** If the button is pressed, The counter increments by 1 ****/
/**** The counter keeps track of which menu screen should be displayed ****/
    if (buttoN == HIGH) {
      counter++;
    }
    
    rtc();
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

/**** Displays main screen on start up ****/
void screen() {

/**** Screen is initislly true so that it runs first ****/  
  if (screeN == true) {
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner

/**** Reads the time from the DS1307RTC and displays it on the OLED ****/
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
        display.printf("%i:0%i PM", tm.Hour, tm.Minute + 1);
      }
      else {
        display.printf("%i:%i PM", tm.Hour, tm.Minute + 1);
      }
    }

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(80, 0);           // Start at top-left corner
    display.printf("%i/%i/%i", tm.Month, tm.Day, tmYearToCalendar(tm.Year));

/**** Displays the current Temperature ****/
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(44, 12);            // Start at top-left corner
    display.printf("%0.2f F", tempF);

/**** Displays the current Mode ****/
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

/**** If the button is pressed, the counter increments ****/
/**** and the current function is set to false, Automatic mode ****/
/**** is set to false and the Main Menu is set to true ****/  
  if (counter == 1) {
    autO = false;
    screeN = false;
    menU = true;
    display.clearDisplay();
    display.display();
  }
}

/**** Displays the Main Menu selections ****/
void menu() {

/**** If the button is pressed once, menU is set to true ****/
/****  and screen is set to false so that the menu function ****/
/**** can initialize. ****/
  if (menU == true) {

/**** Assigns values to the different selections ****/
/**** and maps the encoder to the selections ****/ 
    int Mode = 0;
    int Settings = 1;
    int Back = 2;

/**** If the button has been pressed once, ****/
/**** run proceeding code ****/
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

/**** If the encoder's position is equal to the value of a selection, ****/
/**** the selection is highlighted on the OLED indicating it is selected ***/      
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

/**** If the button is pressed twice and ****/
/**** one of the selections is higlighted, ****/
/**** run the proceeding code. ****/
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

/**** If back is selected, the OLED displays the previous ****/
/**** screen and resets the values to their previous state ****/
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

/**** Displays the Settings menu ****/
void settings() {

/**** If the button is pressed twice and Settings is selected, ****/
/**** settings is set to true  and menU is set to false ****/
/**** so that the settings function can initialize. ****/
  if (settingS == true) {

/**** Assigns values to the different selections ****/
/**** and maps the encoder to the selections ****/ 
    int ThermoSet = 0;
    int SleepTime = 1;
    int Back = 2;
    
/**** If the button has been pressed twice, ****/
/**** run proceeding code ****/
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

/**** If the encoder's position is equal to the value of a selection, ****/
/**** the selection is highlighted on the OLED indicating it is selected ***/     
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
    
/**** If the button is pressed three times and ****/
/**** one of the selections is higlighted, ****/
/**** run the proceeding code. ****/
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

/**** If back is selected, the OLED displays the Main Menu ****/
/**** screen and resets the values to their previous state ****/      
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

/**** Displays the Thermostat Set Screen ****/
void thermoset() {


/**** If the button is pressed three times and thermoset is selected, ****/
/**** thermoSet is set to true  and settings is set to false so that  ****/
/**** the thermoset function can initialize. ****/
  if (thermoSet == true) {
    
/**** Assigns values to the different selections ****/
/**** and maps the encoder to the selections ****/ 
    static int tempSet;

/**** If the buttons has been pressed three times, ****/
/**** runt the proceeding code. ****/    
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

/**** The Encoder is set to a range between 60 degrees and ****/
/**** 85 degrees. ****/
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

/**** Once the desired temperature is set by clicking the button, ****/
/**** the selected temperature is displayed for a moment before the ****/
/**** OLED returns to the previous menu. ****/
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

/**** Displays the Sleep time set menu ****/
void sleeptime() {

/**** If the button is pressed three times and sleeptime is selected, ****/
/**** sleepTime is set to true and settings is set to false so that  ****/
/**** the sleeptime function can initialize. ****/  
  if (sleepTime == true) {

/**** Assigns values to the different selections ****/
/**** and maps the encoder to the selections ****/ 
    static int timeSet;
    int m;
    int s;

/**** If the button has been pressed three times, ****/
/**** run the poceeding code. ****/
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

/**** The Encoder is set to a range between 0degrees and ****/
/**** 10 minutes. ****/
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

/**** Once the desired Sleep time is set by clicking the button, ****/
/**** the selected temperature is displayed for a moment before the ****/
/**** OLED returns to the previous menu. ****/
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
        display.printf("%i:0%i", m, s); //Adds a leading zero
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

/**** Displays the Mode menu ****/
void mode() {

/**** If the button is pressed twice and mode is selected, ****/
/**** modE is set to true and menU is set to false so that  ****/
/**** the mode function can initialize. ****/    
  if (modE == true) {

/**** Assigns values to the different selections ****/
/**** and maps the encoder to the selections ****/ 
    int Auto = 0;
    int BF = 1;
    int Night = 2;
    int Light = 3;
    int Wemo = 4;
    int Thermo = 5;
    int Back = 6;

/**** If the button has been pressed twice, ****/
/**** run the proceeding code. ****/
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

/**** If the encoder's position is equal to the value of a selection, ****/
/**** the selection is highlighted on the OLED indicating it is selected ***/   
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


/**** If the button is pressed three times and ****/
/**** one of the mmodes is higlighted, ****/
/**** run the proceeding code. ****/    
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


/**** If back is selected, the OLED displays the Main Menu ****/
/**** screen and resets the values to their previous state ****/       
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

/**** Sets autO to true, runs the automatic function ****/
/**** and retirns the OLED to the main screen. ****/
void automatic() {

/**** autO is intially true, and is set ****/
/**** to false when the button is pressed. ****/
/**** autO can be re-initialized from the Mode menu. ****/
  if (autO == true) {
    screeN = true;
    static int i = 0;

/**** Use the value of thermoTemp which is set in the settings menu ****/
/**** to determine whether the fan should turn on. ****/
    if (tempF > thermoTemp + 1) {
      weMO.switchON(3);
    }
    else if (tempF <= thermoTemp) {
      weMO.switchOFF(3);
    }

/**** morning is intially true and auto is set to true ****/
/**** on startup. morning runs a quick morning routine that  ****/
/**** Brightens the lights and turns on the breakfast appliances. ****/
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

/**** If the button is pressed, the cycle breaks ****/
/**** and the OLED displays the Main Menu, setting ****/
/**** autO and screeN to false while menU is set to true ****/
        if(counter == 4){
          autO = false;
          screeN = false;
          menU = true;
          counter = 1;
          pos = 0;
          myEnc.write(0);
          display.clearDisplay();
          display.display();
        }
      }
/**** once the sequence has run 16 times, ****/
/**** the lights and appliances shut off ****/
/**** and evening is set to true while morning ****/
/**** is set to false. ****/
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

/**** evening runs a quick evening routine that  ****/
/**** dims the lights and turns on the fan and lava lamp. ****/
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

/**** If the button is pressed, the cycle breaks ****/
/**** and the OLED displays the Main Menu, setting ****/
/**** autO and screeN to false while menU is set to true ****/        
        if(counter == 4){
          autO = false;
          screeN = false;
          menU = true;
          counter = 1;
          pos = 0;
          myEnc.write(0);
          display.clearDisplay();
          display.display();
        }
      }

/**** once the sequence has run 16 times, ****/
/**** the lights and devices shut off ****/
/**** and morning is set to true while evening ****/
/**** is set to false. ****/
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

/**** Runs the Wakeup mode ****/
void Wakeup() {


/**** If the button is pressed three times and wakeup is selected, ****/
/**** bF is set to true and modE is set to false so that  ****/
/**** the Wakeup function can initialize. ****/    
  if (bF == true) {
    static unsigned int currentMill;
    static unsigned int lastMill;
    int sec = 1000;
    static int i = 0;
    static int Light;

    currentMill = millis();

/**** If the buttons has been pressed three times, ****/
/**** run the proceeding code. ****/
    if (counter == 3) {

/**** Wakeup runs a quick morning routine that  ****/
/**** Brightens the lights and turns on the breakfast appliances. ****/
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

/**** once the sequence has run 16 times, ****/
/**** the lights and appliances shut off ****/
/**** and modE is set to true while bF ****/
/**** is set to false. The OLED returns to the ****/
/**** Mode menu. ****/
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

/**** Runs the goodnight function ****/
void goodnight() {

/**** If the button is pressed three times and goodnight is selected, ****/
/**** nighT is set to true and modE is set to false so that  ****/
/**** the goodnight function can initialize. ****/      
  if (nighT == true) {
    unsigned int currentMill;
    unsigned long lastMill;
    int sec = 1000;
    static int i = 256;
    static int light;

/**** If the buttons has been pressed three times, ****/
/**** run the proceeding code. ****/
    if (counter == 3) {

/**** goodnight runs a quick evening routine that  ****/
/**** dims the lights and turns on the fan and lava lamp. ****/
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

/**** once the sequence has run 16 times, ****/
/**** the lights and devices shut off ****/
/**** and modE is set to true while nighT ****/
/**** is set to false.The OLED returns to the ****/
/**** Mode menu. ****/
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

/**** Displays the light menu ****/
void light() {

/**** If the button is pressed three times and light is selected, ****/
/**** lighT is set to true and modE is set to false so that  ****/
/**** the light function can initialize. ****/     
  if (lighT == true) {

/**** Defines the local variables ****/
    static int onOff;
    static int lightState;
    static int lightSelect;
    static int lightN;
    static int color;
    static int i;
    static int intensity;
    static int hueColor;
    static int brightNess;

/**** Maps the encoder to the number of lights ****/ 
    pos = myEnc.read();
    if (pos >= 96) {
      pos = 96;
      myEnc.write(96);
    } else if (pos <= 0) {
      pos = 0;
      myEnc.write(0);
    }

/**** If the button has been pressed three times, ****/
/**** run the proceeding code. ****/
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

/**** Once the desired light(s) is set by clicking the button, ****/
/**** the encoder is re-mapped to 0 - 1 so that the light can be ****/
/**** turned on or off ****/
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

/**** Once the desired light state has been selected, ****/
/**** the device checks it's status. If false, the selected light(s) ***/
/**** are turned of and the OLED returns to the Mode menu. If ****/
/**** true, the encoder is re-mapped to 0-6. ***/
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

/**** Once the desired color has been selected, ****/
/**** the encodeer is re-mapped to 0-255, ****/
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

/**** Once the desired brightness has been selected, ****/
/**** the selected lights turn on with the chosen color ****/
/**** and intesity. The OLED then returns to the Mode menu. ****/
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

/**** Displays the Wemo menu ****/
void Wemo() {

/**** If the button is pressed three times and Wemo is selected, ****/
/**** wemO is set to true and modE is set to false so that  ****/
/**** the Wemo function can initialize. ****/     
  if (wemO == true) {

/**** Defines the local variables ****/
    static int wemoSelect;
    static int onOff;
    static int wemoState;
    static int i;

/**** Maps the encoder to 0-4 (the number of Wemo outlets) ****/
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

/**** When the selected Wemo(s) is selected, ****/
/**** the encoder is re-mapped to 0-1 to select the ****/
/**** wemo state. ****/
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

/**** Once the Wemo state has been set, ****/
/**** the Wemo will either turn on or off ***/
/**** and the OLED will return to the Mode menu. ****/
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

/**** Displays the thermostat menu ****/
void thermoStat() {

/**** If the button is pressed three times and thermostat is selected, ****/
/**** thermO is set to true and modE is set to false so that  ****/
/**** the thermoStat function can initialize. ****/     
  if (thermO == true) {

/**** Defines the local variables ****/
    static int thermoset;
    static int setTemp;

/**** If the button has been pressed three times, ****/
/**** run the proceeding code. ****/
    if (counter == 3) {

/**** Maps the encoder to a range of 60 degrees ****/
/**** to 80 degrees. ****/
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

/**** Once the temperature has been set, ****/
/**** the OLED will display the selected ****/
/**** temperature and then attempt to keep ****/
/**** the room within a 1 degree range of ****/
/**** set temperature. ****/
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

/**** If the button is pressed, ****/
/**** it will stop the function ****/
/**** from running and return the ****/
/**** OLED to the Mode Menu. ****/
    if (counter == 5) {
      thermO = false;
      modE = true;
      counter = 2;
      p = 0;
      pos = 0;
      myEnc.write(0);
      display.clearDisplay();
      display.display();
    }
  }
}

/**** Initializes the RTC object ****/
void rtc() {
  RTC.read(tm);
}
