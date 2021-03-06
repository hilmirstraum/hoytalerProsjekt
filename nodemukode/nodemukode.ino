//biblioteker for å bruke nodemcuen til å hente data fra webservere
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//biblioteker for å kommunisere med skjermen
#include <Wire.h> //bygd inn bibliotek for kommunikasjon via i2c protokollen
#include <Adafruit_GFX.h> //bibliotek for å kontrollere skjermen
#include <Adafruit_SSD1306.h> //bibliotek for å kontrollere skjermen

//bibliotek for å kommunsiere med arduinoen ved å bruke serial over vanlige digitale pinner
#include <SoftwareSerial.h>

//setter opp serial kommunikasjon med arduinoen
#define rxpin 2 //D4, porten for å ta imot informasjon fra arduinoen
#define txpin 0 //D3, porten for å sende informasjon til arduinoen
SoftwareSerial softSerial = SoftwareSerial(rxpin, txpin); //lager et objekt til biblioteket sofwareSerial sånn at vi kan kommunisere med arduinoen

//verdier for å lagre og justere volumet på arduinoen og appen.
int volume = 0; //verdien som kommer via serial porten er volumet som skal vises på skjermen
int lastVolume = 0;

String serialInfo = ""; //lagrer det som leses av serial porten

WiFiServer server(80);

char* ssid = "HilmirNett"; //ssiden til nettverket vi ønsker å koble oss til
char* password = "12345678"; //passordet til nettverket vi ønsker å koble oss til


//skaper et objekt for skjermen og includerer det inbygde biblioteket Wire for i2c kommunikasjon
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//klasse for å hente data fra yr
class WeatherForecast{
  //ofentlige verdier dette er hvor vi lagrer dataene vi henter fra yr
  public:
    String url;
    String temperature1;
    String weather1;
    String temperature2;
    String weather2;
    String timefrom;
    String host = "www.yr.no";

  //private verdier som brukes for å lete gjennom xml filen
  boolean startRead = false;
  int stringPos;
  int stage;
  String tempValue;
  String charValue;
  String dataValue;

  //funksjon som går med en gang nodemcuen starter sånn at vi kan skafe urlen til det stedet vi vil hente informasjon fra.
  WeatherForecast(String url1){
    url = url1;
  }

  //privat funksjon for å sende inn kode for hva slags vær det er. Returnerer en string med hva været er.
  String getWeatherText(String weatherNumber){
    if(weatherNumber=="1"){return  "Sol";}
    if(weatherNumber=="2"){return  "Lett-  skyet";}
    if(weatherNumber=="3"){return  "Delvis skyet";}
    if(weatherNumber=="4"){return  "Skyet";}
    if(weatherNumber=="40"){return "Lette  regnbyger";}
    if(weatherNumber=="5"){return  "Regn-  byger";}
    if(weatherNumber=="41"){return "Kraftige regnbyger";}
    if(weatherNumber=="24"){return "Lette  regnbyger og torden";}
    if(weatherNumber=="6"){return  "Regnbyger og torden";}
    if(weatherNumber=="25"){return "Kraftige regnbyger og torden";}
    if(weatherNumber=="42"){return "Lette   sluddbyger";}
    if(weatherNumber=="7"){return  "Sluddbyger";}
    if(weatherNumber=="43"){return "Kraftige sluddbyger";}
    if(weatherNumber=="26"){return "Lette   sluddbyger og torden";}
    if(weatherNumber=="20"){return "Sluddbyger og torden";}
    if(weatherNumber=="27"){return "Kraftige sluddbyger og torden";}
    if(weatherNumber=="44"){return "Lette   snobyger";}
    if(weatherNumber=="8"){return  "Snobyger";}
    if(weatherNumber=="45"){return "Kraftige snobyger";}
    if(weatherNumber=="28"){return "Lette   snobyger og torden";}
    if(weatherNumber=="21"){return "Snøbyger og torden";}
    if(weatherNumber=="29"){return "Kraftige snobyger og torden";}
    if(weatherNumber=="46"){return "Lett    regn";}
    if(weatherNumber=="9"){return  "Regn";}
    if(weatherNumber=="10"){return "Kraftig regn";}
    if(weatherNumber=="30"){return "Lett    regn og torden";}
    if(weatherNumber=="22"){return "Regn og torden";}
    if(weatherNumber=="11"){return "Kraftig regn og torden";}
    if(weatherNumber=="47"){return "Lett    sludd";}
    if(weatherNumber=="12"){return "Sludd";}
    if(weatherNumber=="48"){return "Kraftig sludd";}
    if(weatherNumber=="31"){return "Lett    sludd og torden";}
    if(weatherNumber=="23"){return "Sludd og torden";}
    if(weatherNumber=="32"){return "Kraftig sludd og torden";}
    if(weatherNumber=="49"){return "Lett sno";}
    if(weatherNumber=="13"){return "Sno";}
    if(weatherNumber=="50"){return "Kraftig sno";}
    if(weatherNumber=="33"){return "Lett sno og torden";}
    if(weatherNumber=="14"){return "Sno og  torden";}
    if(weatherNumber=="34"){return "Kraftig sno og torden";}
    if(weatherNumber=="15"){return "Taake";}
    return "Ikke funnet type "+weatherNumber;
      }



