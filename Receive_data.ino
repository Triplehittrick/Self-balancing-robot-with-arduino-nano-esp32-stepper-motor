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

    // LEGENDARY IF-ELSE 
    /*if(joyX > -10 || joyX <10 || joyY >-10 || joyY <10) {//dead zone
      esc.write(0);
      Vgo = Vgo_L = Vgo_R = 0;
    }*/
    if (joyY >= 10 && joyY <=99 && joyX > -45 && joyX < 45) {//forward
      mmotorSpeed = map(abs(joyY), 10, 99, 1000, 2200);  
      esc.write(mmotorSpeed);
      //Vgo = map(abs(joyY), 10, 99, 0, 3.5);
    }
    else if(joyX <= -45 && joyX >= -90){ //turn right
      Vgo_L -= 30;
      Vgo_R += 30;
    }
    else if (joyX >= 45 && joyX <= 90){//turn left
      Vgo_L += 30;
      Vgo_R -= 30;
    }
    /*else if (joyY >= -99 && joyY <= -10){
      Vgo = map(abs(joyY), 10, 99, 0, -3.5);
    }
    else if(joyY < 0 && joyX > 0){
      Vgo = map(abs(joyY), 10, 99, 0, 5);
    }
    else if (joyY < 0 && joyX < 0){
      Vgo = map(abs(joyY), -10, -99, 0, -5);
    }*/ else {
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