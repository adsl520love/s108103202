#include <SPI.h>
#include <Wire.h>  // Arduino IDE 內建
#include <dht.h>     
#define dht_dpin A8 	           //�w�q�T���n�qPin A0 �i��  
dht DHT;   
                     
#define DEBUG true
// Wi-Fi接入點的SSID和密碼
const  char *  ssid  =  "AIOT";
const  char *  password  =  "0277388000";

const char* host = "120.96.57.61";
const int httpsPort = 443;
const int httpPort = 80;

bool F = false;
String sendData(String command, const int timeout, boolean debug){
    String response = "";
    Serial1.print(command); // send the read character to the esp8266
    long int time = millis();
    while( (time+timeout) > millis()){
      while(Serial1.available()){
        // The esp has data so display its output to the serial window 
        char c = Serial1.read(); // read the next character.
        response+=c;
      }  
    }
    if(DEBUG)    {
      Serial.print(response);
    }
    return response;
}

void setup(){  
 
  Serial1.begin(115200);//RX1 TX1，即 Pin18 及 Pin19 
  Serial.begin(115200);
  while (!Serial) ;       // 等待序列埠開啟完成
  
  sendData("AT+RST\r\n",2000,DEBUG);
  sendData("AT+CWMODE=3\r\n",200,DEBUG);
  String commend = "AT+CWJAP=\"";
  commend += ssid;
  commend += "\",\"";
  commend += password;
  commend += "\"\r\n";
  if (sendData(commend,5000,DEBUG)){  
    Serial.print("Join AP success\r\n");
    Serial.print("IP:");
    sendData("AT+CIFSR\r\n",5000,DEBUG);
    sendData("AT+CIPSERVER=0\r\n",200,DEBUG); 
    sendData("AT+CIPMUX=0\r\n",200,DEBUG);
    sendData("AT+CIFSR\r\n",5000,DEBUG);    
  }
}
void loop(){   
 
  String commend = "AT+CIPSTART=\"TCP\",\"";
  commend += host;
  commend += "\",";
  commend += httpPort; 
  commend += "\r\n";
  sendData(commend,1000,DEBUG);
  
  DHT.read11(dht_dpin); 
  String GET2 = "GET /db_insert.php?ID=123456&name=施勢帆&temp=";
  GET2 += DHT.temperature;
  GET2 += "&hum=";
  GET2 += DHT.humidity;
  GET2 += " HTTP/1.1\r\nHost:";
  GET2 += host;
  GET2 += "\r\n\r\n";
  Serial.println(GET2);
  String cipSend = "AT+CIPSEND=";
  cipSend += GET2.length();
  cipSend += "\r\n";
  sendData(cipSend,500,DEBUG);
  sendData(GET2,500,DEBUG);
  delay(200);
  sendData("AT+CIPCLOSE\r\n",500,DEBUG);
  delay(5000);   
  //sendData("AT+CIFSR\r\n",5000,DEBUG); 
} 
