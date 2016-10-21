//NodeMCU/ESP8266 implement WebSocketsServer
//arduino-er.blogspot.com

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include "FS.h"

//Vivi
#include <Servo.h>

int sensePin_pt = A0;

int servoPin_1 = D1; 
int servoPin_2 = D2; 

Servo servo_1;
Servo servo_2;

//Button control from the web
int whichSong = 1;

// WiFi Module
ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Output Module
const int LED_PIN = LED_BUILTIN;

String html_home;
String css_code;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: 
           {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

                // send message to client
                webSocket.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get text: %s\n", num, payload);

            if(payload[0] == '#') {

                if(payload[1] == 'O') {
                    digitalWrite(LED_PIN, LOW);
                    whichSong = 0; 
                }
                else {
                    digitalWrite(LED_PIN, HIGH);
                    whichSong = 1; 
                }
            }

            break;
    }

}


void prepareFile(){
  
    Serial.println("Prepare file system");
    SPIFFS.begin();
  
    File file = SPIFFS.open("/home.html", "r");
    if (!file) {
        Serial.println("HTML file open failed");  
    } 
    else {
        Serial.println("HTML file open success");

        html_home = "";
        while (file.available()) {
            //Serial.write(file.read());
            String line = file.readStringUntil('\n');
            html_home += line + "\n";
        }
        file.close();

        //Serial.print(html_home);
    }

    File css_file = SPIFFS.open("/style.css", "r");
    if (!css_file) {
        Serial.println("CSS file open failed");  
    } 
    else {
        Serial.println("CSS file open success");

        css_code = "";
        while (css_file.available()) {
            //Serial.write(file.read());
            String line = css_file.readStringUntil('\n');
            css_code += line + "\n";
        }
        css_file.close();

        //Serial.println(css_code);
    }


}


void setup() {
    Serial.begin(115200);

    //Serial.setDebugOutput(true);
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }


    // setup LED pin
    Serial.println("Setup LED pins");
    pinMode(LED_PIN, OUTPUT);    //GPIO16 is an OUTPUT pin;
    digitalWrite(LED_PIN, HIGH); //Initial state is ON

    // read the html code to html_homes
    prepareFile();

    WiFi.softAP("Cannon_wifi", "00000000");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

    // handle index
    server.on("/", []() {
        // send home.html
        server.send(200, "text/html", html_home);
    });

    server.on("/style.css", []() {
        // send home.html
        server.send(200, "text/css", css_code);
    });
    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);

    Serial.printf("Server Start\n");

    //Vivi
    //Sensor input pin 
    pinMode(sensePin_pt,INPUT);

    //Connect the servos with the output pin
    servo_1.attach(servoPin_1);
    servo_2.attach(servoPin_2);
  
    //Initial state of the servo
    servo_1.write(90);  
    servo_2.write(90);  

}

void loop() {
    webSocket.loop();
    server.handleClient();
      //change the speed of the song by reading the value from the potentiometer
    int sense_pt = analogRead(sensePin_pt); 
    int delaySpeed = sense_pt + 200; 
    if (whichSong == 0) {  //song #1 whichSong == 0
      Serial.println("Song 1");
      servo_1.write(135); 
      delay(200); 
      servo_1.write(45);
     
      delay(delaySpeed); 
  
      servo_2.write(135); 
      delay(200); 
      servo_2.write(45);
  
      delay(delaySpeed); 
  
      servo_2.write(135); 
      delay(200); 
      servo_2.write(45);
      
      delay(delaySpeed); 
      
    } else { // song #2 whichSong == 1
      Serial.println("Song 2");
      servo_1.write(135); 
      delay(200); 
      servo_1.write(45);
     
      delay(delaySpeed); 
  
      servo_1.write(135); 
      delay(200); 
      servo_1.write(45);
  
      delay(delaySpeed); 
  
      servo_1.write(135); 
      delay(200); 
      servo_1.write(45);
  
      delay(delaySpeed); 
  
      servo_2.write(135); 
      delay(200); 
      servo_2.write(45);
      
      delay(delaySpeed); 
    }

}
