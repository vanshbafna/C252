#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#define DHTPIN 19
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "The Bafna's Home"
#define WLAN_PASS       "Bhkne321@"
/****************************************************************************/

Adafruit_BMP085 bmp;
String serverName = "http://192.168.1.11:5000/add-data";

void setup()
{
  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1) {}
  }
  dht.begin();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client;
    HTTPClient http;

    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");

    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(102000));
    Serial.println(" meters");
    Serial.println();

    float h = dht.readHumidity();
    if (isnan(h))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    else
    {
      Serial.print("Humidity: ");
      Serial.print(h);
    }

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"temperature\" : " + String(bmp.readTemperature()) + ",\"altitude\" : " + String(bmp.readAltitude()) + ",\"pressure\" : " + String(bmp.readAltitude(102000)) + ",\"humidity\" : " + String(dht.readHumidity()) + "}");
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();

  }
  else 
  {
    Serial.println("WiFi Disconnected");
  }

  delay(3000);
}
