#include <ESP8266WiFi.h>
#include <ESPiLight.h>
#include <fauxmoESP.h>
#include <ESP8266mDNS.h>

#include "credentials.h"

fauxmoESP alexawifi;

#define TRANSMITTER_PIN 0

ESPiLight rf(TRANSMITTER_PIN);

//////////////////////////// Ny server paa port 80
WiFiServer server(80);    //
WiFiClient client;        //
                          //
String webseite = "";     // Her bliver HTML- Siden for knapperne gemt
int webID = 0;            // Til referere til funktionsforespoersel
const char * dName;       //
bool webStatus = false;   //
////////////////////////////

void wifiSetup() {

// -----------------------------------------------------------------------------
// WLAN SETUP
// -----------------------------------------------------------------------------

    WiFi.mode(WIFI_STA);

    Serial.printf("Forbindelsen til %s bliver lavet. ", WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(">");
        delay(100);
    }
    Serial.println("");
    Serial.printf("Forbundet! SSID: %s, IP Adresse: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void anfrage(uint8_t device_id, const char * device_name, bool state) {
  Serial.print("Gerät: "); Serial.println(device_name);
  Serial.print("Status: ");

  if (state) {

    if(device_id == 0) { //Pavoni
    Serial.println("ON");
    rf.send("nexa_switch", "{\"unit\":3,\"id\":15,on\":1}");
    delay(2000);
    }

    else if(device_id == 1) { //Bose Station
    Serial.println("ON");
    rf.send("quigg_gt1000", "{\"unit\":0,\"id\":15,\"on\":1}");
    delay(2000);
    }

    else if(device_id == 2) { //Onkyo
    Serial.println("ON");
    rf.send("quigg_gt1000", "{\"unit\":3,\"id\":5,\"on\":1}");
    delay(2000);
    }

    else if(device_id == 3) { //IQ Floor Light
    Serial.println("ON");
    rf.send("quigg_gt1000", "{\"unit\":0,\"id\":0,\"on\":1}");
    delay(2000);
    }

  }

  else {

        if(device_id == 0) { //Pavoni
    Serial.println("OFF");
    rf.send("nexa_switch", "{\"unit\":3,\"id\":15,off\":1}");
    delay(2000);
    }

    else if(device_id == 1) { //Bose Station
    Serial.println("OFF");
    rf.send("quigg_gt1000", "{\"unit\":0,\"id\":15,\"off\":1}");
    delay(2000);
    }

    else if(device_id == 2) { //Onkyo
    Serial.println("OFF");
    rf.send("quigg_gt1000", "{\"unit\":3,\"id\":5,\"off\":1}");
    delay(2000);
    }

    else if(device_id == 3) { //IQ Floor Light
    Serial.println("OFF");
    rf.send("quigg_gt1000", "{\"unit\":0,\"id\":0,\"off\":1}");
    delay(2000);
    }

  }
}


void pluge(){

    //433MHZ

    alexawifi.addDevice("Pavoni"); //ID 0
    alexawifi.addDevice("Bose Station");//ID 1
    alexawifi.addDevice("Onkyo"); //ID 2
    alexawifi.addDevice("IQ Floor Light"); //ID 3

    alexawifi.onMessage(anfrage);
}


void setup() {

    Serial.begin(115200);
    Serial.println("Når forbindelsen er oprettet, sig 'Alexa, turn on <plug>' eller turn off");

    wifiSetup();
    pluge();
//////////////////////
    server.begin(); //Start serveren
/////////////////////
}



void loop() {

  alexawifi.handle();

//  Serial.println("Vent paa Alexa eller Webrequest");
//  delay(500); //Debug

////////////////////////////////////////////////////////
                                                      //
  WiFiClient client = server.available();             //
                                                      //
  if (client){                                        //
                                                      //
  unsigned long timeout = millis()+250;               //
  while(!client.available() && (millis()<timeout)){   //
    delay(1);                                         //
//  Serial.println ("In Schleife");                   //
  }                                                   //
                                                      //
  if(millis()>timeout){                               //
 // Serial.println ("Time Out");                      //
    client.stop();                                    //
    return;                                           //
  }                                                   //
                                                      //                                                   //
  String request = client.readStringUntil('\r');      //
                                                      //
  client.flush();                                     //
  delay(10);                                          //
                                                      //
  if(request==""){                                    //
  client.stop();                                      //
  Serial.println("TOM");                             //
  return;                                             //
  }                                                   //
                                                      //
  client.println("HTTP/1.1 200 OK");                  //
  client.println("Content-Type: text/html");          //
  client.println("");                                 //
  client.println("<!DOCTYPE HTML>");                  //
  client.println("<html>");                           //
                                                      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  webseite += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";                                                            //
  webseite += "<link rel=\"apple-touch-icon\" href=\"\" />";                                                                //
  webseite += "<center><h1>Mine RF stikkontakter</h1>";                                                                                                                             //
                                                                                                                                                                                // Button anlegen fuer Gerät 1
  webseite += "<p>Pavoni <a href=\"plug=0on\"><button>On</button></a>&nbsp;<a href=\"plug=0off\"><button>Off</button></a></p>";                                              // Button anlegen fuer Gerät 2
  webseite += "<p>Bose Station <a href=\"plug=1on\"><button>On</button></a>&nbsp;<a href=\"plug=1off\"><button>Off</button></a></p>";                                                 //
  webseite += "<p>Onkyo <a href=\"plug=2on\"><button>On</button></a>&nbsp;<a href=\"plug=2off\"><button>Off</button></a></p>";                                              // Button anlegen fuer Gerät 2
  webseite += "<p>IQ Floor Light <a href=\"plug=3on\"><button>On</button></a>&nbsp;<a href=\"plug=3off\"><button>Off</button></a></p>";                                                 //
                                                                                                                                                                                //

                                                                                                                                                                                //
  client.print(webseite);                                                                                                                                                       // Webseite/ Button anzeigen
                                                                                                                                                                                //
                                                                                                                                                                                //
  webseite = "";                                                                                                                                                                // Speicher löschen
  delay(10);                                                                                                                                                                    //
                                                                                                                                                                                //
  if (request.indexOf("/plug=0on") != -1)  {                                                                                                                                  // Anfrage prüfen für AN
  webID = 0;                                                                                                                                                                    //
  dName = "Pavoni";                                                                                                                                                              //
  webStatus = true;                                                                                                                                                             //
  anfrage(webID, dName, webStatus);                                                                                                                                             // Anfrage absenden
  }                                                                                                                                                                             //
                                                                                                                                                                                //
  if (request.indexOf("/plug=0off") != -1)  {                                                                                                                                 // Anfrage prüfen für AUS
  webID = 0;                                                                                                                                                                    //
  dName = "Pavoni";                                                                                                                                                              //
  webStatus = false;                                                                                                                                                            //
  anfrage(webID, dName, webStatus);                                                                                                                                             //
  }                                                                                                                                                                             //
                                                                                                                                                                                //
  if (request.indexOf("/plug=1on") != -1)  {                                                                                                                                  // Anfrage prüfen für AN
  webID = 0;                                                                                                                                                                    //
  dName = "Bose Station";                                                                                                                                                              //
  webStatus = true;                                                                                                                                                             //
  anfrage(webID, dName, webStatus);                                                                                                                                             // Anfrage absenden
  }                                                                                                                                                                             //
                                                                                                                                                                                //
  if (request.indexOf("/plug=1off") != -1)  {                                                                                                                                 // Anfrage prüfen für AUS
  webID = 0;                                                                                                                                                                    //
  dName = "Bose Station";                                                                                                                                                              //
  webStatus = false;                                                                                                                                                            //
  anfrage(webID, dName, webStatus);                                                                                                                                             //
  }                                                                                                                                                                             //
                                                                                                                                                                                //
  if (request.indexOf("/plug=2on") != -1)  {                                                                                                                                  // Anfrage prüfen für AN
  webID = 2;                                                                                                                                                                    //
  dName = "Onkyo";                                                                                                                                                          //
  webStatus = true;                                                                                                                                                             //
  anfrage(webID, dName, webStatus);                                                                                                                                             // Anfrage absenden
  }                                                                                                                                                                             //
                                                                                                                                                                                //
  if (request.indexOf("/plug=2off") != -1)  {                                                                                                                                 // Anfrage prüfen für AUS
  webID = 2;                                                                                                                                                                    //
  dName = "Onkyo";                                                                                                                                                          //
  webStatus = false;                                                                                                                                                            //
  anfrage(webID, dName, webStatus);                                                                                                                                             //
  }                                                                                                                                                                             //
                                                                                                                                                                              //
  if (request.indexOf("/plug=3on") != -1)  {                                                                                                                                  // Anfrage prüfen für AN
  webID = 3;                                                                                                                                                                    //
  dName = "IQ Floor Light";                                                                                                                                                            //
  webStatus = true;                                                                                                                                                             //
  anfrage(webID, dName, webStatus);                                                                                                                                             // Anfrage absenden
  }                                                                                                                                                                             //
                                                                                                                                                                                //
  if (request.indexOf("/plug=3off") != -1)  {                                                                                                                                 // Anfrage prüfen für AUS
  webID = 3;                                                                                                                                                                    //
  dName = "IQ Floor Light";                                                                                                                                                          //
  webStatus = false;                                                                                                                                                            //
  anfrage(webID, dName, webStatus);                                                                                                                                             //
  }                                                                                                                                                                             //
                                                                                                                                                                                //
  delay(1);                                                                                                                                                                     //
  client.stop();                                                                                                                                                                //
                                                                                                                                                                                //
Serial.println("Forbindelsen afbrudt");                                                                                                                                           //
delay(500); //Debug                                                                                                                                                             //
                                                                                                                                                                                //
 }                                                                                                                                                                              // Anfrage absenden
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
