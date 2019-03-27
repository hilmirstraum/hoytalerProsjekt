#define volumeDown 2
#define volumeUp 3

int volume = 100; //volumet i prosent
int newVolume = 100;

void setup(){
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);
  }

void loop(){

  if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH);
    delay((volume-newVolume)*45);
    digitalWrite(volumeDown, LOW);
    volume = newVolume;}

    else if (newVolume > volume){
      digitalWrite(volumeDown, LOW);
      digitalWrite(volumeUp, HIGH);
      delay((newVolume-volume)*45);
      digitalWrite(volumeUp, LOW);
      volume = newVolume;}

  if (newVolume == volume){
    if (volume == 0){
      newVolume = 100;
      Serial.println(100);}
    else{newVolume = 0;
    Serial.println(0);}
  }

  }
