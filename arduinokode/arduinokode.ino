#include <SoftwareSerial.h>

//definering av forskjellige porter sånn at vi kan sikte til de senere i koden uten at de må ta opp minne på arduinoen.
#define volumeDown 5 //port for å skru volumet på Bluethootkortet ned
#define volumeUp 6 //port for å skru volumet på Bluethootkortet opp
#define skipSong 8 //port for å gå til neste sang
#define lastSong 7 //port for å gå tilbake til forrige sang
#define playPin 4 //port for å pause og stoppe sangen
#define echoPin 12 //port for å motta signaler fra ultralyden
#define trigerPin 13 //port for å sende signaler til ultralyden
#define rxPin 3 //port for å motta serial informasjon fra nodemcuen
#define txPin 2 //sende porten for serial kommunikasjon med nodemcuen

boolean sendtIpStopMessage = false; //verdi som forteller om man har sendt melding til nodemcuen om å stoppe å vise ip addressen på skjermen


SoftwareSerial softSerial = SoftwareSerial(rxPin, txPin); //lager et objekt til classen sofwareSerial sånn at vi kan ha serial kommmunikasjon via andre porter en standard serial.

int volume = 100; //faktiske volumet i prosent
int newVolume = 100; //verdi for hva volumet skal endre seg til
float originalVolume; //volumet i det håndet tas over ultralydsensoren

boolean paused = false; //forteller om man har sendt et pausesignal i den perioden hånden har vært over ultralydsensoren
String serialInfo = ""; //lagrer beskjedene man får fra nodemcuen sånn at man kan se om den inneholder den innformasjonen vi er interesert i

//verdier for å beregne hvor langt unna ultralyden har registret noe
float distance[11]; //lagrer de 10 siste verdiene som er lest av ultralydsensoren. distanc[0] er hvilken verdi som er den eldste.
float averageDistance; //gjennomsnittet av de verdiene i distance som er under maksverdien
float currentDistance; //verdien som ble lest av sist av ultralydsensoren
float handInside[] = {0, 0}; //1 veri er om hånden er over ultralydsensoren eller ikke og andre verdi er hvor høyt hånden var over ultralydsensoren når den ble tatt over ultralydsensoren
#define maxDistance 100 //maksverdien for hvor langt opp den regner det som en avlesning. Hvis verdien som er blitt lest av er over denne verdien byttes den ut med 0. Dette er for å ungå problemer med at den registrer taket
#define minDistance 7 //verdien for når den registrer det som et pause signal istedenfor justering av volum


int const volumeSensitivity = 1; //verdien for hvor mye volumet skal endres for en gitt avstand ultralyden beveger seg.


boolean appConnected = false; //forteller om appen er koblet til nodemcuen sånn at vi vet om vi justere volumet på Bluethootkortet eller telefonnen.


float volumeChangeStartTime; //forteller når man startet å sende et signal til ultralydsensoren for å justere volumet


void setup(){
  softSerial.begin(9600);
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);
  pinMode(skipSong, OUTPUT);
  pinMode(lastSong, OUTPUT);
  pinMode(playPin, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigerPin, OUTPUT);
  }

