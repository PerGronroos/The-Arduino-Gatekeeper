/*
 * Time, by Michael Margolis, adds timekeeping ability in hours, 
 * minutes, seconds, days, months and years. 
 
  time.c - low level time and date functions
  Copyright (c) Michael Margolis 2009-2014
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(month());
  Serial.print("-");
  Serial.print(day());
  Serial.print("-");
  Serial.print(year()); 
  Serial.print(" ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println(); 
  digitalClockString();
  screen.printAt(10, internetTime);
  if (hour() > 19)
    digitalWrite(23, HIGH);
  else if (hour() > 6)
    digitalWrite(23, LOW);
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockString()
{
  // digital clock display of the time
  internetTime = "";
  internetTime.concat(month());
  internetTime.concat("-");
  internetTime.concat(day());
  internetTime.concat("-");
  internetTime.concat(year()); 
  internetTime.concat(" ");
  internetTime.concat(hour());
  printDigitsString(minute());
  printDigitsString(second());
}

void printDigitsString(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  internetTime.concat(":");
  if(digits < 10)
    internetTime.concat('0');
  internetTime.concat(digits);
}

/*-------- NTP code ----------*/

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) 
  {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) 
    {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + ((timeZone + DST) * SECS_PER_HOUR);
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

