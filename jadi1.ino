#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>


#define ONE_WIRE_BUS 4                          //D2 pin of nodemcu
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);            // Pass the oneWire reference to Dallas Temperature.

const char* ssid = "1";
const char* password = "haziq123";
const char* host = "ntah1.000webhostapp.com";

float calibration = -3.60; //change this value to calibrate
const int analogInPin = A0; 
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],tmp;

void setup() {
  Serial.begin(115200);
  delay(100);
  sensors.begin();
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}

void loop() {

  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (isnan(t)) {    
    Serial.println("Failed to read from temp sensor!");
    return;
  }

for(int i=0;i<10;i++) 
 { 
 buf[i]=analogRead(analogInPin);
 delay(30);
 }
 for(int i=0;i<9;i++)
 {
 for(int j=i+1;j<10;j++)
 {
 if(buf[i]>buf[j])
 {
 tmp=buf[i];
 buf[i]=buf[j];
 buf[j]=tmp;
 }
 }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float pHVol=(float)avgValue*5.0/1024/6;
 float phValue = 3.50 * pHVol + calibration;
 float p = phValue;
  if (isnan(p)) {    
    Serial.println("Failed to read from pH sensor!");
    return;
  }

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  String url = "/api/insert.php?temp=" + String(t) + "&ph="+ String(p);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  delay(30000);
}
