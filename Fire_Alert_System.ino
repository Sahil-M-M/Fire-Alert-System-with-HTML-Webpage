/*
Auther: Sahil Mahendra Mangaonkar
Description: This program reads data from various sensors and display it onto an HTML webpage.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h> 
#include "DHT.h" 

//Pin declarations
#define FlameSensorPin D0       // Digital pin connected to the Flame sensor
#define DHTPin D1               // Digital pin connected to the DHT sensor
#define BuzzerPin D2            // Digital pin connected to the Buzzer
#define SmoakeSensorPin A0      // Analog pin connected to the Smoke gas sensor
#define DHTTYPE DHT11           // DHT 11/22

ESP8266WebServer server(80);     //HTML Webserver
DHT dht(DHTPin, DHTTYPE);        // Creating an object

/*Wi-Fi SSID & Password*/ 
const String ssid="Galaxy M51B56A";
const String pass="Home@123";  

bool FlameState = 0;                        //Varible to store Flame State; 0 when detected
int SmokeVal = 0;                           //Variable to store the Smoke Value
int SmokePercent = 0;                       //Variable to store the Smoke Value
const int SmokeThreshold = 600;             //Threshold for Smoke Value
float TemperatureVal = 0;                   //Variable to store the Temperature Value
const float TemperatureThreshold = 50.0;    //Threshold for Humidity Value
float HumidityVal = 0;                      //Variable to store the Temperature Value
const float HumidityMinThreshold = 30.0;    //Lower Threshold for Humidity Value
const float HumidityMaxThreshold = 50.0;    //Higher Threshold for Humidity Value 
       
void setup() {   
    Serial.begin(115200);   
    pinMode(BuzzerPin,OUTPUT);
    pinMode(FlameState,INPUT); 
    delay(100);            

    //Connecting to Wi-Fi          
    Serial.println("Connecting to ");   
    Serial.println(ssid);    //connect to your local wi-fi network   
    WiFi.begin(ssid, pass);    //check wi-fi is connected to wi-fi network   
    while (WiFi.status() != WL_CONNECTED) {   
      delay(1000);  
      Serial.print("."); 
      }  
    Serial.println("");   
    Serial.println("WiFi connected..!");   

    Serial.print("Got IP: ");     
    Serial.println(WiFi.localIP());   

    server.on("/", handle_OnConnect);   
    server.onNotFound(handle_NotFound);    
    server.begin();   
    Serial.println("HTTP server started");  
} 

void loop() { 
    server.handleClient();  
    TemperatureVal =dht.readTemperature();     // Get temperature reading  
    HumidityVal = dht.readHumidity();          // Get the humidity reading 
    Serial.print("Temperature Value : ");
    Serial.println(TemperatureVal); 
    Serial.print("Humidity Value : ");
    Serial.println(HumidityVal);  

    if (TemperatureVal>=TemperatureThreshold){    // Check if the temperature is beond the Threshold value
      Serial.println("Temperature is beyond the Threshold value");
      digitalWrite(BuzzerPin, HIGH);
      delay(3000);
      digitalWrite(BuzzerPin, LOW);
    }

    SmokeVal= analogRead(SmoakeSensorPin);//reads the analog value from the methane sensor's A0 pin
    SmokePercent=map(SmokeVal, 0, 1023, 0, 100); // Convert the smoake value into %
    Serial.print("Percentage of Smoke : ");
    Serial.println(SmokePercent);
    if (SmokeVal>650){         // Check if the smoke value is beond the Threshold value
      digitalWrite(BuzzerPin, HIGH);
      delay(3000);
      digitalWrite(BuzzerPin, LOW);
    }
  
    FlameState = digitalRead(FlameSensorPin);  //Get flame status
    if (FlameState ==0){     //Check if a flame is detected
      Serial.println("Fire Detected");
      digitalWrite(BuzzerPin, HIGH);
      delay(3000);
      digitalWrite(BuzzerPin, LOW);
    }
      
    delay(1000);
  }  

//Function to send data to the HTML webpage
void handle_OnConnect() {    
    server.send(200, "text/html", SendHTML(TemperatureVal,HumidityVal,SmokePercent,FlameState));    
    Serial.println(WiFi.localIP()); 
  }  
    
//Function to handle if serve not found  
void handle_NotFound(){   
      server.send(404, "text/plain", "Not found"); 
  }  
      
String SendHTML(float TemperatureVal,float HumidityVal, int SmokePercent, int FlameState){   
    String ptr = "<!DOCTYPE html> <html>\n";  
    ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";  
    ptr +=" <meta http-equiv=\"refresh\" content=\"3\">\n";
    ptr +="<title>ESP8266 Weather Report</title>\n";   
    ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";  
    ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";   
    ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";   
    ptr +="</style>\n";  
    ptr +="</head>\n";   
    ptr +="<body>\n";   
    ptr +="<div id=\"webpage\">\n";   
    ptr +="<h1>Fire Alert System Report</h1>\n";     
    ptr +="<p>Temperature : ";   
    ptr +=(int)TemperatureVal;  
    ptr +="&deg;C</p>";   
    ptr +="<p>Humidity : ";  
    ptr +=(int)HumidityVal;  
    ptr +="%</p>"; 
    ptr +="<p>Smoke Percentage : ";   
    ptr +=(int)SmokePercent;  
    ptr +="%</p>";   
    ptr +="<p>Flame State : ";  
    ptr +=(int)!FlameState;  
    ptr +="</p>";
    ptr +="</div>\n";   
    ptr +="</body>\n";  
    ptr +="</html>\n";  
    return ptr; 
}
