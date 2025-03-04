#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <WiFiClient.h>
#include <Adafruit_SSD1306.h>
#include <BlynkSimpleEsp32.h>

float latitude, longitude;
String latitude_string, longitude_string;
unsigned int move_index = 1;

#define BLYNK_TEMPLATE_ID "TMPL68IScHkob"
#define BLYNK_TEMPLATE_NAME "Smart Bike"
#define BLYNK_AUTH_TOKEN "IYjQaDBZppyFmJ5TPTAS9WNr1q9Z0wXe"
#define BLYNK_PRINT Serial
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

TinyGPSPlus gps;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char *ssid = "tanggal tua";
const char *pass = "aku12345";
char auth[] = "IYjQaDBZppyFmJ5TPTAS9WNr1q9Z0wXe";

WidgetMap myMap(V1);
WiFiClient client;
HardwareSerial SerialGPS(2);

void setup()
{
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");

  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V0, "clr");

  // Clear the OLED display on startup
  display.clearDisplay();
  display.display();
}

void loop()
{
  while (SerialGPS.available() > 0)
  {
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        latitude_string = String(latitude, 6);
        longitude = gps.location.lng();
        longitude_string = String(longitude, 6);
        Serial.print("Latitude = ");
        Serial.println(latitude_string);
        Serial.print("Longitude = ");
        Serial.println(longitude_string);

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 20);
        display.print("Latitude: ");
        display.println(gps.location.lat(), 6);
        display.print("Longitude: ");
        display.println(gps.location.lng(), 6);
        display.display();

        Blynk.virtualWrite(V5, gps.location.lng());       // Send longitude data to pin V5 in Blynk
        Blynk.virtualWrite(V6, gps.location.lat());       // Send latitude data to pin V6 in Blynk
        Blynk.virtualWrite(V1, longitude, latitude);       // Send latitude and longitude data to pin V1 (WidgetMap) in Blynk

        Blynk.virtualWrite(V1, "clr");                    // Clear the map widget
        Blynk.virtualWrite(V1, longitude, latitude, "L"); // Add a marker at the specified location on the map
        Blynk.run();

        // Delay to control the refresh rate
        delay(1000);
      }
    }
  }
}
