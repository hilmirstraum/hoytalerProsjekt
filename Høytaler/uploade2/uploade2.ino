#include <SoftwareSerial.h>

#define txpin 2 
#define rxpin 3
int y;
boolean x = false;
SoftwareSerial mySerial = SoftwareSerial(rxpin, txpin);

void setup(){
  pinMode(rxpin, INPUT);
  pinMode(txpin, OUTPUT);
  //pinMode(7, OUTPUT)
  mySerial.begin(9600);
  Serial.begin(9600);
}

void loop(){
  x=false;
  y= 0;
  if (mySerial.available()){
    y = mySerial.read();
    x = true;}
  if (x){
  y +=1;
  Serial.println(y);}
    //digitalWrite(7, HIGH);
    }