void loop(){
  /*serial kommunikasjon med nodemcuen
  -------------------------
  -------------------------*/
  //leser av informasjonen som kommer fra nodemcuen og putter det i en string
  if (softSerial.available()>0){
    serialInfo = softSerial.readStringUntil('\n'); //bruker readStringUntil for å sikre at den bare leser av en linje av gangen.
    Serial.println(serialInfo);
  }
  //oppdatere volumet på arduinoen hvis det er blitt endret av appen
  if (serialInfo.indexOf("volume:") != -1){
  newVolume = serialInfo.substring(7 + serialInfo.indexOf("volume:")).toInt();}

  //oppdaterer om appen er koblet til høytaleren
  else if (serialInfo.indexOf("appConnected") != -1){
    appConnected = true;
  }
  //oppdaterer om appen er koblet til høytalerne
  else if (serialInfo.indexOf("appDisconnected") != -1){
    appConnected = false;
  }
  //sender stopp/start signal til Bluethootkortet hvis den får beskjed om det fra nodemcuen
  else if (serialInfo.indexOf("play") != -1 || serialInfo.indexOf("pause") != -1){ //starter/stopper musikken
    digitalWrite(playPin, HIGH);
    delay(400); //Bluethootkortet bruker ca 105 ms på å registrere at musikken pauses.
    digitalWrite(playPin, LOW);
  }
  //sender signal til Bluethootkortet om å gå til neste sang
  else if (serialInfo.indexOf("skip song") != -1){
    digitalWrite(skipSong, HIGH);
    delay(200);
    digitalWrite(lastSong, LOW);
  }
  //sender signal til Bluethootkortet om å gå til forrige sang
  else if (serialInfo.indexOf("last song") != -1){
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

  /*justering av volum
  --------------------
  --------------------*/
  if (appConnected == false){ //app er ikke koblet til nodemcuen så vi må justere volumet på Bluethootkortet
  //justering av volumet på Bluethootkortet
  if (volume != newVolume && volumeChangeStartTime == 0){
    volumeChangeStartTime = millis();} //starter en klokke som forteller når vi startet å endre volumet

  //volumet er over det ønskede volumet så det må justeres ned
  if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH);
    if ((volume - newVolume)*50 < (millis())-volumeChangeStartTime){ //beregner om singalet er sendt lenge nok til at den er ferdig med endringne i volumet
      volume = newVolume;
      digitalWrite(volumeDown, LOW);
      volumeChangeStartTime = 0;
      softSerial.println(newVolume); //oppdaterer volumet til nodemcuen.
    }
    }
  //volumet er under det ønskede volumet så det må justeres oppover
  else if (newVolume > volume){
    digitalWrite(volumeDown, LOW);
    digitalWrite(volumeUp, HIGH);
    if ((newVolume - volume)*50 < (millis())-volumeChangeStartTime){ //beregner om singalet er sendt lenge nok til at den er ferdig med endringne i volumet
      volume = newVolume;
      if (newVolume == 100){}
      digitalWrite(volumeUp, LOW);
      volumeChangeStartTime = 0;
      softSerial.println(newVolume);}}} //oppadeter volumet på nodemcuen

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
    //beregner avstanden som ultralydsensoren registrerer.
      digitalWrite(trigerPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigerPin, HIGH);
      delay(10);
      digitalWrite(trigerPin, LOW);
      currentDistance = pulseIn(echoPin, HIGH)*0.034/2; //lydens hastighet er 340 m/s og vi tar tiden frem og tilbake så derfor er det 0,034/2. pulseIn gjør om signalet til en tallverdi som er tiden signalet brukte.

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
    //regner ut gjennomsnittsavstanden (averageDistance)
    for (int i = 1; i<11; i++){ //går gjennom programet under 10 ganger for å kunne kjøre det for alle verdiene i handInside, bortsett fra den første verdien
        //sikrer at man bare regner ut gjennomsnittet av de verdiene som er innenfor maksverdien
        if (distance[i] != 0){
          sum += distance[i];
          numberOfReadings ++;}
        //tar gjennomsnittet av verdiene som er lagt sammen i forige if statement
        if (i == 10){
          averageDistance = sum/numberOfReadings;}}

    //Hvis det er færre en 5 verdier som er innenfor maksverdien skal det regnes som at hånden er utenfor ultralydsensoren
    if (numberOfReadings < 5){
      handInside[0] = 0;
      handInside[1] = 0;
      originalVolume = 0;}
    //hvis det er over 5 verdier og dette er første syklus hvor det er over 5 utregninger skal man sette en startverdi for hånden sånn at man kan regne ut hvor mye hånden har beveget seg.
    else if (handInside[0] == 0){
      handInside[0] = 1;
      handInside[1] = averageDistance;
      originalVolume = newVolume;}
    }

    /*annet
    ---------------------
    ---------------------*/
    //stopper/starter musikken og regner ut endringen i newVolume
    if (handInside[0] == 1){
      //pauser eller starter telefonen
      if (averageDistance < minDistance && paused != true){ //hvis hånden er innenfor 5 cm skal den pause/starte musikken istedenfor å justere volumet
        Serial.println("pausing/starting phone"); //git beskjed til eventuele pcer som er koblet til sånn at vi kan feilsøke
        digitalWrite(playPin, HIGH);
        delay(200); //Bluethootkortet bruker ca 105 ms på å registrere at musikken pauses.
        digitalWrite(playPin, LOW);
        paused = true;
      }else{
        newVolume = originalVolume + (averageDistance - handInside[1])*volumeSensitivity;}} //beregner nytt volum

      else{paused = false;} //sørger for at den ikke starter og stopper musikken flere ganger når man tar hånden over ultralydsensoren



      //sørger for at newVolumet ikke går utenfor 0-100
      if (newVolume > 100){
        newVolume = 100;}
      else if (newVolume < 0){
        newVolume = 0;}
        }
