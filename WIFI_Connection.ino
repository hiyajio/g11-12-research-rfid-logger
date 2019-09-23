/*Project: Development of an NFC-based Attendance and Location
               Logging System Using an Online Scheduling
                    Database for PSHS-MC Batch 2018
  Mechanic: Juan Sergio A. Buenviaje
  System Programmer: Juan Sergio A. Buenviaje
  Database Programmer: Joshua Kyle C. Kim
  Statistical Analyst: Francis Anton S. Garcia*/
//Running on WIFI Module
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial link(12,14); //12 rx, 14 tx

const char* ssid     = "Vici";
const char* password = "unnecessarY";
 
const char* host = "192.168.43.98";

//ESP8266 wifi(link); 

char buff[8];
char c;
byte i; //counter for string reading
byte j = 0; //counter for string output
int stationID = 1;
WiFiClient client;
 
void setup() {
  Serial.begin(115200);
  link.begin(115200);
 
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
 
  Serial.println(F(""));
  Serial.println(F("WiFi connected"));  
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
 
  Serial.print(F("Connecting to "));
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println(F("Connection failed"));
    return;
  }
}

void loop() {
  if(link.available()){
      Serial.println(F("Link available"));
      i = 0;
      c = link.read();
      while(c != '\n' && i < 8)
      {
        buff[i] = c;
        i++; 
        c = link.read();
        Serial.println(F("Reading...")); 
        yield();
      }
      buff[i] = '\0';
      if (c == '\n') {
        feed(); 
      }
  }    
  yield();
}

void feed() {
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println(F("Connection failed"));
    return;
  }
  String url = "/index.php/attendance_update/old_add_attendance/";
  Serial.print(F("Requesting URL: "));
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + String(stationID) + "/" + buff + 
               " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  Serial.println(buff);
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
  
  // Read all the lines of the reply from server and print them to Serial
  client.stop();
  delay(500);
}
