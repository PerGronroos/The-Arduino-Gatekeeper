/*
  This code connects to the Azure IoT Hub, blinks onboard LED
  and prints message to serial console, then sends an
  acknowledge message to the IoT Hub. In addition, it will
  upload an RFID tag to the IoT Hub when an RFID card is
  read by the card reader.

  Parts written by Mohan Palanisamy (http://mohanp.com).
  Parts written by Timothy Le, KK6FSL.

  Instructions are here to properly set up the MKR1000 for
  SSL connections:
  http://mohanp.com/mkr1000-azure-iot-hub-how-to/

  Feb 27, 2016
*/

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <Servo.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <VirtualShield.h>
#include <Graphics.h>
#include <Colors.h>

///***WiFi Network Config ***///
char ssid[] = "---"; //  your network SSID (name)
char pass[] = "---"; // your network password (use for WPA, or use as key for WEP)

///*** Azure IoT Hub Config ***///
//see: http://mohanp.com/  for details on getting this right if you are not sure.
char hostname[] = "---.azure-devices.net";    // host name address for your Azure IoT Hub, usually part of azure-devices.net domain
char feeduri[] = "/devices/MKR1000/messages/"; //feed URI
char apiver[] = "?api-version=2016-02-03";
char authSAS[] = "---"; //Place SAS code for device here.

///*** Azure IoT Hub Config ***///
unsigned long lastConnectionTime = 0;
const unsigned long pollingInterval = 10L * 1000L; // 10 sec polling delay, in milliseconds
String RFID = "";
String access = "";
String gate = "Closed";
String user = "";
String POST = "";
String reason = "";
IPAddress ip;
IPAddress timeServer(132, 163, 4, 101);
const int timeZone = -8;
int DST = 1;
String internetTime = "";

///***Servo Setup***///
Servo myservo;

///*** Program Global Variables ***///
int status = WL_IDLE_STATUS;
WiFiSSLClient client;
WiFiUDP Udp;
unsigned int localPort = 2390;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

///*** Windows Virtual Shield Setup ***///
VirtualShield shield;
Graphics screen = Graphics(shield);

void setup()
{
  pinMode(13, OUTPUT); //MEGA 2560 LED, IoT message received LED.
  pinMode(2, OUTPUT); //RFID-fail LED.
  pinMode(3, OUTPUT); //RFID-success LED.
  pinMode(4, OUTPUT); //RFID read LED.
  pinMode(12, OUTPUT); //Servo control pin.
  pinMode(22, OUTPUT); //Bluetooth power ctrl.
  pinMode(23, OUTPUT); //Manual override lighting pin.
  pinMode(8, INPUT); //Manual override switch.
  pinMode(9, INPUT); //Reed switch.
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(22, LOW); //Enable bluetooth.
  digitalWrite(23, LOW); //Turn override LED off.
  myservo.attach(11);
  Serial.begin(9600);
  Serial.println("MKR1000 Azure IoT Gatekeeper");
  Serial2.begin(9600);

  ///*** VirtualShields Setup ***///
  shield.setOnRefresh(refresh);
  screen.setOnEvent(screenEvent);
  shield.begin(115200);
  screen.clear();
  screen.printAt(1, "This is the IoT Gatekeeper Virtual Shields interface");
  screen.printAt(2, "WiFi connecting...");

  if (digitalRead(9) == 1)
    closeservo();

  ///*** WiFi Connection ***///
  if (WiFi.status() == WL_NO_SHIELD)
    while (true);

  //Connect to WiFi
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(ssid, pass);
    delay(10000);
    Serial.println("WiFi connecting");
    ip = WiFi.localIP();
    Serial.println(ip);
  }
  screen.printAt(2, "WiFi connected.");
  Udp.begin(localPort);
  Serial.println("Waiting for sync...");
  setSyncProvider(getNtpTime);
}

void loop()
{
  
  String response = "";
  char c;
  //Read WiFi response
  while (client.available())
  {
    c = client.read();
    response.concat(c);
  }

  if (!response.equals(""))
  {
    //No Azure IoT messages = 204 status code
    if (response.startsWith("HTTP/1.1 204"))
      digitalWrite(13, LOW); //Turn off LED
      
    else if (response.startsWith("HTTP/1.1 200 OK"))
    {
      //Turn on LED
      digitalWrite(13, HIGH);
      Serial.println("Message: ");
      Serial.println(response);
      int firstQuote = response.indexOf('"');
      int secondQuote = response.indexOf('"', firstQuote + 1);
      String lockid = response.substring(firstQuote + 1, secondQuote);
      Serial.print("Lock ID: ");
      Serial.println(lockid);
      azureHTTPdelete(lockid);
    }
    else
    {
      Serial.println('1');
      Serial.println(response);
    }
  }

  if (Serial2.available() > 0)
  {
    Serial.println("RFID input");
    RFIDread();
  }
  if (millis() - lastConnectionTime > pollingInterval)
  {
    digitalWrite(13, LOW);
    azureHTTPget();
  }

  if (digitalRead(8) == 1) 
  {
    reason = "button";
    manualOverride(reason);
    reason = "";
  }
  shield.checkSensors();
}
