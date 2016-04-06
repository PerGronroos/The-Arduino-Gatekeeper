# The-Arduino-Gatekeeper
Code repo for Arduino Maker Challenge

The ArduinoCode folder contains the multiple-tab Arduino sketch written for the Arduino Mega 2560. While each file is a  *.ino, each one is a tab in the Arduino IDE. This helps keep the code organized in separate sections.

The src folder contains the modified VirtualShield library (*.cpp and *.h) which uses the Serial1 port on the Mega 2560. The major edit is on line 179 which assigns the serial port to the VirtualShield.

The WiFi configuration will need to be set under WiFi Network Config section in the Azure_RFID_Upload_Formatted_Mega2560_VirtualShields_Revised.ino file. 
Under the Azure IoT Hub Config section, the hostname, feeduri, and authSAS (SAS code for the device) will need to be updated with details from Azure, and the apiver field may need to be updated.
The timeZone can be updated to reflect your timezone (set by default to Pacific Time).

Please refer to https://www.hackster.io/challenges/arduino-microsoft-maker for additional usage details.
