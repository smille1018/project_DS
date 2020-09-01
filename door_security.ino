// Wi-Servo v1.0 By Igor Fonseca Albuquerque

//include libraries
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial esp8266(5, 4); //set ESP8266 RX pin = 5, and TX pin = 4

//define variables
#define DEBUG true //display ESP8266 messages on Serial Monitor
#define SERV1 9  //servo 1 on digital port 9

Servo s1; //servo 1
int pos1 = 170; //servo 1 current position
int vel = 10; //servos' speed (100 -> 90º in 1 s)(1 -> 90º in 9 s)
int pos1min = 20; //servo 1 minimum position
int pos1max = 160; //servo 1 maximum position


//**************
// Arduino Setup
//**************
void setup()
{

  //attach and set servos' angles
  s1.attach(SERV1);
  s1.write(pos1max);
  //detach servos
  s1.detach();
  
  //start serial communication
  Serial.begin(9600);
  esp8266.begin(19200);
  
  sendData("AT+RST\r\n", 2000, DEBUG); //reset module
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); //set station mode
  sendData("AT+CWJAP=\"eunseo0218\",\"89987563\"\r\n", 2000, DEBUG);   //connect wifi network
  while(!esp8266.find("OK")) { //wait for connection
  } 
  sendData("AT+CIFSR\r\n", 1000, DEBUG); //show IP address
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); //allow multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // start web server on port 80
}

//**********
// Main Loop
//**********
void loop()
{
  if (esp8266.available())  //check if there is data available on ESP8266
  {
    if (esp8266.find("+IPD,")) //if there is a new command
    {
      String msg;
      esp8266.find("?"); //run cursor until command is found
      msg = esp8266.readStringUntil(' '); //read the message
      String command = msg.substring(0, 3); //command is informed in the first 3 characters. "sr1" = command to servo #1 and "sr2" = command to servo #2
      String valueStr = msg.substring(4);   //next 3 characters inform the desired angle
      int value = valueStr.toInt();         //convert to integer
      if (DEBUG) {
        Serial.println(command);
        Serial.println(value);
      }
      delay(100);
      
      //************
      // move servos
      //************
      
      //move servo1 to desired angle
      if(command == "sr1") {
         //limit input angle
      if (value >= pos1max) {
        value = pos1max;
      }
      if (value <= pos1min) {
        value = pos1min;
      }
      s1.attach(SERV1); //attach servo
      while(pos1 != value) {
        if (pos1 > value) {
          pos1 -= 1;
          s1.write(pos1);
          delay(100/vel);
        }
        if (pos1 < value) {
          pos1 += 1;
          s1.write(pos1);
          delay(100/vel);
        }
      }
      s1.detach(); //dettach
      }
    }
  }
}


//********************
// Auxiliary functions
//********************

//send data to ESP8266
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
