
//definering av forskjellige porter sånn at vi kan sikte til de senere i koden uten at de må ta opp minne på arduinoen.
#define volumeDown 5 //port for å skru volumet på Bluethootkortet ned
#define volumeUp 6 //port for å skru volumet på Bluethootkortet opp


int volume = 100; //faktiske volumet i prosent
int newVolume = 100; //verdi for hva volumet skal endre seg til

float volumeChangeStartTime; //forteller når man startet å sende et signal til ultralydsensoren for å justere volumet


void setup(){
  Serial.begin(9600);
  pinMode(volumeDown, OUTPUT);
  pinMode(volumeUp, OUTPUT);
  }

void loop(){

  /*justering av volum
  --------------------
  --------------------*/
  //justering av volumet på Bluethootkortet
  if (volume != newVolume && volumeChangeStartTime == 0){ //tester om det er skjedd en endring av newvolum forrige runde programet kjørte gjennom
    volumeChangeStartTime = millis();} //starter en klokke som forteller når vi startet å endre volumet. volumeChangeStartTime er tiden da endringen av volumet startet

  //volumet er over det ønskede volumet så det må justeres ned
  if (newVolume < volume){
    digitalWrite(volumeUp, LOW);
    digitalWrite(volumeDown, HIGH); //sender signal om at volumet skrues ned
    if ((volume - newVolume)*50 < (millis())-volumeChangeStartTime){ //ser om tiden programet startet å endre programet (i millisekund etter oppstarten av programet) + endringen i volum ganger 50 ettersom det tar 5000 millisekund å endre volumet fra 0-100 er større en den nåverende tiden (millis())
      volume = newVolume; //oppdaterer verdien volum til det nåverende volumet
      digitalWrite(volumeDown, LOW);
      volumeChangeStartTime = 0; //oppdaterer volumet til nodemcuen.
    }
    }
  //volumet er under det ønskede volumet så det må justeres oppover
  else if (newVolume > volume){
    digitalWrite(volumeDown, LOW);
    digitalWrite(volumeUp, HIGH);
    if ((newVolume - volume)*50 < (millis())-volumeChangeStartTime){ //beregner om singalet er sendt lenge nok til at den er ferdig med endringne i volumet
      volume = newVolume; //oppdaterer volumet til det nåverende volumet
      digitalWrite(volumeUp, LOW);
      volumeChangeStartTime = 0;}}


    //testkode som skrur volumet fra 0 til 100 med hopp på 10 (0, 10, 20, etc) og så fra 100 til 0.
    if (volume == newVolume){
      delay(500);
      if (volume == 100){
        newVolume=0;
      }
      else{newVolume += 10;}
      Serial.println(newVolume);
    }
}
