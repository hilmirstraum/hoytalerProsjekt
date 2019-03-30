#define volumeDown 2
#define volumeUp 3
#define echoPin 8
#define trigerPin 7
int const playPin = 4;

int volume = 100; //volumet i prosent
int newVolume = 100;

boolean paused = false;

float distance[11];
float averageDistance;
float currentDistance;
int const maxDistance = 100;
int const minDistance = 10;

int const changeVolumeSensitivity = 1;
float originalVolume;

float handInside[] = {0, 0};

float volumeChangeStartTime;


void setup(){
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigerPin, OUTPUT);
  }

void loop(){

  //justering av volumet på Bluethootkortet
  if (volume != newVolume && volumeChangeStartTime == 0){
    /*if (volume < newVolume){
      digitalWrite(volumeUp, HIGH);
      delay(2000);
    }
    else{
      digitalWrite(volumeDown, HIGH);
      delay(2000);
    }*/
    volumeChangeStartTime = millis();} //starter en klokke som forteller når vi startet å endre volumet

  if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH);
    if ((volume - newVolume)*50 < (millis())-volumeChangeStartTime){ //hvis endringen i volum er ferdig
      volume = newVolume;
      digitalWrite(volumeDown, LOW);
      volumeChangeStartTime = 0;
    }
    }

  else if (newVolume > volume){
    digitalWrite(volumeDown, LOW);
    digitalWrite(volumeUp, HIGH);
    if ((newVolume - volume)*50 < (millis())-volumeChangeStartTime){ //hvis endringen i volum er ferdig
      volume = newVolume;
      if (newVolume == 100){}
      digitalWrite(volumeUp, LOW);
      volumeChangeStartTime = 0;}}


    /*testing
    ----------------------------------------
    ----------------------------------------
    ----------------------------------------*/
    Serial.print(newVolume);
    Serial.print("    ");
    Serial.println(volume);




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
    //regner ut endringen i newVolumet
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
