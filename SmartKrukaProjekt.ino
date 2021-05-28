/**
 * @version 1.0
 * @author Alex Nilsson, Rasmus Mathiasson, Alex Hultman
 */
#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <dht.h>

dht DHT;

#define DHT11_PIN 7
  int ledpin = 11;
  int sensorPin = A1; 
  int soilHumidity = 0; // nollställer varje gång
  int sensorVCC = 13;
  int waterPump = 6;
  const int dry = 600;
  

const char kHostname[] = "84.217.9.249";
const char kPath[] = "/data/00000001a033f6d3";

byte mac[] = { 0xDA, 0xED, 0xBB, 0xFF, 0xBE, 0xAC };


const int kNetworkTimeout = 30*1000;

const int kNetworkDelay = 1000;

void initWater() {
 
 while (!Serial);
 pinMode(waterPump, OUTPUT);
 digitalWrite(waterPump, LOW);// 
}

int getWater() {
 soilHumidity = analogRead(sensorPin);
 Serial.println(soilHumidity);
 if (soilHumidity >= dry) {
    Serial.println("Watering starts now..soil humidity is " + String(soilHumidity));
 digitalWrite(waterPump, HIGH); // pump1 activated
 delay(2000); //Hur länge vatten ska rinna
 digitalWrite(waterPump, LOW); // pump1 deactivated
 Serial.println("Done watering.");
 
 }
 else {
    Serial.println("Soil humidity is wet enough. No water needed " + String(soilHumidity));
  }
  delay(5000);
  //Paus mellan varje vattning
}

void initSoilHumidity() {
 pinMode(sensorVCC, OUTPUT);
 digitalWrite(sensorVCC, LOW);
}
int getSoilHumidity() {
 digitalWrite(sensorVCC, HIGH); // spänning till sensorn
 delay(100); // kollar så sensorn är på
 soilHumidity = analogRead(sensorPin); // läser värdet
 digitalWrite(sensorVCC, LOW); // stannar

return soilHumidity;
}

void setup()
{

  Serial.begin(9600); 
  pinMode(ledpin, OUTPUT);
  initSoilHumidity();
  initWater();

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(1000);
  }  
}

void loop()
{
  int err =0;
  int chk = DHT.read11(DHT11_PIN);
  int val;
  
  char buf[100];

  
  getSoilHumidity();
  int humidity = DHT.temperature;
  int temperature = DHT.humidity;
  int content_length = sprintf(buf, "{\"temperature\":\"%d\",\"humidity\":\"%d\",\"soil humidity\":\"%d\"}", humidity, temperature, soilHumidity);
  
  delay(1000);
  
  EthernetClient c;
  HttpClient http(c);
  http.beginRequest();
  err = http.post(kHostname, kPath);
  http.sendHeader("Content-Length", content_length);
  http.write((const uint8_t *) buf, content_length);
  http.flush();
  http.endRequest();
   
  if (err == 0)
  {
    Serial.println("startedRequest ok");
   
    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        
        unsigned long timeoutStart = millis();
        char c;
        
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                
                Serial.print(c);
               
                bodyLen--;
              
                timeoutStart = millis();
            }
            else
            {
               
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  
  getWater();
  
  http.stop();
  //stannar ifall följande inte uppfyller krav
}
