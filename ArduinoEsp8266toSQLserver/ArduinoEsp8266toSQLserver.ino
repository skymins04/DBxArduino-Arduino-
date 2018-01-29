#include <SoftwareSerial.h>
#include <DHT.h>

SoftwareSerial esp(2,3); //RX, TX
DHT dht(4, 11); //DHT11 sensor

#define _SSID ""
#define _PSWD ""
#define _HOST ""
#define _PORT ""

#define TEMP dht.readTemperature();
#define TEMPf dht.readTemperature(true); 
#define HUM dht.readHumidity();

String cmd = " ";
boolean _check;

void setup() {
  Serial.begin(9600);
  Serial.println("HW Serial open");
  esp.begin(9600);
  Serial.println("SW Serial open");

  Serial.println("ESP8266 reset");
  esp.println("AT+RST");
  
  Serial.print("connect WiFi... ");
  _check = false;
  for(int i = 0; i < 10; i++) {
    if(connectWiFi()) {
      Serial.println("succes"); 
      _check = true;
      break;  
    }
  }
  if(!_check) {
    Serial.println("fail");
    while(1);  
  }
re:
  delay(10);
  Serial.print("try connect server... ");
  esp.print("AT+CIPSTART=\"");
  esp.print("TCP");
  esp.print("\",\"");
  esp.print(_HOST);
  esp.print("\",");
  esp.println(_PORT);
  if(esp.find("OK")) Serial.println("succes");  
  else {
    Serial.println("fail");
    rxEmpty();
    goto re;  
  }
}

void loop() {
  String C = String((int)dht.readTemperature());
  String F = String((int)dht.readTemperature(true));
  String H = String((int)dht.readHumidity());
  String sendValue = C + "," + F + "," + H; 
  cmd = "AT+CIPSEND=";
  cmd += String(sendValue.length());
  Serial.println(cmd);
  esp.println(cmd);
  delay(5);
  if(esp.find(">")) {
    Serial.println("send OK");
  } else {
    Serial.println("responeTime over"); 
    return; 
  }
  esp.println(sendValue);
  delay(5000);
}

boolean connectWiFi() {
  cmd = "AT+CWJAP=\""+String(_SSID)+"\",\""+String(_PSWD)+"\"";
  esp.println(cmd);
  delay(3000);
  if(esp.find("OK")) return true;
  return false;
}

void rxEmpty() {
  while(esp.available() > 0) {
    esp.read();  
  }  
}
