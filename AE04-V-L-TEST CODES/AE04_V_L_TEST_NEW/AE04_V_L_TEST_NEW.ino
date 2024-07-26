/*
 * RTC Check
 * micro SD Card Check 
 * RS485
 * EC25 ADDON
 * All Output Turn ON Series
 * All input status serial print
 * Turns ON All Outputs in series
 * Serial prints all the input status
 * External Antenna Test
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>
#include "FS.h"
#include "SD.h"
#include "RTClib.h"

#define ANALOG_PIN_0 36

#define INPUT1 27
#define INPUT2 34
#define INPUT3 35
#define INPUT4 14
#define INPUT5 13
#define INPUT6 5

#define OUTPUT1 12
#define OUTPUT2 2

#define RS485_RX 25
#define RS485_TX 26
#define RS485_FC 22

#define GSM_RX 33
#define GSM_TX 32
#define GSM_RESET 21

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;

RTC_DS3231 rtc; 
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int analog_value = 0;
  
int readSwitch(){
  analog_value = analogRead(ANALOG_PIN_0);
  return analog_value; //Read analog
}
unsigned long int timer1 = 0;

// ================================================ SETUP ================================================
void setup() {
  Serial.begin(115200);
  Serial.println("Hello");
  
  pinMode(RS485_FC, OUTPUT);
  digitalWrite(RS485_FC, HIGH);   // RS-485 
  
  Serial1.begin(9600, SERIAL_8N1, RS485_RX, RS485_TX); 
  Serial2.begin(115200, SERIAL_8N1, GSM_RX, GSM_TX); 

  pinMode(GSM_RESET, OUTPUT);
  digitalWrite(GSM_RESET, HIGH);   
  
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);

  pinMode(15, OUTPUT);
  digitalWrite(15,HIGH);

  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(INPUT3, INPUT);
  pinMode(INPUT4, INPUT);
  pinMode(INPUT5, INPUT);
  pinMode(INPUT6, INPUT);
  Wire.begin(16,17);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  if (!ads1.begin(0x48)) {
    Serial.println("Failed to initialize ADS 1 .");
    while (1);
  }
  if (!ads2.begin(0x49)) {
    Serial.println("Failed to initialize ADS 2 .");
    while (1);
  }
  RTC_Check();
  delay(1000);
  SD_CHECK();
  delay(1000);
  
  Serial.println("Testing Modem");  
  timer1 = millis();
  Serial2.println("AT");
  while(millis()<timer1+10000){
    while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
    }
  }
  timer1 = millis();
  Serial2.println("AT+CPIN?");
  while(millis()<timer1+10000){
    while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
    }
  }
  timer1 = millis();
  Serial2.println("AT+CFUN?");
  while(millis()<timer1+10000){
    while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
    }
  }
  Serial.println("Testing Modem Done");
  adcAttachPin(36);
  digitalWrite(RS485_FC, HIGH);   // RS-485 
}

void loop() {
  int16_t adc0, adc1, adc2, adc3;
  float volts0, volts1, volts2, volts3;
  // read from port 0, send to port 1:
  while (Serial.available()) {
    int inByte = Serial.read();
    Serial2.write(inByte);
  }
  while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
  }

  Serial.print(digitalRead(INPUT1));
  Serial.print(digitalRead(INPUT2));
  Serial.print(digitalRead(INPUT3));
  Serial.print(digitalRead(INPUT4));
  Serial.print(digitalRead(INPUT5));
  Serial.print(digitalRead(INPUT6));
  Serial.println(""); 
  
  adc0 = ads1.readADC_SingleEnded(0);
  adc1 = ads1.readADC_SingleEnded(1);
  adc2 = ads1.readADC_SingleEnded(2);
  adc3 = ads1.readADC_SingleEnded(3);

  Serial.println("-----------------------------------------------------------");
  Serial.print("AIN1: "); Serial.print(adc0); Serial.println("  ");
  Serial.print("AIN2: "); Serial.print(adc1); Serial.println("  ");
  Serial.print("AIN3: "); Serial.print(adc2); Serial.println("  ");
  Serial.print("AIN4: "); Serial.print(adc3); Serial.println("  ");

  adc0 = ads2.readADC_SingleEnded(0);
  adc1 = ads2.readADC_SingleEnded(1);
  adc2 = ads2.readADC_SingleEnded(2);
  adc3 = ads2.readADC_SingleEnded(3);

  Serial.print("AIN5: "); Serial.print(adc0); Serial.println("  ");
  Serial.print("AIN6: "); Serial.print(adc1); Serial.println("  ");

  Serial.println(""); 
  Serial.print("Push button  ");
  Serial.println(readSwitch());
  Serial.println(""); 
  
  digitalWrite(OUTPUT1, HIGH);
  digitalWrite(OUTPUT2, LOW);
  delay(500);
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, HIGH);
  delay(500);
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, LOW);
   
 
  digitalWrite(RS485_FC, HIGH);                    // Make FLOW CONTROL pin HIGH
  delay(500);
  Serial1.println(F("RS485 01 SUCCESS"));    // Send RS485 SUCCESS serially
  delay(500);                                // Wait for transmission of data
  digitalWrite(RS485_FC, LOW) ;                    // Receiving mode ON
                                             // Serial1.flush() ;
  delay(500);     
  
  while (Serial1.available()) {  // Check if data is available
    char c = Serial1.read();     // Read data from RS485
    Serial.write(c);             // Print data on serial monitor
  }
 delay(500);
}

void displayTime(void) {
  DateTime now = rtc.now();  
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(1000);
}

void RTC_Check(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
 else{
 if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  }
   
  int a=1;
  while(a<6) {
    displayTime();   // printing time function for oled
    a=a+1;
  }
 }
}

void SD_CHECK(){
  uint8_t cardType = SD.cardType();
  if(SD.begin(15))
 {
  Serial.println("Card Mount: success");
  Serial.print("Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } 
  else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } 
  else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } 
  else {
      Serial.println("Unknown");
  }
  int cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("Card Size: %lluMB\n", cardSize);
  }
  if(!SD.begin(15)) {
     Serial.println("NO SD card");            
  }
}
