// 2018-01-16
// Volker Pinkitz
// volker.pinkitz@gmail.com

// this code transfers hard coded temperatur values from a NodeMCU V2 Amica (ESP8266) via HTTPS into the SAP Cloud Platform
// via iotmms Service. Device, Device Type and Message Type have to be preconfigured within the HANA Cloud Cockpit.


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>

// ========== start configuration ==========
// WiFi configuration
const char* ssid = "3neo_2.4Ghz_DF62";
const char* password = "CZ7m32HVB8";

// SAP HCP specific configuration
const char* host = "iotmmsp1710121069trial.hanatrial.ondemand.com";
String device_id = "d7a54249-89b3-451f-8fa3-de1f270bf4b5";
String message_type_id = "dbbf2042bb0e967a3c64";
String oauth_token="742b81c7e448a824657b7254477d45b5";  // !!! klick button Generate Token (Device)

//fixed values but you directly read from a sensor
String temperatureinC="22.0";
String temperatureinF="70.01";
String humidityVal="45.00";
String pressureVal="955.000";
String dateTime = "";
String TimeDate = "";
String Date ="";

//NTP Time server
const char* NTPhost = "1.it.pool.ntp.org";

// ========== end configuration ============

String url = "https://iotmmsp1710121069trial.hanatrial.ondemand.com/com.sap.iotservices.mms/v1/api/http/data/" + device_id;

String post_payload = "{\"mode\":\"async\",\"messageType\":\"" + message_type_id + "\",\"messages\":[{\"timestamp\":1516911876,\"temperatureC\":\"" + temperatureinC + "\",\"temperatureF\":\"" + temperatureinF + "\",\"pressure\":\"" + pressureVal + "\",\"humidty\":\"" + humidityVal + "\"}]}";

const int httpsPort = 443; //HTTP port

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print(post_payload);
  Serial.println(post_payload);
  Wire.begin(9);
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Use WiFiClient class to create TCP connections
  WiFiClient Client;
  const int httpPort = 13;
  //Connect to NTP Server
  if (!Client.connect(NTPhost, httpPort)) {
    Serial.println("connection failed to NTP");
   // return;
  }

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
//NTP
  // Read all the lines of the reply from server and print them to Serial
  // expected line is like : Date: Thu, 01 Jan 2015 22:00:14 GMT
  char buffer[12];
  String dateTime = "";
  
 while(Client.available())
  {
    String line = Client.readStringUntil('\r');

    if (line.indexOf("Date") != -1)
    {
      Serial.print("=====>");
    } else
    {
      // Serial.print(line);
      // date starts at pos 7
      Date= line.substring (7);
      Serial.println(Date);
      TimeDate = line.substring(7);
      Serial.println(TimeDate);
      // time starts at pos 14
      TimeDate = line.substring(7, 15);
      TimeDate.toCharArray(buffer, 10);
      //sendStrXY("UTC Date/Time:", 4, 0);
      //sendStrXY(buffer, 5, 0);
      TimeDate = line.substring(16, 24);
      TimeDate.toCharArray(buffer, 10);
      Serial.println(TimeDate);
      //sendStrXY(buffer, 6, 0);
    }
  }
 //END NTP 
  Serial.print("requesting URL: ");
  Serial.println(url);
  
  // using HTTP/1.0 enforces a non-chunked response
  client.print(String("POST ") + url + " HTTP/1.0\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/json;charset=utf-8\r\n" +
               "Authorization: Bearer " + oauth_token + "\r\n" +
               "Content-Length: " + post_payload.length() + "\r\n\r\n" +
               post_payload + "\r\n\r\n");
               
  Serial.println("request sent");

  Serial.println("reply was:");
  Serial.println("==========");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
  Serial.println("==========");
  Serial.println("closing connection");
}

void loop() {
}