  public:
    //funksjons som kales på i starten av loppen, henter værdata fra yr og oppdatere de offentlige variablene.
    void opperate(){
      while(weather1 == "" && weather2 == "" && temperature1 == ""){//dette er for å sikre at funksjonen faktisk henter verdier fordi jeg kom over en bugg hvor det tok noen forsøk før den hentet verdiene
        yield();
        stringPos = 0; // teller for inkomne tegn i XML
        stage = 0; // Brukes for å holde orden på hvor langt vi er kommet i XMLen
        tempValue = ""; // Midlertidig verdi
        charValue = ""; // Verdi på tegnet som nå ligger i buffer fra XML
        dataValue = ""; // Verdien hentet ut på det vi ønsker oss.
        //reseter verdiene for værinformasjon
        temperature1 = "";
        weather1 = "";
        temperature2 = "";
        weather2 = "";
        timefrom = "";

        //skriver ut til en pc hvor den kobler seg til, for feilsøking når høytaleren er ferdig
        Serial.print("connecting to ");
        Serial.println(host);

        // Bruker WiFiClient til å koble seg til yr via TCP tilkobling. Sender meldig hvis den ikke klarer å koble seg til
        WiFiClient client;
        const int httpPort = 80;
        if (!client.connect(host, httpPort)) {
          Serial.println("connection failed");
          return;
        }

           Serial.println("Laster..");



        Serial.print("Requesting URL: ");
        Serial.println(url);

        //Sender forespørsel til sørveren om å få xml filen
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n");
        delay(2000);

        // Read all the lines of the reply from server and print them to Serial

        //memset( &inString, 0, 32 ); //clear inString memory

        /*leser av xml filen og letter etter værdataen
        ---------------------------------------------
        ---------------------------------------------*/
        while(client.available()){
          yield(); //yield brukes fordi nodemcuen gjør en del bakgrunnsprosseser som har med å behandle tcp og ip tilkoblinger. Disse gjøres i starten av loopen men vis de ikke blir gjort på en stund restarter nodemcuen seg og yield gjør disse funksjonene.

          char c = client.read(); //leser av en bokstav i xml filen

           if (c == '<' ) { // Sjekk om vi har starten på en xml node
              startRead = true; //hvis det er tilfelet må vi starte å se etter de ønskede verdiene.
            }

            /*leser XML fra yr tegn for tegn. Om den finner tegnet < er det starten på en XML-node. Leser så inn XML
            noden inn i en streng. Sjekker så, for hvert tegn, om strengen innholder kombinasjonen av tegn vi ønsker oss. Om den
            finner kombinasjonen av tegn, begynner den å lese ut / lagre funnet data. Bruker så stage til å sjekke hvor vi er i
             xml og tilegner den utleste verdien til en variabel (vær, temperatur, eller tidspunkt.)*/

            // leser inn innhold til en streng
            if(startRead == true){
                charValue = String(c); // konverter til string
                tempValue = tempValue + charValue; // legg til string

                stringPos++;

            // les ut vær-kode: sjekk om XML-strengen inneholder symbol number "numberEx=" og sjekk om vi er forbi tegn 38
                 if(tempValue.indexOf("numberEx=") > 0)
                  {
                     if (c == '"' && dataValue != "=" ) {
                      startRead = false; // slutt å lese
                      tempValue = ""; // nullstill streng
                      stringPos = 0; // Nullstill teller+

                      stage++;

                     }else{
                       if (c == '"' && dataValue == "=" ){
                        dataValue=""; //reseter stringen sånn at den er klar for bruk.
                      }else{
                      dataValue = dataValue + charValue; //legger karakteren som ble lest av denne syklusen til stringen som inneholder innformasjonen
                     }
                     }
                  }


            // les ut temperatur. sjekk om det inneholder temperatur, og sjekk om vi er forbi tegn 35
                if(tempValue.indexOf("temperature") > 0 && stringPos > 35)
                  {
                     if (c == '"' ) {
                      startRead = false; // slutt å lese

                      tempValue = ""; // nullstill streng
                      stringPos = 0; // Nullstill teller+

                      stage++;


                       }else{
                      dataValue = dataValue + charValue;
                     }
                  }

               if(tempValue.indexOf("time from=") > 0 && stringPos > 23 && weather1!="" && timefrom=="")
                  {
                     if (stringPos == 26 ) {
                      startRead = false; // slutt å lese

                      tempValue = ""; // nullstill streng
                      stringPos = 0; // Nullstill teller+

                      stage++;


                       }else{
                      dataValue = dataValue + charValue;
                     }
                  }

              if (c == '>' ) { // ferdig med en xml-node
                startRead = false; // slutt å lese
                tempValue = ""; // nullstill streng
                stringPos = 0; // Nullstill teller
              }


            //lagrer informasjonen som er i stringen i de riktige variablene.
            if(stage==1 && weather1==""){
               weather1=getWeatherText(dataValue);
               dataValue="";
              }else if(stage==2 && temperature1==""){
               temperature1=dataValue;
               dataValue="";
              }else if(stage==3 && timefrom==""){
               timefrom="Fra kl "+dataValue+":00";
               dataValue="";
              }else if(stage==4 && weather2==""){
               weather2=getWeatherText(dataValue);
               dataValue="";
              }else if(stage==5 && temperature2==""){
               temperature2=dataValue;
               dataValue="";
               //Serial.println("All done!: "+stage);
               break;
              }


            }

          // end
        }
        Serial.println("closing connection");
    }};
};


