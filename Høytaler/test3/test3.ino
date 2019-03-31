//arduino side
#include <SoftwareSerial.h>
#define rxPin 3
#define txPin 2

//biblioteker for skjermen
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

SoftwareSerial softSerial(rxPin, txPin);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

int volume;
int lastVolume;
boolean appConnected = false;

boolean changedVolume = false;

String serialInfo = "";
boolean showIpAddres = true;

void setup(){
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  softSerial.begin(9600);
  Serial.begin(9600);
  while (showIpAddres){
    if (softSerial.available()>0){
      serialInfo = softSerial.readStringUntil('\n');
    }
    if (serialInfo.indexOf("stop ip") != -1){showIpAddres = false;}
    display.clearDisplay(); //fjerner all informasjon på skjermen
    display.setTextSize(2);      // størelsen på teksten
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);
    //display.println("ip: " + wifi.localIP());
    display.println("ip");
    display.println(wifi.localIP())
    display.display();
  }
  display.clearDisplay();
  display.display();
}

void loop(){
  Serial.print("hello");
    }
