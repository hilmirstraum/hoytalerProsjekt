#define volumeDown 2
#define volumeUp 3
#define echoPin 8
#define trigerPin 7

int volume = 100; //volumet i prosent
int newVolume = 100;



float distance[11];
float averageDistance;
float currentDistance;
int const maxDistance = 100;
int const minDistance = 10;

int const changeVolumeSensitivity = 1;
float originalVolume;

float handInside[] = {0, 0};

int volumeChangeStartTime = 0;


void setup(){
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigerPin, OUTPUT);
  }

void loop(){

  //justering av volumet på Bluethootkortet
  if (volume != newVolume && volumeChangeStartTime == 0){volumeChangeStartTime = millis();} //starter en klokke som forteller når vi startet å endre volumet

  if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH);
    if ((volume - newVolume)*45 < (millis())-volumeChangeStartTime){ //hvis endringen i volum er ferdig
      volume = newVolume;
      digitalWrite(volumeDown, LOW);
      volumeChangeStartTime = 0;
    }
    }

  else if (newVolume > volume){
    digitalWrite(volumeDown, LOW);
    digitalWrite(volumeUp, HIGH);
    if ((newVolume - volume)*45 < (millis())-volumeChangeStartTime){ //hvis endringen i volum er ferdig
      volume = newVolume;
      digitalWrite(volumeUp, LOW);
      volumeChangeStartTime = 0;}}

  //endring av verdien newvolume ved hjelp av ultralydsensoren
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
    originalVolume = volume;}
  }
  //regner ut endringen i volumet
  if (handInside[0] == 1){
    newVolume = originalVolume + (averageDistance - handInside[1])*changeVolumeSensitivity;
    //sørger for at volumet ikke går utenfor 0-100
    if (newVolume > 100){
      newVolume = 100;}
    else if (newVolume < 0){
      newVolume = 0;}}

  //skriver ut verdiene
  Serial.print(newVolume);
  Serial.print(": ");
  Serial.println(volume);
  /*for (int i = 0; i<11; i++){
    if (i != 10){
      Serial.print(distance[i]);
      Serial.print(", ");
    }
    else{
      Serial.println(distance[10]);}}*/


      }