WeatherForecast weatherForecast("/place/Norge/Buskerud/Drammen/Drammen_videregående_skole/varsel.xml"); //lager et objekt til classen weatherForecast med 1 argument som er urlen til der vi ønsker å hente værdata fra


void setup() {
  Serial.begin(9600); //for feilmeldinger
  softSerial.begin(9600); //for komunikasjon med arduinoen

  //kobler til wifi
  if (WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){
      Serial.println("not connected to wifi");
      yield();} //sikrer at man ikke går videre med koden før wifi er koblet til
    }

  //gjør klar skjermen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  //starter sørveren
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());

  /*viser ip addressen på skjermen helt til ultralydsensoren registrer at hånden er over sensoren
  ----------------------------------------
  ----------------------------------------*/
  boolean showIpAddress = true;
  while (showIpAddress){
    if (softSerial.available()>0){
      serialInfo = softSerial.readStringUntil('\n'); //leser av serial informasjon man får fra arduinoen
    }
    if (serialInfo.indexOf("stop ip") != -1){ //venter til man får beskjed fra arduinoen om at noe har vært over ultralydsensoren før man går videre med koden
      showIpAddress = false;
    }

    //legge til kode for å stoppe det hvis man kobler til appen
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //ta med: softSerial.println("appConnected");

    //gjør klar skjermen for at man kan skrive på den
    display.clearDisplay(); //fjerner all informasjon på skjermen
    display.setTextSize(2);      // setter størelsen på teksten
    display.setTextColor(WHITE); // spesifiserer fargen på teksten
    display.setCursor(0, 0);     //sier hvor man skal starte å skrive teksten
    display.cp437(true);         //spesifiserer fonten

    //skriver ut ip adressen på skjermen
    display.println("ip");
    display.println(WiFi.localIP());
    display.display(); //oppdaterer innholdet på skjermen
  }
  display.clearDisplay();
  display.display();
}

