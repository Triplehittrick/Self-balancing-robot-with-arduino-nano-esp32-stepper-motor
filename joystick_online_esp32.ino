#include <HardwareSerial.h>

HardwareSerial SerialPort(2); 

const int esp32TxPin = 17;
const int esp32RxPin = 16;

//LED CONTROL AVOID DELAY FUNCTION-----------------------------------------
#define ledPin 2
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 500;
//BLUETOOTH CONTROLLER-----------------------------------------------------
#include <BluetoothSerial.h>

#define    STX          0x02
#define    ETX          0x03

byte cmd[8] = {0, 0, 0, 0, 0, 0, 0, 0};                // bytes received
byte buttonStatus = 0;                                 // first Byte sent to Android device

BluetoothSerial SerialBT;

float loop_time = 15;
long currentT, previousT;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);//serial begin
  

  SerialBT.begin("ROBOT");//Read data from controller
  while(SerialBT.available())  SerialBT.read();
  //UART ARDUINO NANO-ESP32
  SerialPort.begin(9600, SERIAL_8N1, esp32RxPin, esp32TxPin);
  Serial.println("ESP32 Ready");
  //LED setup
  pinMode(ledPin, OUTPUT);
  delay(500);
}
void loop() {
  currentT = millis();
  if (currentT - previousT >= loop_time){
  //BLE_CONTROLLER------------------------------------------------------------------------------------------------------------------------
  if(SerialBT.available())  {                           // data received from smartphone
    delay(2);
    cmd[0] =  SerialBT.read();  
    if(cmd[0] == STX)  {
      int i=1;      
      while(SerialBT.available())  {
        delay(1);
        cmd[i] = SerialBT.read();
        if(cmd[i]>127 || i>7)                 break;     // Communication error
        if((cmd[i]==ETX) && (i==2 || i==7))   break;     // Button or Joystick data
        i++;
      }
    //if(i==2)          getButtonState(cmd[1]);    // 3 Bytes  ex: < STX "C" ETX >
      if(i==7)          getJoystickState(cmd);     // 6 Bytes  ex: < STX "200" "180" ETX >
    }
  } 
  BLINK_500ms();
  previousT = currentT;
  }
}
//Functions
void getJoystickState(byte data[8])    {
  int joyX = (data[1]-48)*100 + (data[2]-48)*10 + (data[3]-48);       // obtain the Int from the ASCII representation
  int joyY = (data[4]-48)*100 + (data[5]-48)*10 + (data[6]-48);
  joyX = joyX - 200;                                                  // Offset to avoid
  joyY = joyY - 200;   
  // Send data
  SerialPort.print(joyX);
  SerialPort.print(" ");
  SerialPort.println(joyY);
  Serial.println();                                              // transmitting negative numbers
  // Print out data
  //Serial.print("ESP32: ");
  //Serial.print(joyX);
  //Serial.print(" ");
  //Serial.println(joyY);
  if(joyX<-100 || joyX>100 || joyY<-100 || joyY>100)     return; // commmunication error
  /*if(joyX < -10 || joyX >10 || joyY <-10 || joyY >10) {
  }*/
}
void BLINK_500ms(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}