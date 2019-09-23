/*Project: Development of an NFC-based Attendance and Location
               Logging System Using an Online Scheduling
                    Database for PSHS-MC Batch 2018
  Mechanic: Juan Sergio A. Buenviaje
  System Programmer: Juan Sergio A. Buenviaje
  Database Programmer: Joshua Kyle C. Kim
  Statistical Analyst: Francis Anton S. Garcia*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

//#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Color definitions
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define PINK        0xF81F

/******************* UI details */
int BUTTON_X = 39;
int BUTTON_Y = 280;
int BUTTON_W = 77;
int BUTTON_H = 50;
int BUTTON_SPACING_X = 3;
int BUTTON_SPACING_Y = 20;
int BUTTON_TEXTSIZE = 2;

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int stationNumber = 1;
char buttonlabels[3][6] = {"Setup", "Scan", "End"};
uint16_t buttoncolors[3] = {NAVY, DARKGREEN, MAROON};
char scanLabels[1][5] = {"Back"};
uint16_t scanColors[1] = {RED};
char endLabels[2][4] = {"Yes", "No"};
uint16_t endColors[2] = {GREEN, RED};
char settingsLabels[9][5] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
uint16_t settingsColors[9] = {DARKGREY, DARKGREY, DARKGREY,
                              DARKGREY, DARKGREY, DARKGREY,
                              DARKGREY, DARKGREY, DARKGREY};
/*char namesF[5] = {" Juan Sergio", " Joshua Kyle", " Francis Anton", 
                         " Hans", " Patrick Angelo"};
char namesL[][5] = {" A. Buenviaje", " C. Kim", " S. Garcia", 
                         " T. Mingala", " G. Adducul"};
char headersT[][9] = {"Scanning", "Card", "Access", "Start scan", "Turn off?",
                         "Settings", "Granted", "Denied", "Authenticate"};   
char headersM[][9] = {" Student number:", " Name:", " Class:", " Attendance:", 
                         " Status:", " Please wait...", " Timeout in:", 
                         " Scan authorized", " card in:"};
char number[][5] = {" 12-83355", " 12-61685", " 12-63440", 
                         " 12-13413", " 12-12501"};  
char classes[][5] = {" Social Science 5.C", " Filipino 5.C", " English 5.D",
                         " Physics 3.B", " Technology 3.A"};                                                                      
char attendance[4][] = {" On Time", " Late", " Wrong Class",
                         " Not Applicable"};
char check[2][] = {" Checking In", " Checking Out"};*/                       

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (37)
#define PN532_MOSI (35)
#define PN532_SS   (33)
#define PN532_MISO (31)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (37)
#define PN532_RESET (35)  // Not connected by default on the NFC Shield

// Use this line for a breakout with a software SPI connection (recommended):
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
   #define Serial SerialUSB
#endif

void setup(void) {
  Serial.begin(115200);
  Serial1.begin(115200);

  tft.reset();

  uint16_t identifier = tft.readID();
  if(identifier==0x0101)
      identifier=0x9341;

  tft.begin(identifier);
  tft.setCursor(0, 25);

 #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif

  nfc.begin();
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  mainPage();
}

#define MINPRESSURE 5
#define MAXPRESSURE 1000

uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uid1[] = { 0x0B, 0x8F, 0xEA, 0x29 }; 
uint8_t uid2[] = { 0x5B, 0x8E, 0xE1, 0x29 };
uint8_t uid3[] = { 0x64, 0x71, 0x8B, 0xAB };
uint8_t uid4[] = { 0x0B, 0x18, 0xE9, 0x29 };
uint8_t uid5[] = { 0xDB, 0x8D, 0xEA, 0x29 };
uint8_t uidLength; 
  
int t = 0;
int s = 0;
int j = 0;
int k = 0;
int l = 0;
int m = 0;
int n = 0;
int incomingByte = 0;

void loop(void) {
    // Not in use due to repeating nature
}

