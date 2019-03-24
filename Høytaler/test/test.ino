int volume = 0;
int volumeChange = 0;
#define volume_up_pin 3
#define volume_down_pin 4

void setup(){
  pinMode(volume_up_pin, OUTPUT);
  pinMode(volume_down_pin, OUTPUT);
  Serial.begin(9600);}

void loop(){
  while(volume != volumeChange){
    if (volume < volumeChange){
      volume ++;
      digitalWrite(volume_up_pin, HIGH);
      delay(10);
      digitalWrite(volume_up_pin, LOW);}
    else{
      volume --;
      digitalWrite(volume_down_pin, HIGH);
      delay(10);
      digitalWrite(volume_down_pin, LOW);}
    if (volume == volumeChange){Serial.println("volume changed to" + String(volume));}}
  
  {String serialRead = "";
    while (Serial.available() != 0){
    serialRead += Serial.read();}
    volumeChange = serialRead.toInt();}
  }
