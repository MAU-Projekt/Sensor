#include
#include
#include

byte mac[] = { 0x00, 0xAB, 0xBC, 0xCC, 0xDE, 0x01 };
EthernetClient client;

#define DHTPIN 2 // pin till sensor
#define DHTTYPE DHT11 //typ
DHT dht(DHTPIN, DHTTYPE); //variabel för avläsning

//int light = 0; lux
int temp = 0; //grader celcius
int humidity = 0// procent i luft

void setup() {
    Serial.begin(9600); //överföring via USB med hastighet
    pinMode(/*pin*/, INPUT); //görs till output
  if(Ethernet.begin(mac)== 0){
    Serial.println("Misslyckad Ethernet-konfiguration");
  }
  dht.begin();
  delay(10000); 
}

int k(){
   if(digitalRead(/*pin*/)){
        return 0;
       }else{
        return 1; 
       } 
       
}
int m(){
   if(analogRead(A0)<1022){ //mäter spänningen, inte större än 1022 units, 5volt är ca 1024
        return 1;
       }else{
        return 0; 
       } 

void loop() {
  temp =(int) dht.readTemperature();
  humidity = (int) dht.readHumidity();
  
  Serial.println(analogRead(A0)); //skriver ut analoga signalen
  String a = String(k());
  String b = String(m());
  String data = "Temperatur=";
  data.concat(temp);
  data.concat("&Luftfuktighet=");
  data.concat(humidity);
/*data.concat("&Illuminans=");
  data.concat(light);*/
  data.concat("&Digitalread=");
  data.concat(a);
  data.concat("&Analogread=");
  data.concat(b);

if(client.connect(".....", 5000){ //server address
//Ändra info
client.println("POST /....../....  HTTP/1.1");
client.println("Host: ");
client.println("Content-Type: application/x-www-form-urlencoded");
client.println("Connection:close");
client.print("Content-Length:");
client.println(data.length);
client.println();
client.println(data);
client.flush();
client.stop();

delay(12000) //vänta innan data sänds igen
}
