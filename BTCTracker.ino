#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include "BTC_Bitmap.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char ssid[] = "DeezNutz";
char pass[] = "8nS72Mv2";
int status = WL_IDLE_STATUS;
WiFiSSLClient client;

void setup() {
  Serial.begin(9600);

  // initialize the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.println("Connected to wifi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connecting to server...");
    if (client.connect("api.coindesk.com", 443)) {
      client.println("GET /v1/bpi/currentprice.json HTTP/1.1");
      client.println("Host: api.coindesk.com");
      client.println("Connection: close");
      client.println();
      
      String line = "";
      while (client.connected()) {
        line = client.readStringUntil('\n');
        if (line.indexOf("rate_float") > 0) {
          DynamicJsonDocument doc(1024);
          deserializeJson(doc, line);
          float price = doc["bpi"]["USD"]["rate_float"].as<float>();
          Serial.print("Current bitcoin price (USD): ");
          Serial.println(price);

          display.clearDisplay();

          // Calculate the starting position to centerize the image horizontally
          int startX = (display.width() - your_bitmap_width) / 2;

          display.drawBitmap(startX, 0, BTC_Bitmap, your_bitmap_width, your_bitmap_height, 1);
          display.display();
          // display the BTC logo for 10 seconds
          delay(10000);
          //display the actual BTC price for a minute
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0,0);
          display.println("BTC Price:");
          display.setCursor(0,20);
          display.setTextSize(3);
          display.println("$" + String(price,0));
          display.display();
          break;
        }
      }
      client.stop();
    } else {
      Serial.println("Failed to connect to server");
    }
  } else {
    Serial.println("Wifi not connected");
  }
  delay(300000); // re=poll coindesk every 5 minutes for the price
}
