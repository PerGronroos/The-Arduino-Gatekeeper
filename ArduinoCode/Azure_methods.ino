void azureHTTPget() //Check for IoT Hub messages
{
  //Close any connection before sending a new request
  client.stop();
  if (client.connect(hostname, 443))
  {
    client.print("GET ");
    client.print(feeduri);
    client.print("devicebound");
    client.print(apiver);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(hostname);
    client.print("Authorization: ");
    client.println(authSAS);
    client.println("Connection: close");
    client.println();

    //Note last connection time
    lastConnectionTime = millis();
    Serial.print("Azure GET - ");
    digitalClockDisplay();
  }
  else
  {
    Serial.println("Azure connection failed - GET");
    delay(2000);
  }
}

void azureHTTPdelete(String lockid) //Acknowledge latest IoT Hub message
{
  client.stop();
  if (client.connect(hostname, 443))
  {
    client.print("DELETE ");
    client.print(feeduri);
    client.print("devicebound");
    client.print("/");
    client.print(lockid);
    client.print(apiver);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(hostname);
    client.print("Authorization: ");
    client.println(authSAS);
    client.println("Connection: close");
    client.println();

    //Note last connection time
    lastConnectionTime = millis();
    Serial.println("Azure DELETE");
  }
  else Serial.println("Azure connection failed - DELETE");
  
}

void azureHTTPpost(String RFID, String access, String gate, String user)
{
  client.stop();
  if(client.connect(hostname, 443))
  {
    client.print("POST ");
    client.print(feeduri);
    client.print("events");
    client.print(apiver);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(hostname);
    client.print("Authorization: ");
    client.println(authSAS);
    client.println("Connection: close");
    client.println("Content-Type: text/plain");
    String upload = "{\"deviceId\":\"MKR1000-01\",\"deviceTime\":\"";
    digitalClockString();
    upload.concat(internetTime);
    upload.concat("\",\"RFID\":\"");
    upload.concat(RFID);
    upload.concat("\",\"ID1\":\"");
    upload.concat(user);
    upload.concat("\",\"Access\":\"");
    upload.concat(access);
    upload.concat("\",\"Gate\":\"");
    upload.concat(gate);
    upload.concat("\"}");
    client.print("Content-Length: ");
    client.println(upload.length());
    client.println();
    client.println(upload);
    client.println();

    //Note last connection time
    lastConnectionTime = millis();
    Serial.println("Azure POST");
    POST = "Success";
  }
  else {
    Serial.println("Azure connection failed - POST");
    POST = "Failed";
    digitalWrite(2, LOW);
    delay(250);
    digitalWrite(2, HIGH);
  }
}
