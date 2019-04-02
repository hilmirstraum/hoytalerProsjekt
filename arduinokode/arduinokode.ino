#include <SoftwareSerial.h>

#define volumeDown 4
#define volumeUp 5
#define echoPin 12
#define trigerPin 13
#define rxPin 3
#define txPin 2
#define skipSong 7
#define lastSong 8
#define playPin 6


boolean sendtIpStopMessage = false;


SoftwareSerial softSerial = SoftwareSerial(rxPin, txPin);

int volume = 100; //volumet i prosent
int newVolume = 100;

boolean paused = false;
String serialInfo = "";

float distance[11];
float averageDistance;
float currentDistance;
int const maxDistance = 100;
int const minDistance = 10;

int const changeVolumeSensitivity = 1;
float originalVolume;

boolean appConnected = false;

float handInside[] = {0, 0};

float volumeChangeStartTime;


void setup(){
  softSerial.begin(9600);
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigerPin, OUTPUT);
  }

void loop(){
  /*mottakning av serial info
  -------------------------
  -------------------------*/
  if (softSerial.available()>0){
    serialInfo = softSerial.readStringUntil('\n');
  }
  if (serialInfo.indexOf("volume:" != -1)){
  newVolume = serialInfo.substring(7 + serialInfo.indexOf("volume:")).toInt();}

  else if (serialInfo.indexOf("appConnected") != -1){
    appConnected = true;
  }
  else if (serialInfo.indexOf("appDisconnected") != -1){
    appConnected = false;
  }
  else if (serialInfo.indexOf("play") != -1 || serialInfo.indexOf("pause") != -1){ //starter/stopper musikken
    digitalWrite(playPin, HIGH);
    delay(400); //Bluethootkortet bruker ca 105 ms på å registrere at musikken pauses.
    digitalWrite(playPin, LOW);
  }

  else if (serialInfo.indexOf("skip song") != -1){ //hopper til neste sang
    digitalWrite(skipSong, HIGH);
    delay(200);
    digitalWrite(lastSong, LOW);
  }
  else if (serialInfo.indexOf("last song") != -1){ //går til forrige sang
    digitalWrite(skipSong, HIGH);
    delay(200);
    digitalWrite(lastSong, LOW);
  }


  //sender info om at man skal stoppe å vise ip addressen hvis noe er over ultralydsensoren
  if (appConnected == false && averageDistance < 100 && averageDistance != 0 && sendtIpStopMessage == false){
    softSerial.println("stop ip");
    Serial.println("stop ip");
    sendtIpStopMessage = true;
  }


  if (appConnected == false){ //app er ikke koblet til nodemcuen så vi må justere volumet på Bluethootkortet
  //justering av volumet på Bluethootkortet
  if (volume != newVolume && volumeChangeStartTime == 0){
    volumeChangeStartTime = millis();} //starter en klokke som forteller når vi startet å endre volumet

  if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH);
    if ((volume - newVolume)*50 < (millis())-volumeChangeStartTime){ //hvis endringen i volum er ferdig
      volume = newVolume;
      digitalWrite(volumeDown, LOW);
      volumeChangeStartTime = 0;
      softSerial.println(newVolume); //oppdaterer volumet til nodemcuen.
    }
    }

  else if (newVolume > volume){
    digitalWrite(volumeDown, LOW);
    digitalWrite(volumeUp, HIGH);
    if ((newVolume - volume)*50 < (millis())-volumeChangeStartTime){ //hvis endringen i volum er ferdig
      volume = newVolume;
      if (newVolume == 100){}
      digitalWrite(volumeUp, LOW);
      volumeChangeStartTime = 0;
      softSerial.println(newVolume);}}}

    //app er koblet til nodemcuen så den skriver bare en oppdatering av volumet til nodemcuen.
    else{
      if (newVolume != volume){
        softSerial.println(newVolume);
        volume = newVolume;
      }
    }



    /*bruk av ultralydsensoren for å finne endringen i volumet
    --------------------------------------------
    --------------------------------------------
    */
    //beregner avstanden som ultralydsensoren registrerer
      digitalWrite(trigerPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigerPin, HIGH);
      delay(10);
      digitalWrite(trigerPin, LOW);
      currentDistance = pulseIn(echoPin, HIGH)*0.034/2;

    //sørger for at man bare lagrer verdien hvis den er innenfor maxverdien
    if (currentDistance < maxDistance){
      distance[int(distance[0])] = currentDistance;
    }
    else{
      distance[int(distance[0])] = 0;}

    //justerer distance[0] som er hvilken verdi som er den eldste
    if (distance[0] < 10){
      distance[0]  ++;}
    else{
    distance[0] = 1;}

    //regner ut gjennomsnittet av verdiene lagret i distance
    {float sum = 0;
    int numberOfReadings = 0;
    for (int i = 1; i<11; i++){
      //regner ut gjennomsnittsavstanden
        //sikrer at man bare regner ut gjennomsnittet av de verdiene som er innenfor maksverdien
        if (distance[i] != 0){
          sum += distance[i];
          numberOfReadings ++;}
        //tar gjennomsnittet av verdiene som er lagt sammen i forige if statement
        if (i == 10){
          averageDistance = sum/numberOfReadings;}}

    //Hvis det er færre en 5 verdier som er innenfor maksverdien skal det regnes som at hånden er utenfor lydsensoren
    if (numberOfReadings < 5){
      handInside[0] = 0;
      handInside[1] = 0;
      originalVolume = 0;}
    else if (handInside[0] == 0){
      handInside[0] = 1;
      handInside[1] = averageDistance;
      originalVolume = newVolume;}
    }
    //stopper/starter musikken og regner ut endringen i newVolume
    if (handInside[0] == 1){
       //sikrer at den ikke pauser sangen mer en en gang når hånden er innefor området og at den ikke endrer på volumet når sangen er blitt pauset hvis man flytter hånden
      if (averageDistance < 5 && paused != true){ //hvis hånden er innenfor 5 cm skal den pause/starte musikken istedenfor å justere volumet.        digitalWrite(playPin, HIGH);
        Serial.println("pausing/starting phone");
        digitalWrite(playPin, HIGH);
        delay(400); //Bluethootkortet bruker ca 105 ms på å registrere at musikken pauses.
        digitalWrite(playPin, LOW);
        paused = true;
      }else{
        newVolume = originalVolume + (averageDistance - handInside[1])*changeVolumeSensitivity;}}

      else{paused = false;} //sørger for at den ikke starter og stopper musikken flere ganger når man tar hånden over ultralydsensoren



      //sørger for at newVolumet ikke går utenfor 0-100
      if (newVolume > 100){
        newVolume = 100;}
      else if (newVolume < 0){
        newVolume = 0;}
        }
