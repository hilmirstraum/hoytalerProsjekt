int const echoPin = 8;
int const trigerPin = 7;

float distance[11];
float averageDistance;
float currentDistance;
int const maxDistance = 100;
int const minDistance = 10;

int const changeVolumeSensitivity = 1;
float newVolume;
float originalVolume;

float handInside[] = {0, 0};

void setup(){
  pinMode(echoPin, INPUT);
  pinMode(trigerPin, OUTPUT);
  Serial.begin(9600);}

void loop(){

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
    newVolume = originalVolume + (averageDistance - handInside[1])*changeVolumeSensitivity;
    //sørger for at newVolumet ikke går utenfor 0-100
    if (newVolume > 100){
      newVolume = 100;}
    else if (newVolume < 0){
      newVolume = 0;}}A

  //skriver ut verdiene
  /*Serial.print("volume: ");
  Serial.print(volume);
  Serial.print(" originalVolume: ");
  Serial.println(originalVolume);*/
  /*for (int i = 0; i<11; i++){
    if (i != 10){
      Serial.print(distance[i]);
      Serial.print(", ");
    }
    else{
      Serial.println(distance[10]);}}*/
}