void mainPage () {
    Adafruit_GFX_Button buttons[3];
    int i = 0;
    BUTTON_X = 39;
    BUTTON_Y = 280;
    BUTTON_W = 77;
    BUTTON_H = 50;
    BUTTON_SPACING_X = 3;
    BUTTON_SPACING_Y = 20;
    BUTTON_TEXTSIZE = 2;
        
    tft.fillScreen(BLACK);
    for (uint8_t row=0; row<1; row++) {
      for (uint8_t col=0; col<3; col++) {
        buttons[col + row*3].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                  BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                    BUTTON_W, BUTTON_H, WHITE, buttoncolors[col+row*3], WHITE,
                    buttonlabels[col + row*3], BUTTON_TEXTSIZE); 
        buttons[col + row*3].drawButton();
      }
    }

    tft.setTextColor(YELLOW);    
    tft.setTextSize(3);
    tft.setCursor(60, 25);
    tft.println("PSHS-MC");
    tft.setCursor(60, 49);
    tft.println("Scanner");
    tft.println();
    tft.setTextColor(WHITE);    
    tft.setTextSize(2);
    tft.println(" Room: SHB 311");
    tft.println();
    tft.println(" Time: 8:45am");
    tft.println();
    tft.println(" Class: Math 5");
    tft.println();
    tft.println(" Status:");
    tft.println(" Ready to Scan");
    tft.println();

    do {
      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);

      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
    
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      // scale from 0->1023 to tft.width
        p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
        p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
      }

      for (uint8_t b=0; b<3; b++) {
        if (buttons[b].contains(p.x, p.y)) {
          buttons[b].press(true);  // tell the button it is pressed
        } else {
          buttons[b].press(false);  // tell the button it is NOT pressed
        }
      }
  
      for (uint8_t b=0; b<3; b++) {
        if (buttons[b].justReleased()) {
          buttons[b].drawButton();  // draw normal
        }
    
          if (buttons[b].justPressed()) {
              buttons[b].drawButton(true);
              if (b == 0) {
                lockPage();
                i++;
              } else if (b == 1) {
                scanPage();
                i++;
              } else {
                endPage();
                i++;
              }
              delay(100);
          }
        }
      } while (i == 0);
}

