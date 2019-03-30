#define volumeDown 2
#define volumeUp 3

int volume = 100; //volumet i prosent
int newVolume = 100;
float startVolumeChange; //grunnnen til at dette er en float verdi er at arduinoen er 8bit så int er mellom -32768 og 32768 noe vi kan ungå ved at en av verdiene er float.

void setup(){
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);
  }

void loop(){
  if (newVolume != volume && startVolumeChange == 0){startVolumeChange = millis();}

  if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH);
    if (millis() > startVolumeChange + (volume - newVolume)*45){
    digitalWrite(volumeDown, LOW);
    volume = newVolume;
    startVolumeChange = 0;}}

    else if (newVolume > volume){
      digitalWrite(volumeDown, LOW);
      digitalWrite(volumeUp, HIGH);
      if (millis() > startVolumeChange + (newVolume - volume)*45){
      digitalWrite(volumeUp, LOW);
      volume = newVolume;
      startVolumeChange = 0;}}

  if (newVolume == volume){
    if (volume == 0){
      newVolume = 100;
      //Serial.println(100);
      }
    else{newVolume = 0;
    //Serial.println(0);
    }
  }
  /*Serial.print("volume: ");
  Serial.print(volume);
  Serial.print(" newVolume: ");
  Serial.print(newVolume);
  Serial.print("   ");
  Serial.print(startVolumeChange);
  Serial.print("   ");
  Serial.print(millis());
  Serial.print("   ");
  Serial.print(startVolumeChange + (newVolume - volume)*45);
  Serial.print("  ");
  Serial.println(startVolumeChange + (volume - newVolume)*45);*/

  }
