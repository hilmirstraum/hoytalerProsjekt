
//definering av forskjellige porter sånn at vi kan sikte til de senere i koden uten at de må ta opp minne på arduinoen.
#define volumeDown 5 //port for å skru volumet på Bluethootkortet ned
#define volumeUp 6 //port for å skru volumet på Bluethootkortet opp
#define volume100 3
#define volume50 4
#define volume0 5


int volume = 100; //faktiske volumet i prosent
int newVolume = 100; //verdi for hva volumet skal endre seg til

float volumeChangeStartTime; //forteller når man startet å sende et signal til ultralydsensoren for å justere volumet


void setup(){
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);
  pinMode(volume100, OUTPUT);
  pinMode(volume50, OUTPUT);
  pinMode(volume0, OUTPUT);
  }

void loop(){

  /*justering av volum
  --------------------
  --------------------*/
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
      volumeChangeStartTime = 0; //oppdaterer volumet til nodemcuen.
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
      volumeChangeStartTime = 0;}}


    if (newVolume == 100){
      digitalWrite(volume100, HIGH);
      digitalWrite(volume50, LOW);
      digitalWrite(volume0, LOW);
      if (volume == 100){volume=0;}
    }
    else if (newVolume == 50){
      digitalWrite(volume50, HIGH);
      digitalWrite(volume100, LOW);
      digitalWrite(volume0, LOW);
      if (volume == 50){volume=100;}
    }
    else if (newVolume == 0){
      digitalWrite(volume0, HIGH);
      digitalWrite(volume50, LOW);
      digitalWrite(volume100, LOW);
      if (volume == 0){volume=50;}
    }

    } //oppadeter volumet på nodemcuen