void scanCard () {              
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    if (uid[1] == uid1[1]) {
          j++;
          tft.fillScreen(BLACK);
          tft.setCursor(50, 25);
          tft.setTextColor(YELLOW);    
          tft.setTextSize(3);
          tft.println("Scanning");
          tft.setCursor(80, 49);
          tft.println("Card");
          tft.println();
          tft.setTextSize(2);
          tft.println(" Please wait...");
          delay(1500);
          tft.fillScreen(BLACK);
          tft.setCursor(0, 25);
          tft.setTextColor(WHITE);    
          tft.setTextSize(2);
          tft.println(F(" Student Number:"));
          tft.println(F(" 12-83355")); 
          tft.println();
          tft.println(F(" Name:"));
          tft.println(F(" Juan Sergio")); 
          tft.println(F(" A. Buenviaje"));
          nfc.PrintHex(uid, uidLength);
          char ID[8]; 
          sprintf(ID, "%0.2x%0.2x%0.2x%0.2x", uid[0], uid[1], uid[2], uid[3]);
          Serial.println(ID);
          Serial1.print(ID);
          Serial1.print('\n');
          tft.println();
          tft.println(F(" Class:"));
          tft.println(F(" Social Science 5.C")); 
          tft.println();
          tft.println(F(" Attendance:"));
          tft.println(F(" On time"));
          tft.println();
          tft.println(F(" Status:"));
          if (j % 2 == 0) {
            tft.println(F(" Checking Out"));
          } else {
            tft.println(F(" Checking In"));
          }
          tft.println();
          delay(3500);
          mainPage();
    } else if (uid[1] == uid2[1]) {
          k++;
          tft.fillScreen(BLACK);
          tft.setCursor(50, 25);
          tft.setTextColor(YELLOW);    
          tft.setTextSize(3);
          tft.println(F("Scanning"));
          tft.setCursor(80, 49);
          tft.println(F("Card"));
          tft.println();
          tft.setTextSize(2);
          tft.println(F(" Please wait..."));
          delay(1500);
          tft.fillScreen(BLACK);
          tft.setCursor(0, 25);
          tft.setTextColor(WHITE);    
          tft.setTextSize(2);
          tft.println(F(" Student Number:"));
          tft.println(F(" 12-61685")); 
          tft.println();
          tft.println(F(" Name:"));
          tft.println(F(" Joshua Kyle")); 
          tft.println(F(" C. Kim"));
          nfc.PrintHex(uid, uidLength);
          char ID1[8]; 
          sprintf(ID1, "%0.2x%0.2x%0.2x%0.2x", uid[0], uid[1], uid[2], uid[3]);
          Serial.println(ID1);
          Serial1.print(ID1);
          Serial1.print('\n');
          tft.println();
          tft.println(F(" Class:"));
          tft.println(F(" Filipino 5.C")); 
          tft.println();
          tft.println(F(" Attendance:"));
          tft.println(F(" Late"));
          tft.println();
          tft.println(F(" Status:"));
          if (k % 2 == 0) {
            tft.println(F(" Checking Out"));
          } else {
            tft.println(F(" Checking In"));
          }
          tft.println();
          delay(3500);
          mainPage();
    } else if (uid[1] == uid3[1]) {
          l++;
          tft.fillScreen(BLACK);
          tft.setCursor(50, 25);
          tft.setTextColor(YELLOW);    
          tft.setTextSize(3);
          tft.println(F("Scanning"));
          tft.setCursor(80, 49);
          tft.println(F("Card"));
          tft.println();
          tft.setTextSize(2);
          tft.println(F(" Please wait..."));
          delay(1500);
          tft.fillScreen(BLACK);
          tft.setCursor(0, 25);
          tft.setTextColor(WHITE);    
          tft.setTextSize(2);
          tft.println(F(" Student Number:"));
          tft.println(F(" 12-63440")); 
          tft.println();
          tft.println(F(" Name:"));
          tft.println(F(" Francis Anton")); 
          tft.println(F(" S. Garcia"));
          nfc.PrintHex(uid, uidLength);
          char ID2[8]; 
          sprintf(ID2, "%0.2x%0.2x%0.2x%0.2x", uid[0], uid[1], uid[2], uid[3]);
          Serial.println(ID2);
          Serial1.print(ID2);
          Serial1.print('\n');
          tft.println();
          tft.println(F(" Class:"));
          tft.println(F(" English 5.D")); 
          tft.println();
          tft.println(F(" Attendance:"));
          tft.println(F(" On time"));
          tft.println();
          tft.println(F(" Status:"));
          if (l % 2 == 0) {
            tft.println(F(" Checking Out"));
          } else {
            tft.println(F(" Checking In"));
          }
          tft.println();
          delay(3500);
          mainPage();
    } else if (uid[1] == uid4[1]) {
          m++;
          tft.fillScreen(BLACK);
          tft.setCursor(50, 25);
          tft.setTextColor(YELLOW);    
          tft.setTextSize(3);
          tft.println(F("Scanning"));
          tft.setCursor(80, 49);
          tft.println(F("Card"));
          tft.println();
          tft.setTextSize(2);
          tft.println(F(" Please wait..."));
          delay(1500);
          tft.fillScreen(BLACK);
          tft.setCursor(0, 25);
          tft.setTextColor(WHITE);    
          tft.setTextSize(2);
          tft.println(F(" Student Number:"));
          tft.println(F(" 12-13413")); 
          tft.println();
          tft.println(F(" Name:"));
          tft.println(F(" Hans"));
          tft.println(F(" T. Mingala"));
          nfc.PrintHex(uid, uidLength);
          char ID3[8]; 
          sprintf(ID3, "%0.2x%0.2x%0.2x%0.2x", uid[0], uid[1], uid[2], uid[3]);
          Serial.println(ID3);
          Serial1.print(ID3);
          Serial1.print('\n');
          tft.println();
          tft.println(F(" Class:"));
          tft.println(F(" Physics 3.B")); 
          tft.println();
          tft.println(F(" Attendance:"));
          tft.println(F(" Wrong class"));
          tft.println();
          tft.println(F(" Status:"));
          if (m % 2 == 0) {
            tft.println(F(" Checking Out"));
          } else {
            tft.println(F(" Checking In"));
          }
          tft.println();
          delay(3500);
          mainPage();
    } else if (uid[1] == uid5[1]) {
          n++;
          tft.fillScreen(BLACK);
          tft.setCursor(50, 25);
          tft.setTextColor(YELLOW);    
          tft.setTextSize(3);
          tft.println(F("Scanning"));
          tft.setCursor(80, 49);
          tft.println(F("Card"));
          tft.println();
          tft.setTextSize(2);
          tft.println(F(" Please wait..."));
          delay(1500);
          tft.fillScreen(BLACK);
          tft.setCursor(0, 25);
          tft.setTextColor(WHITE);    
          tft.setTextSize(2);
          tft.println(F(" Student Number:"));
          tft.println(F(" 12-12501")); 
          tft.println();
          tft.println(F(" Name:"));
          tft.println(F(" Patrick Angelo")); 
          tft.println(F(" G. Adducul"));
          nfc.PrintHex(uid, uidLength);
          char ID4[8]; 
          sprintf(ID4, "%0.2x%0.2x%0.2x%0.2x", uid[0], uid[1], uid[2], uid[3]);
          Serial.println(ID4);
          Serial1.print(ID4);
          Serial1.print('\n');
          tft.println();
          tft.println(F(" Class:"));
          tft.println(F(" Technology 3.A")); 
          tft.println();
          tft.println(F(" Attendance:"));
          tft.println(F(" On time"));
          tft.println();
          tft.println(F(" Status:"));
          if (n % 2 == 0) {
            tft.println(F(" Checking Out"));
          } else {
            tft.println(F(" Checking In"));
          }
          tft.println();
          delay(3500);
          mainPage();
    }
  }

  t++; 
}

