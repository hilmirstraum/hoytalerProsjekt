//noodemcuside

#include <SoftwareSerial.h>
#define rxPin 3
#define txPin 2

SoftwareSerial softSerial(rxPin, txPin);


void setup(){
  softSerial.begin(9600);
  Serial.begin(9600);

}

void loop(){
  softSerial.println("stop ip");
  delay(10000);

}
