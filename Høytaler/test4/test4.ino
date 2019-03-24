#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/*for å koble til nettverket
WeatherForecast(String url1, char* ssid, char* password){
  url = url1;
  //kobler til wifi
  while (WiFi.status() != WL_CONNECTED){} //sikrer at man ikke går videre med koden før wifi er koblet til
  WiFi.begin(ssid, password);}*/

class WeatherForecast{
  public:
    String url;
    String temperature1;
    String weather1;
    String temperature2;
    String weather2;
    String timefrom;
    String host = "www.yr.no";

  boolean startRead = false;
  int stringPos;
  int stage;
  String tempValue;
  String charValue;
  String dataValue;

  WeatherForecast(String url1){
    url = url1;
  }

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
    void opperate(){
      while(weather1 == "" && weather2 == "" && temperature1 == ""){//dette er for å sikre at funksjonen faktisk henter verdier fordi jeg kom over en bugg hvor det tok noen forsøk før den hentet verdiene
        yield();
        stringPos = 0; // teller for inkomne tegn i XML
        stage = 0; // Brukes for å holde orden på hvor langt vi er kommet i XMLen
        tempValue = ""; // Midlertidig verdi
        charValue = ""; // Verdi på tegnet som nå ligger i buffer fra XML
        dataValue = ""; // Verdien hentet ut på det vi ønsker oss.
        temperature1 = "";
        weather1 = "";
        temperature2 = "";
        weather2 = "";
        timefrom = "";

        Serial.print("connecting to ");
        Serial.println(host);

        // Use WiFiClient class to create TCP connections
        WiFiClient client;
        const int httpPort = 80;
        if (!client.connect(host, httpPort)) {
          Serial.println("connection failed");
          return;
        }

           Serial.println("Laster..");



        Serial.print("Requesting URL: ");
        Serial.println(url);

        // This will send the request to the server
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n");
        delay(2000);

        // Read all the lines of the reply from server and print them to Serial

        //memset( &inString, 0, 32 ); //clear inString memory

        while(client.available()){
          yield(); //yield er for å forhindre DVM problemer som er at en klokke må oppdateres. Dette går automatisk når loop går runden men ikke inni loopen så på grunn av at while kan ta litt tid bør man bruke denne for å ungå kresjer

          //String line = client.readStringUntil('\r');
          //Serial.print(line);



          char c = client.read();
          //displayInfo(c);



           if (c == '<' ) { // Sjekk om vi har starten på en xml node
              startRead = true; // i såfall, la oss lese inn
            }

            //Kort forklart - leser XML fra yr tegn for tegn. Om den finner tegnet < er det starten på en XML-node. Leser så inn XML
            //noden inn i en streng. Sjekker så, for hvert tegn, om strengen innholder kombinasjonen av tegn vi ønsker oss. Om den
            // finner kombinasjonen av tegn, begynner den å lese ut / lagre funnet data. Bruker så stage til å sjekke hvor vi er i
            // xml og tilegner den utleste verdien til en variabel (vær, temperatur, eller tidspunkt.)

            // leser inn innhold til en streng
            if(startRead == true){
                charValue = String(c); // konverter til string
                tempValue = tempValue + charValue; // legg til string

                stringPos++;

            // les ut vær-kode: sjekk om XML-strengen inneholder symbol number "numberEx=" og sjekk om vi er forbi tegn 38
                 if(tempValue.indexOf("numberEx=") > 0)
                  {
                     if (c == '"' && dataValue != "=" ) {
                      //Serial.println(tempValue);
                      //Serial.println("Fant ver");
                      startRead = false; // slutt å lese
                      //Serial.println(dataValue); // skriv ut linjen til skjerm
                      tempValue = ""; // nullstill streng
                      stringPos = 0; // Nullstill teller+

                      stage++;

                     }else{

                       if (c == '"' && dataValue == "=" ){
                        dataValue="";
                      }else{

                      dataValue = dataValue + charValue;
                     }
                     }
                  }


            // les ut temperatur. sjekk om det inneholder temperatur, og sjekk om vi er forbi tegn 35
                if(tempValue.indexOf("temperature") > 0 && stringPos > 35)
                  {
                     if (c == '"' ) {
                      //Serial.println("Fant grader");
                      startRead = false; // slutt å lese
                      //Serial.println("dataValue: "+dataValue); // skriv ut linjen til skjerm

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
                      //Serial.println("Fant tidsinterval");
                      startRead = false; // slutt å lese
                      //Serial.println("dataValue: "+dataValue); // skriv ut linjen til skjerm

                      tempValue = ""; // nullstill streng
                      stringPos = 0; // Nullstill teller+

                      stage++;


                       }else{
                      dataValue = dataValue + charValue;
                     }
                  }

              if (c == '>' ) { // ferdig med en xml-node
                startRead = false; // slutt å lese
                //Serial.println(tempValue); // skriv ut linjen til konsoll
                tempValue = ""; // nullstill streng
                stringPos = 0; // Nullstill teller
              }

              //Stages


            if(stage==1 && weather1==""){
               weather1=getWeatherText(dataValue);
               Serial.println("weather 1 innside" + weather1);
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

WeatherForecast weatherForecast("/place/Norway/Oslo/Oslo/Kværnerveien/varsel.xml");

void setup(){Serial.begin(9600);}

void loop(){
  weatherForecast.opperate();
  Serial.println("weather1" + weatherForecast.weather1);
  delay(10000);
}
