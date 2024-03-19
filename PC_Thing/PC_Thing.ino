/*
  WriteSingleField
  
  Description: Writes a value to a channel on ThingSpeak every 20 seconds.
  
  Hardware: ESP32 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires installation of EPS32 core. See https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md for details. 
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/

#include <WiFi.h>
#include "Secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <M5StickCPlus.h>

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

int number = 0;


int lastUpdate = 0;
int currentMilis =  0;

void setup() {
  M5.begin();
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  M5.Lcd.setRotation(3);

}

void loop() {



  currentMilis = millis();


  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  if(currentMilis - lastUpdate >= 20000 ){
      int x = ThingSpeak.writeField(myChannelNumber, 1, number, myWriteAPIKey);
      if(x == 200){
      Serial.println("Channel update successful.");
      M5.Lcd.fillScreen(TFT_DARKGREEN);
      lastUpdate = currentMilis;
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(x));
        M5.Lcd.fillScreen(TFT_MAROON);
        M5.Lcd.setCursor(30, 30);
        M5.Lcd.println(String(x));
      }
  }

  M5.update();
  if(M5.BtnA.wasReleased()) number++;

  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(30, 10);
  M5.Lcd.printf("%d", number);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(50, 10);
  M5.Lcd.printf("%d", currentMilis);
}
