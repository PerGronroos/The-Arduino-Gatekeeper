/*Parts written by BARRAGAN, HC Gilje, djmatic, and 
 * Martijn The for RFID reader.
 */
void RFIDread()
{
  byte i = 0;
  byte val = 0;
  byte code[6];
  byte checksum = 0;
  byte bytesread = 0;
  byte tempbyte = 0;

  if ((val = Serial2.read()) == 2)
  {
    bytesread = 0;
    while (bytesread < 15)
    {
      if (Serial2.available() > 0)
      {
        val = Serial2.read();
        if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) // if header or stop bytes before the 10 digit reading 
        {
          break;                                    // stop reading
        }

        // Do Ascii/Hex conversion:
        if ((val >= '0') && (val <= '9')) 
          val = val - '0';
        else if ((val >= 'A') && (val <= 'F')) 
          val = 10 + val - 'A';

        // Every two hex-digits, add byte to code:
        if (bytesread & 1 == 1) {
          // make some space for this hex-digit by
          // shifting the previous hex-digit with 4 bits to the left:
          code[bytesread >> 1] = (val | (tempbyte << 4));

          if (bytesread >> 1 != 5) // If we're at the checksum byte,
          {                
            checksum ^= code[bytesread >> 1];       // Calculate the checksum... (XOR)
          };
        } 
        else 
        {
          tempbyte = val;                           // Store the first hex digit first...
        };

        bytesread++;                                // ready to read next digit
      } 
    } 

    if (bytesread == 12)
    {
      Serial.print("5-byte code: ");
      for (i=0; i<5; i++) 
      {
        if (code[i] < 16) Serial.print("0");
        Serial.print(code[i], HEX);
        Serial.print(" ");
      }
      RFID = "";
      for (i=0; i<5; i++) 
      {
        if (code[i] < 16) RFID.concat(0);
        RFID.concat(String(code[i], HEX));
      }
      screen.printAt(12, "An RFID tag has been scanned:");
      screen.printAt(13, RFID);
      Serial.println();

      Serial.print("Checksum: ");
      Serial.print(code[5], HEX);
      Serial.println(code[5] == checksum ? " -- passed." : " -- error.");
      Serial.println();
      digitalWrite(4, LOW);
      delay(1000);
      digitalWrite(4, HIGH);
      //tone(51, 523, 2); //Activates buzzer tone.
    }
    bytesread = 0;
  }
  Serial2.end();
  Serial2.begin(9600);

  if (RFID == "6700729be4") {
    user = "Timothy Le";
    RFIDGranted();
  }
  else if (RFID == "7e001fe09e") {
    user = "Nathan Le";
    RFIDGranted();
  }
  else {
    Serial.println("RFID - Access Denied");
    screen.printAt(14, "RFID - Access has been denied.");
    digitalWrite (2, LOW);
    delay(500);
    digitalWrite(2, HIGH);
    closeservo();
    access = "Denied";
    gate = "Closed";
    user = "Invalid_RFID";
    azureHTTPpost(RFID,access,gate,user);
  }
}

void RFIDGranted() {
  Serial.println("RFID - Access Granted");
  screen.printAt(14, "RFID - Access has been granted.");
  access = "Granted";
  gate = "Open";
  manualopenservo();
}

