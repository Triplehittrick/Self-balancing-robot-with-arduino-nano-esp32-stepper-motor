void motor_run(){
  if (AngleY > 60 || AngleY < -60) {//Kill robot
      digitalWrite(Enable,HIGH);//stop motors
      esc.write(0);
  } else if(AngleY < 60 && AngleY > -60){
  digitalWrite(Enable,LOW);

  //ENCODER LOOP
  //PID for AS5600
  Input_ENC = correctedAngle;  
  //Input_ENC = constrain(Input_ENC,-360,360);                  
  I_ENC += Input_ENC * Ki_ENC;
  I_ENC = constrain(I_ENC, -200, 200);

  Output_ENC = Kp_ENC * Input_ENC + I_ENC + Kd_ENC * (Input_ENC - Input_lastENC);
  Input_lastENC = Input_ENC;                               

  //Set output limit
  if (Output_ENC >= -2 && Output_ENC <= 2)Output_ENC = 0;
  Output_ENC = constrain(Output_ENC, -100, 100); //-100 100
  //PID for MOTOR_L
  Input_L = AngleY + Offset - Vgo;                    
  I_L += Input_L * Ki;
  I_L = constrain(I_L, -500, 500);

  //Output_L = Kp * Input_L + I_L + Kd * (Input_L - Input_lastL) - Vgo_L;//for 1 PID loop
  Output_L = Kp * Input_L + I_L + Kd * (Input_L - Input_lastL)-Output_ENC - Vgo_L;//for 2 PID loops
  Input_lastL = Input_L;                               

  //Set output limit
  if (Output_L >= -5 && Output_L <= 5)Output_L = 0;
  Output_L = constrain(Output_L, -520, 520);//Adjust motor speed. Motor speed increase = momen decrease.


  //PID for MOTOR_R
  Input_R = AngleY + Offset - Vgo; 
  I_R += Input_R * Ki;
  I_R = constrain(I_R, -500, 500);



  //Output_R = Kp * Input_R + I_R + Kd * (Input_R - Input_lastR)- Vgo_R;
  Output_R = Kp * Input_R + I_R + Kd * (Input_R - Input_lastR) - Output_ENC - Vgo_R;
  Input_lastR = Input_R;

  //Set output limit
  if (Output_R >= -5 && Output_R <= 5)Output_R = 0;
  Output_R = constrain(Output_R, -520, 520);

  //LINEARIZING
  if (Output_L > 0)M_L = 405 - (1 / (Output_L + 9)) * 5500;
  else if (Output_L < 0)  M_L = -405 - (1 / (Output_L - 9)) * 5500;
  else M_L = 0;

  if (Output_R > 0)M_R = 405 - (1 / (Output_R + 9)) * 5500; 
  else if (Output_R < 0)M_R = -405 - (1 / (Output_R - 9)) * 5500;
  else M_R = 0;

  if (M_L > 0)Motor_L = 400 - M_L;
  else if (M_L < 0)Motor_L = -400 - M_L;
  else Motor_L = 0;

  if (M_R > 0)Motor_R = 400 - M_R;
  else if (M_R < 0)Motor_R = -400 - M_R;
  else Motor_R = 0;
  
  //Motors run
  Speed_L(Motor_L);
  Speed_R(Motor_R);
  }
}
//     Moving MOTOR_L
//....................................
void Speed_L(int16_t x) {
  if (x < 0) {
    Dir_M2 = -1;
    PORTD &= 0b11110111;
  }
  else if (x > 0) {
    Dir_M2 = 1;
    PORTD |= 0b00001000;
  }
  else Dir_M2 = 0;

  Count_BOT2 = abs(x);
  Count_TOP2 = Count_BOT2 / 2;
}


//     Moving MOTOR_R
//....................................
void Speed_R(int16_t x) {
  if (x < 0) {
    Dir_M3 = -1;
    PORTD &= 0b11101111;
  }
  else if (x > 0) {
    Dir_M3 = 1;
    PORTD |= 0b00010000;
  }
  else Dir_M3 = 0;

  Count_BOT3 = abs(x);
  Count_TOP3 = Count_BOT3 / 2;
}
