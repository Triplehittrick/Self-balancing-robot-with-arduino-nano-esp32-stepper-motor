#include "stmpu6050.h"
#include "SoftwareSerial.h"
#include <stdlib.h>     /* strtof */
SoftwareSerial SerialPort(10,9); //define RX, TX
//Non blocking approach
//UART usually makes conflict. To solve this problem, use non blocking approach
const byte numChars = 32; // Size of the receive buffer
char receivedChars[numChars];
bool newData = false;

long currentT,previousT,currentT2,previousT2 = 0;
SMPU6050 mpu6050;
//EDF 
#include<Servo.h>

#define ESC_PIN 2

Servo esc;

// Serial output refresh time
const long SERIAL_REFRESH_TIME = 5;
long refresh_time;

# define Enable       8                       
# define Step_3       7                          
# define Step_2       6                         
# define Dir_3        4          
# define Dir_2        3                           

void  pin_INI() {
  pinMode(Enable, OUTPUT);
  pinMode(Step_2, OUTPUT);
  pinMode(Step_3, OUTPUT);
  pinMode(Dir_2, OUTPUT);
  pinMode(Dir_3, OUTPUT);
  digitalWrite(Enable, LOW);
}

//     timer2
void timer_INI() {
  TCCR2A = 0;                                                               //Make sure that the TCCR2A register is set to zero
  TCCR2B = 0;                                                               //Make sure that the TCCR2A register is set to zero
  TCCR2B |= (1 << CS21);                                                    //Set the CS21 bit in the TCCRB register to set the prescaler to 8
  OCR2A = 39;        //159                                                       //The compare register is set to 39 => 20us / (1s / (16.000.000Hz / 8)) - 1
  TCCR2A |= (1 << WGM21);                                                   //Set counter 2 to CTC (clear timer on compare) mode Chế độ CTC bộ đếm được xóa về 0 khi giá trị bộ đếm (TCNT0) khớp với OCR0A
  TIMSK2 |= (1 << OCIE2A);                                                  //Set the interupt enable bit OCIE2A in the TIMSK2 register
}


int8_t Dir_M1, Dir_M2, Dir_M3;                                               //Biến xác định hoạt động của động cơ và chiều quay Dir_Mx >0 quay thuận , Dir_Mx <0 quay ngược Dir_Mx =0 motor ngừng quay
volatile int Count_timer1, Count_timer2, Count_timer3;                       //đếm các lần TIMER xuất hiện trong chu kỳ xung STEP
volatile int32_t Step1, Step2, Step3;
int16_t Count_TOP1, Count_BOT1, Count_TOP2, Count_BOT2, Count_TOP3, Count_BOT3;  //vị trí cuối của phần cao và cuối phần thấp trong 1 chu kỳ xung STEP
float Input_L, Input_R, Output, I_L, I_R, Input_lastL, Input_lastR, Output_L, Output_R, M_L, M_R, Motor_L, Motor_R;
//globals
float joyX,joyY;
// PID 35 12 3.5
float Kp =60;
float Ki = 10;
float Kd = 15;

float  Offset = 2;
float AngleY = 0;
float  Vgo = 0;
int  Vgo_L = 0;
int  Vgo_R = 0;


unsigned long loop_timer;

void setup() {
  mpu6050.init(0x68);
  Serial.begin(115200);               //Khai báo Serial
  SerialPort.begin(9600);
  pin_INI();                        //Khai báo PIN Arduino đấu nối 3 DRIVER A4988
  timer_INI();                      //Khai báo TIMER2

  esc.attach(ESC_PIN, 1000, 2000);
  esc.write(0);
  delay(2000);
}

void loop() {
  //RECEIVING DATA FROM ESP32
  recvWithEndMarker(); // Check for new data
    if (newData) {
    processReceivedData(); // Process the data if available
    newData = false; // Reset the flag
  }
  //PID CONTROLLER CYCLE 
  currentT = millis();
  if (currentT - previousT >= 15){
    AngleY = mpu6050.getYAngle();
   //Serial.println(AngleY);
     motor_run();
    previousT = currentT;
  }
}

