#include <SPI.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

char ssid[] = "Your_SSID";
char pass[] = "Your_Password";
int status = WL_IDLE_STATUS;
WiFiSSLClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial) continue;

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
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0,0);
          display.println("BTC Price:");
          display.setCursor(60,0);
          display.setTextSize(2);
          display.println(price,0);
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
  delay(60000);
}