void scanPage () {
        t = 0;
        tft.fillScreen(DARKGREEN);
        tft.setCursor(30, 25);
        tft.setTextColor(WHITE);    
        tft.setTextSize(3);
        tft.println(F("Start scan"));
        tft.println();
        
        do {
          scanCard();
          switch (t) {
            case 1:
              tft.setTextSize(2);
              tft.println(F(" Timeout in:"));
              tft.println();
              tft.setCursor(65, 120);
              tft.setTextSize(20);
              tft.println(F("5"));
              delay(50);
            break;
            case 2:
              tft.fillScreen(DARKGREEN);
              tft.setCursor(30, 25);   
              tft.setTextSize(3);
              tft.println(F("Start scan"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Timeout in:"));
              tft.println();
              tft.setCursor(65, 120);
              tft.setTextSize(20);
              tft.println(F("4"));
            break;
            case 3:
              tft.fillScreen(DARKGREEN);
              tft.setCursor(30, 25);   
              tft.setTextSize(3);
              tft.println(F("Start scan"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Timeout in:"));
              tft.println();
              tft.setCursor(65, 120);
              tft.setTextSize(20);
              tft.println(F("3"));
            break;
            case 4:
              tft.fillScreen(DARKGREEN);
              tft.setCursor(30, 25);  
              tft.setTextSize(3);
              tft.println(F("Start scan"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Timeout in:"));
              tft.println();
              tft.setCursor(65, 120);
              tft.setTextSize(20);
              tft.println(F("2"));
            break;
            case 5:
              tft.fillScreen(DARKGREEN);
              tft.setCursor(30, 25);   
              tft.setTextSize(3);
              tft.println(F("Start scan"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Timeout in:"));
              tft.println();
              tft.setCursor(65, 120);
              tft.setTextSize(20);
              tft.println(F("1"));
            break;
            case 6:
              mainPage();
            break;
          }
        } while (t < 6);      
}

void endPage () {
        Adafruit_GFX_Button buttons[2];
        int i = 0;
        BUTTON_X = 120;
        BUTTON_Y = 220;
        BUTTON_W = 220;
        BUTTON_H = 50;
        BUTTON_SPACING_X = 7;
        BUTTON_SPACING_Y = 10;
        BUTTON_TEXTSIZE = 2;

        tft.fillScreen(MAROON);
        tft.setCursor(40, 25);
        tft.setTextColor(WHITE);    
        tft.setTextSize(3);
        tft.println(F("Turn off?"));
        
        for (uint8_t eCol=0; eCol<1; eCol++) {
          for (uint8_t eRow=0; eRow<2; eRow++) {
             buttons[eCol*2 + eRow].initButton(&tft, BUTTON_X+eCol*(BUTTON_W+BUTTON_SPACING_X), 
             BUTTON_Y+eRow*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
             BUTTON_W, BUTTON_H, WHITE, endColors[eCol*2+eRow], WHITE,
             endLabels[eCol*2 + eRow], BUTTON_TEXTSIZE); 
             buttons[eCol*2 + eRow].drawButton();
          }
        }

        do {
          digitalWrite(13, HIGH);
          TSPoint p = ts.getPoint();
          digitalWrite(13, LOW);

          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          
          if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
            // scale from 0->1023 to tft.width
            p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
            p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
          }

          for (uint8_t e=0; e<2; e++) {
            if (buttons[e].contains(p.x, p.y)) {
             buttons[e].press(true);  // tell the button it is pressed
            } else {
             buttons[e].press(false);  // tell the button it is NOT pressed
            }
          }
  
         for (uint8_t e=0; e<2; e++) {
            if (buttons[e].justReleased()) {
           buttons[e].drawButton();  // draw normal
           }
    
           if (buttons[e].justPressed()) {
              buttons[e].drawButton(true);
              if (e == 0) {
                offPage();
                i++;
              } else {
                mainPage();
                i++;
              }
              delay(100);
            }
          }
        } while (i == 0);
}

void settingsPage () {
        Adafruit_GFX_Button buttons[9];
        int i = 0;
        BUTTON_X = 39;
        BUTTON_Y = 100;
        BUTTON_W = 77;
        BUTTON_H = 50;
        BUTTON_SPACING_X = 3;
        BUTTON_SPACING_Y = 20;
        BUTTON_TEXTSIZE = 2;

        tft.fillScreen(NAVY);
        tft.setCursor(50, 25);
        tft.setTextColor(WHITE);    
        tft.setTextSize(3);
        tft.println(F("Settings"));
        
        for (uint8_t sRow=0; sRow<3; sRow++) {
          for (uint8_t sCol=0; sCol<3; sCol++) {
             buttons[sCol + sRow*3].initButton(&tft, BUTTON_X+sCol*(BUTTON_W+BUTTON_SPACING_X), 
             BUTTON_Y+sRow*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
             BUTTON_W, BUTTON_H, WHITE, settingsColors[sCol+sRow*3], WHITE,
             settingsLabels[sCol + sRow*3], BUTTON_TEXTSIZE); 
             buttons[sCol + sRow*3].drawButton();
          }
        }

        do {
          digitalWrite(13, HIGH);
          TSPoint p = ts.getPoint();
          digitalWrite(13, LOW);

          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
        
          if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
            // scale from 0->1023 to tft.width
            p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
            p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
          }

          for (uint8_t s=0; s<9; s++) {
              if (buttons[s].contains(p.x, p.y)) {
              buttons[s].press(true);  // tell the button it is pressed
            } else {
              buttons[s].press(false);  // tell the button it is NOT pressed
            }
          }
  
          for (uint8_t s=0; s<9; s++) {
            if (buttons[s].justReleased()) {
            buttons[s].drawButton();  // draw normal
            }
    
            if (buttons[s].justPressed()) {
              buttons[s].drawButton(true);
              switch (s) {
                case 0:
                  tft.fillScreen(CYAN);
                  delay(3000);
                  mainPage();
                  i++;
                break;
                case 1:
                  tft.fillScreen(PURPLE);
                  delay(3000);
                  mainPage();
                  i++;
                break;
                case 2:
                  tft.fillScreen(OLIVE);
                  delay(3000);
                  mainPage();
                  i++;
                break;
                case 3:
                  tft.fillScreen(MAGENTA);
                  delay(3000);
                  mainPage();
                  i++;
                break;
                case 4:
                  tft.fillScreen(YELLOW);
                  delay(3000);
                  mainPage();
                  i++;
                break;
                case 5:
                  tft.fillScreen(ORANGE);
                  delay(3000);
                  mainPage();
                  i++;
                break;
                case 6:
                  tft.fillScreen(PINK);
                  delay(3000);
                  mainPage();
                  i++;
                break;
                case 7:
                  tft.fillScreen(GREENYELLOW);
                  delay(3000);
                  mainPage();
                  i++;
                break;
                case 8:
                  tft.fillScreen(WHITE);
                  delay(3000);
                  mainPage();
                  i++;
                break;
              }
              delay(100);
            }
          }
        } while(i == 0);
}

void offPage() {
      tft.fillScreen(BLACK); // "turns off" display
}

void secureCard(){
      success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
      if (success) {
        if (uid[1] == uid1[1]) {
              tft.fillScreen(BLACK);
              tft.setCursor(50, 25);
              tft.setTextColor(YELLOW);    
              tft.setTextSize(3);
              tft.println(F("Scanning"));
              tft.setCursor(80, 49);
              tft.println(F("Card"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Please wait..."));
              delay(1500);
              tft.fillScreen(BLACK);
              tft.setCursor(60, 25);
              tft.setTextColor(GREEN);    
              tft.setTextSize(3);
              tft.println(F("Access"));
              tft.setCursor(60, 49); 
              tft.println(F("Granted"));  
              tft.println(); 
              tft.setTextColor(WHITE);  
              tft.setTextSize(2);
              tft.println(F(" Name:"));
              tft.println(F(" Juan Sergio")); 
              tft.println(F(" A. Buenviaje"));
              tft.println();
              tft.println(F(" Status:"));
              tft.println(F(" Student Admin"));
              tft.println();
              tft.println(F(" Clearance Level: 5"));
              delay(3500);
              settingsPage();
        } else if (uid[1] == uid2[1]) {
              tft.fillScreen(BLACK);
              tft.setCursor(50, 25);
              tft.setTextColor(YELLOW);    
              tft.setTextSize(3);
              tft.println(F("Scanning"));
              tft.setCursor(80, 49);
              tft.println(F("Card"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Please wait..."));
              delay(1500);
              tft.fillScreen(BLACK);
              tft.setCursor(60, 25);
              tft.setTextColor(GREEN);    
              tft.setTextSize(3); 
              tft.println(F("Access"));
              tft.setCursor(60, 49); 
              tft.println(F("Granted"));  
              tft.println();   
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.println(F(" Name:"));
              tft.println(F(" Joshua Kyle")); 
              tft.println(F(" C. Kim"));
              tft.println();
              tft.println(F(" Status:"));
              tft.println(F(" Student Admin"));
              tft.println();
              tft.println(F(" Clearance Level: 5"));
              delay(3500);
              settingsPage();
        } else if (uid[1] == uid3[1]) {
              tft.fillScreen(BLACK);
              tft.setCursor(50, 25);
              tft.setTextColor(YELLOW);    
              tft.setTextSize(3);
              tft.println(F("Scanning"));
              tft.setCursor(80, 49);
              tft.println(F("Card"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Please wait..."));
              delay(1500);
              tft.fillScreen(BLACK);
              tft.setCursor(60, 25);
              tft.setTextColor(RED);    
              tft.setTextSize(3);
              tft.println(F("Access"));
              tft.setCursor(60, 49); 
              tft.println(F("Denied"));   
              tft.println();   
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.println(F(" Name:"));
              tft.println(F(" Francis Anton")); 
              tft.println(F(" S. Garcia"));
              tft.println();
              tft.println(F(" Status:"));
              tft.println(F(" Student"));
              tft.println();
              tft.println(F(" Clearance Level: 1"));
              delay(3500);
              mainPage();
        } else if (uid[1] == uid4[1]) {
              tft.fillScreen(BLACK);
              tft.setCursor(50, 25);
              tft.setTextColor(YELLOW);    
              tft.setTextSize(3);
              tft.println(F("Scanning"));
              tft.setCursor(80, 49);
              tft.println(F("Card"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Please wait..."));
              delay(1500);
              tft.fillScreen(BLACK);
              tft.setCursor(60, 25);
              tft.setTextColor(RED);
              tft.setTextSize(3); 
              tft.println(F("Access"));
              tft.setCursor(60, 49); 
              tft.println(F("Denied"));  
              tft.println();   
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.println(F(" Name:"));
              tft.println(F(" Hans"));
              tft.println(F(" T. Mingala"));
              tft.println();
              tft.println(F(" Status:"));
              tft.println(F(" Student"));
              tft.println();
              tft.println(F(" Clearance Level: 1"));
              delay(3500);
              mainPage();
        } else if (uid[1] == uid5[1]) {
              tft.fillScreen(BLACK);
              tft.setCursor(50, 25);
              tft.setTextColor(YELLOW);    
              tft.setTextSize(3);
              tft.println(F("Scanning"));
              tft.setCursor(80, 49);
              tft.println(F("Card"));
              tft.println();
              tft.setTextSize(2);
              tft.println(F(" Please wait..."));
              delay(1500);
              tft.fillScreen(BLACK);
              tft.setCursor(60, 25);
              tft.setTextColor(RED);    
              tft.setTextSize(3);
              tft.println(F("Access"));
              tft.setCursor(60, 49); 
              tft.println(F("Denied"));  
              tft.println();   
              tft.setTextColor(WHITE);
              tft.setTextSize(2);
              tft.println(F(" Name:"));
              tft.println(F(" Patrick Angelo")); 
              tft.println(F(" G. Adducul"));
              tft.println();
              tft.println(F(" Status:"));
              tft.println(F(" Student"));
              tft.println();
              tft.println(F(" Clearance Level: 1"));
              delay(3500);
              mainPage();
        }
      }

      s++; 
} 

void lockPage() {
      s = 0;
      tft.fillScreen(BLACK);
      tft.setCursor(12, 25);
      tft.setTextColor(RED);    
      tft.setTextSize(3);
      tft.println(F("Authenticate"));
        
      do {
        secureCard();
        switch (s) {
          case 1:
            tft.setTextSize(2);
            tft.println(F(" Scan authorized")); 
            tft.println(F(" card in:"));
            tft.println();
            tft.setCursor(65, 120);
            tft.setTextSize(20);
            tft.println(F("5"));
            delay(50);
          break;
          case 2:
            tft.fillScreen(BLACK);
            tft.setCursor(12, 25);   
            tft.setTextSize(3);
            tft.println(F("Authenticate"));
            tft.setTextSize(2);
            tft.println(F(" Scan authorized")); 
            tft.println(F(" card in:"));
            tft.println();
            tft.setCursor(65, 120);
            tft.setTextSize(20);
            tft.println(F("4"));
          break;
          case 3:
            tft.fillScreen(BLACK);
            tft.setCursor(12, 25);   
            tft.setTextSize(3);
            tft.println(F("Authenticate"));
            tft.setTextSize(2);
            tft.println(F(" Scan authorized")); 
            tft.println(F(" card in:"));
            tft.println();
            tft.setCursor(65, 120);
            tft.setTextSize(20);
            tft.println(F("3"));
          break;
          case 4:
            tft.fillScreen(BLACK);
            tft.setCursor(12, 25);  
            tft.setTextSize(3);
            tft.println(F("Authenticate"));
            tft.setTextSize(2);
            tft.println(F(" Scan authorized")); 
            tft.println(F(" card in:"));
            tft.println();
            tft.setCursor(65, 120);
            tft.setTextSize(20);
            tft.println(F("2"));
          break;
          case 5:
            tft.fillScreen(BLACK);
            tft.setCursor(12, 25);   
            tft.setTextSize(3);
            tft.println(F("Authenticate"));
            tft.setTextSize(2);
            tft.println(F(" Scan authorized")); 
            tft.println(F(" card in:"));
            tft.println();
            tft.setCursor(65, 120);
            tft.setTextSize(20);
            tft.println(F("1"));
          break;
          case 6:
            mainPage();
          break;
        }
      } while (s < 6); 
}
