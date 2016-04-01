void manualOverride(String method) 
{
  Serial.println("Manual override.");
  screen.printAt(7, "Manual override switch has been triggered.");
  RFID = "NULL";
  access = method;
  user = ("Unauthenticated");
  manualopenservo();
}

