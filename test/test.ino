
//definering av forskjellige porter sånn at vi kan sikte til de senere i koden uten at de må ta opp minne på arduinoen.
#define volumeDown 5 //port for å skru volumet på Bluethootkortet ned
#define volumeUp 6 //port for å skru volumet på Bluethootkortet opp
#define volume100 4
#define volume50 3
#define volume0 2


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
  //justering av volumet på Bluethootkortet
  if (volume != newVolume && volumeChangeStartTime == 0){
    volumeChangeStartTime = millis();} //starter en klokke som forteller når vi startet å endre volumet

  //volumet er over det ønskede volumet så det må justeres ned
  if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH);
    if (((volume - newVolume)^0.5/2)*1000 < (millis())-volumeChangeStartTime){ //beregner om singalet er sendt lenge nok til at den er ferdig med endringne i volumet
      volume = newVolume;
      digitalWrite(volumeDown, LOW);
      volumeChangeStartTime = 0; //oppdaterer volumet til nodemcuen.
    }
    }
  //volumet er under det ønskede volumet så det må justeres oppover
  else if (newVolume > volume){
    digitalWrite(volumeDown, LOW);
    digitalWrite(volumeUp, HIGH);
    if (((newVolume - volume)^0.5/2)*1000 < (millis())-volumeChangeStartTime){ //beregner om singalet er sendt lenge nok til at den er ferdig med endringne i volumet
      volume = newVolume;
      digitalWrite(volumeUp, LOW);
      volumeChangeStartTime = 0;}}


    if (volume == newVolume){
      delay(500);
      if (volume == 100){
        newVolume=0;
      }
      else{newVolume += 10;}
      Serial.println(newVolume);
    }

    }
