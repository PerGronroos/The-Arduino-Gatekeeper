void closeservo() 
{
  digitalWrite(12, HIGH); //Activate servo power.
  myservo.write(103);              //Set servo position
  delay(600);                      //Wait 600ms for servo to complete rotation.
  digitalWrite(12, LOW); //Disable servo power.
  delay(50);
  screen.printAt(15, "Gate closed.");
  screen.printAt(7, " ");
  screen.printAt(16, " ");
  gate = "Closed";
}

void manualopenservo()
{
  gate = "Open";
  digitalWrite(3, LOW); //Blink green LED.
  delay(500);
  digitalWrite(3, HIGH);
  digitalWrite(12, HIGH); //Activate servo power.
  myservo.write(54);              //Set servo position
  delay(600);                     //Wait 600ms for servo to complete rotation.
  digitalWrite(12, LOW); //Disable servo power.
  screen.printAt(15, "Gate open.");
  const unsigned long time1 = millis(); //time access was granted
  while (digitalRead(9) == 1) //while the gate still hasn't been opened
  {
    unsigned long time2 = millis(); //current time
    if (time2 - time1 > 3500) //If the gate isn't opened within 5 seconds, close the gate
      break;
  }
  azureHTTPpost(RFID,access,gate,user); //Post opening of gate.
  int dotCount = 0;
  int waitCount = 0;
  while (digitalRead (9) == 0) 
  {
    Serial.println("Waiting for gate to close...");
    screen.printAt(15, "Waiting for gate to close...");
    if (digitalRead(9) != 0) //Check periodically if the gate has physically closed yet
      break; //If it has, continue to code to close the gate
    delay(500);
    String output = "";
    while(dotCount < (waitCount * 3)) //Waiting animation for Virtual Shields
    {
      output.concat(" ");
      dotCount++;
    }
    output.concat("...");
    screen.printAt(16, output);
    dotCount = 0;
    if (waitCount < 8)
      waitCount++;
    else
      waitCount = 1;
  }
  delay(250);
  closeservo(); //close the gate
  azureHTTPpost(RFID,access,gate,user); //Post the closure.
}
