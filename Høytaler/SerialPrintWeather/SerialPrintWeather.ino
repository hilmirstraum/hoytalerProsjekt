//biblioteker for å hente data fra yr
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//biblioteker for skjermen
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//for komunikasjon med arduinoen
#include <SoftwareSerial.h>
#define rxpin 2 //D4
#define txpin 0 //D3
SoftwareSerial softSerial = SoftwareSerial(rxpin, txpin);
int volume = 0; //verdien som kommer via serial porten er volumet som skal vises på skjermen
int lastVolume = 0;
String serialInfo = ""; //informasjon som sendes via serialporten
WiFiServer server(80);

char* ssid = "ARRIS-B692";
char* password = "1CFC1251C0269E58";


//skaper et objekt for skjermen
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//klasse for å hente data fra yr, ble gjort fordi det da er mindre sjanse for å rotte det til hvis vi får den andre skjermen. Da er dette veldig oversiktelig og greit.


void setup() {
  Serial.begin(9600); //for feilmeldinger
  softSerial.begin(9600); //for komunikasjon med arduinoen

  //kobler til wifi
  if (WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){Serial.println("not connected to wifi"); yield();} //sikrer at man ikke går videre med koden før wifi er koblet til
    }

  //gjør klar skjermen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Start the server
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());

  //viser ip addressen på skjermen helt til ultralydsensoren registrer at hånden er over sensoren
  boolean showIpAddress = true;
  while (showIpAddress){
    if (softSerial.available()>0){
      serialInfo = softSerial.readStringUntil('\n');
    }
    if (serialInfo.indexOf("stop ip") != -1){showIpAddress = false;} //stopper å vise det på skjermen vis ultralydsensoren blir aktivert

    //legge til kode for å stoppe det hvis man kobler til appen
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //ta med: softSerial.println("appConnected");

    display.clearDisplay(); //fjerner all informasjon på skjermen
    display.setTextSize(2);      // størelsen på teksten
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);
    //display.println("ip: " + wifi.localIP());
    display.println("ip");
    display.println(WiFi.localIP());
    display.display();
  }
  display.clearDisplay();
  display.display();
}

void loop() {
 // weatherForecast.opperate(); //henter værdata

  //Sjekker om en client har kobla til
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  softSerial.println("appConnected");

  //seral kommunikasjon med arduinoen
  if (softSerial.available()>0){
    serialInfo = softSerial.readStringUntil('\n');}

  //sjekker om informasjonen som ble sent er en volum oppdatering
  if (serialInfo.indexOf("volume:")!= -1){
  volume = serialInfo.substring(7 + serialInfo.indexOf("volume:")).toInt();
  lastVolume = volume;
  /*putt in kode for å oppdatere  volumet til mobilen
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  putt in kode for å oppdatere  volumet til mobilen*/}

  // Venter til at klienten ha send noe data
  while(!client.available()){
    delay(1);
  }

  String req = client.readStringUntil('\r');
  client.flush();

  // Ser på dataen og sender de riktige kommandoene
  if (req.indexOf("/cmd/MUSIC_START") != -1)  // led=on
    softSerial.println("play");
  else if (req.indexOf("/cmd/MUSIC_STOP") != -1)
    softSerial.println("plause");
  else if(req.indexOf("/cmd/MUSIC_NEXT") != -1)
    softSerial.println("next song");
  else if(req.indexOf("/cmd/MUSIC_PREVIOUS") != -1)
    softSerial.println("last song");
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // printer ut svar
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");

  //oppdaterer volumet til arduinoen hvis noe et annet sted i koden har endret på volumet
  if (lastVolume != volume){
    softSerial.println(volume);
    lastVolume = volume;}



  //skriver ut til skjermen
  display.clearDisplay(); //fjerner all informasjon på skjermen
  display.setTextSize(2);      // størelsen på teksten
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);
  display.print(F("grader:"));
//  Serial.println(weatherForecast.temperature1);
//  display.println(weatherForecast.weather1);
  display.print(F("volume"));
  display.print(volume);
  //display.print(F(volume)) //gis fra arduinoen via serial port mest sannynlig
  display.display(); //oppdaterer skjermen

}

