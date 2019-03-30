#define volumeDown 2
#define volumeUp 3

float volume = 100; //nåværende volumet
float newVolume = 100; //ønsket volum
int lastVolumeUpdate; //grunnnen til at dette er en float verdi er at arduinoen er 8bit så int er mellom -32768 og 32768 noe vi kan ungå ved at en av verdiene er float.

void setup(){
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);
  }

void loop(){
  if (newVolume > volume){
    digitalWrite(volumeUp, HIGH);
    digitalWrite(volumeDown, LOW);
    volume = volume + (millis() - lastVolumeUpdate)*45;
    lastVolumeUpdate = millis();
    if (newVolume =< volume){
      volume = newVolume;
      digitalWrite(volumeUp, LOW);
      }}

  else if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH);
    volume = volume + (millis() - lastVolumeUpdate)*45;
    lastVolumeUpdate = millis();
    if (newVolume => volume){
      volume = newVolume;
      digitalWrite(volumeDown, LOW);
      }}

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