void loop() {

  weatherForecast.opperate(); //henter værdata fra yr.

  //Sjekker om en client har kobla til nodemcuen
  WiFiClient client = server.available();
  if (!client) {
    return;
  }else{
    Serial.println("YES");
  }
  softSerial.println("appConnected");
  Serial.println("appConnected");

  //leser av informasjonen som er sent fra arduinoen.
  if (softSerial.available()>0){
    serialInfo = softSerial.readStringUntil('\n');
  }

  //sjekker om informasjonen som ble sent er en volum oppdatering og oppdaterer volumet
  if (serialInfo.indexOf("volume:")!= -1){
  volume = serialInfo.substring(7 + serialInfo.indexOf("volume:")).toInt();
  lastVolume = volume;
  /*putt in kode for å oppdatere  volumet til mobilen
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  putt in kode for å oppdatere  volumet til mobilen*/
  }

  // Venter til at klienten ha send noe data
  while(!client.available()){
    delay(1);
  }

  String req = client.readStringUntil('\r');
  client.flush();

  // Ser på dataen og sender de riktige kommandoene
  Serial.println("Music Start: " + req.indexOf("/cmd/MUSIC_START"));
  Serial.println("Music Stop: " + req.indexOf("/cmd/MUSIC_STOP"));
  Serial.println("Music next: " + req.indexOf("/cmd/MUSIC_NEXT"));
  Serial.println("Music start: " + req.indexOf("/cmd/MUSIC_PREVIOUS"));

  if (req.indexOf("/cmd/MUSIC_START") != -1){
    softSerial.println("play");}

  else if (req.indexOf("/cmd/MUSIC_STOP") != -1){
    softSerial.println("pause");}
  else if(req.indexOf("/cmd/MUSIC_NEXT") != -1){
    softSerial.println("next song");
    Serial.println("Nxt song");
  }
  else if(req.indexOf("/cmd/MUSIC_PREVIOUS") != -1){
    softSerial.println("last song");
    Serial.println("last nong");
  }
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // printer ut svar
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");

  //oppdaterer volumet til arduinoen hvis volumet er endret
  if (lastVolume != volume){
    softSerial.println(volume);
    lastVolume = volume;}



  //skriver ut værdata til skjermen
  display.clearDisplay(); //fjerner all informasjon på skjermen
  display.setTextSize(2);      // størelsen på teksten
  display.setTextColor(WHITE); // skriver hvit tekst
  display.setCursor(0, 0);     // starter i øverste venstre hjørne
  display.cp437(true);  //spesifisererer fonten
  display.print(F("grader:"));
  display.println(weatherForecast.temperature1);
  display.println(weatherForecast.weather1);
  display.print(F("volume: "));
  display.print(volume);
  display.display(); //oppdaterer skjermen

}
