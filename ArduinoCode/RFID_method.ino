/*Based on prior code written by BARRAGAN, HC Gilje, djmatic, and 
  Martijn The for RFID reader.
  **
  Based on code by BARRAGAN <http://people.interaction-ivrea.it/h.barragan> 
  and code from HC Gilje - http://hcgilje.wordpress.com/resources/rfid_id12_tagreader/
  Modified for Arudino by djmatic
  Modified for ID-12 and checksum by Martijn The - http://www.martijnthe.nl/
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
        if (code[i] < 16) Serial.print("0"); //Add zeroes in empty spaces.
        Serial.print(code[i], HEX);
        Serial.print(" ");
      }
      RFID = "";
      for (i=0; i<5; i++) 
      {
        if (code[i] < 16) RFID.concat(0); //Setup upload string.
        RFID.concat(String(code[i], HEX));
      }
      screen.printAt(12, "An RFID tag has been scanned:"); //Print the data to Serial UART.
      screen.printAt(13, RFID);
      Serial.println();

      Serial.print("Checksum: ");
      Serial.print(code[5], HEX);
      Serial.println(code[5] == checksum ? " -- passed." : " -- error.");
      Serial.println();
      digitalWrite(4, LOW); // Flash the RFID scan LED.
      delay(1000);
      digitalWrite(4, HIGH);
      //tone(51, 523, 2); //Activates buzzer tone.
    }
    bytesread = 0;
  }
  Serial2.end();
  Serial2.begin(9600); //Reset the serial port for the RFID reader.
  
  ////*** List RFID tags that should be recognized ***///

  if (RFID == "0000000000") {
    user = " ";
    RFIDGranted();
  }
  else if (RFID == "0000000000") {
    user = " ";
    RFIDGranted();
  }
  
  //Only perform these actions if the RFID is not recognized.
  else 
  {
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

//Perform these actions for all recognized RFID tags.
void RFIDGranted() 
{
  Serial.println("RFID - Access Granted");
  screen.printAt(14, "RFID - Access has been granted.");
  access = "Granted";
  gate = "Open";
  manualopenservo();
}

