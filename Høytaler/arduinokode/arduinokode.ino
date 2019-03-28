#include <SoftwareSerial.h>

#define rxpin 3 //D4
#define txpin 2 //D3

SoftwareSerial mySerial = SoftwareSerial(rxpin, txpin);

void setup(){
  pinMode(rxpin, INPUT);
  pinMode(txpin, OUTPUT);
  mySerial.begin(9600);
}

void loop(){

}
