void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n'; // Or any character can be used to mark the end of a message
  char rc;

  while (SerialPort.available() > 0) {
    rc = SerialPort.read();

    if (rc != endMarker) {
      if (ndx < numChars - 1) {
        receivedChars[ndx] = rc;
        ndx++;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // Null terminate the string
      ndx = 0;
      newData = true;
    }
  }
}
void processReceivedData() {
  //Serial.print("Received: ");
  //Serial.println(receivedChars);
  int x,y;
  int mmotorSpeed;
  if (sscanf(receivedChars, "%d %d", &x, &y) == 2) {
    joyX = x;
    joyY = y;

    //Print data values:
    //Serial.print("X: ");
    //Serial.print(joyX);
    //Serial.print(", Y: ");
    //Serial.println(joyY);
    //Serial.println();

    // LEGENDARY IF-ELSE COMMANDS
    if (joyY > 10 && joyY <= 99 && joyX > -45 && joyX < 45) {//forward
      mmotorSpeed = map(abs(joyY), 11, 99, 1000, 3000);  
      esc.write(mmotorSpeed);
    }
    else if(joyX <= -45 && joyX >= -90 && joyY > 10){ //turn right and forward
    mmotorSpeed = map(abs(joyY), 11, 99, 1000, 2500);
    Vgo = map (abs(joyX), 10, 99, 0, 5);
      Vgo_L -= 12;
      Vgo_R += 12;
    }
    else if (joyX >= 45 && joyX <= 90 && joyY > 10){//turn left and forward
    mmotorSpeed = map(abs(joyY), 11, 99, 1000, 2500);
    Vgo = map (abs(joyX), 10, 99, 0, -5);
      Vgo_L += 12;
      Vgo_R -= 12;
    }
    else if (joyY >= -99 && joyY < -10 && joyX <= 99 && joyX > 45){//turn right only
      Vgo_L += 25;
      Vgo_R -= 25;
      Vgo = 0;
    }
    else if(joyY >= -99 && joyY < -10 && joyX >= -99 && joyX <= -45){//turn left only
      Vgo_L -= 25;
      Vgo_R += 25;
      Vgo = 0;
    } else {
      esc.write(0); // braking
      Vgo = Vgo_L = Vgo_R = 0;
    }
  } else {
    Serial.println("Invalid input format.  Send two floats separated by space and ending with a newline.");
    // Reset all data
    joyX = 0;
    joyY = 0;
  }
}
