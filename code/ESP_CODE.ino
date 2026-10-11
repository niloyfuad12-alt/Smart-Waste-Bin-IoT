/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP32 chip.

  NOTE: This requires ESP32 support package:
    https://github.com/espressif/arduino-esp32

  Please be sure to select the right ESP32 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL66sRXtJf"
#define BLYNK_TEMPLATE_NAME "Dustbin"
#define BLYNK_AUTH_TOKEN "qt_vErUcYVGfJ7qa8dDZBrxHf1Oqpls"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Niloy";
char pass[] = "112233";

void setup()
{
  // Debug console
  Serial.begin(9600);
  Serial2.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
  Blynk.run();
  if (Serial2.available() > 0) {
    String data = Serial2.readStringUntil('\n');
    Blynk.virtualWrite(V0, data.substring(0, data.indexOf("p")));
    Blynk.virtualWrite(V1, data.substring(data.indexOf("p") + 1, data.indexOf("s")));
    Blynk.virtualWrite(V2, data.substring(data.indexOf("s") + 1, data.indexOf("g")));
    Blynk.virtualWrite(V3, data.substring(data.indexOf("g") + 1, data.indexOf("b")));
    Blynk.virtualWrite(V4, data.substring(data.indexOf("b") + 1,data.length()));
    Serial.println(data);
  }
}
