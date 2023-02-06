/*
  Program for entering a code for a locking mechanism
*/

#include <LiquidCrystal.h>
#include <IRremote.h>
#include <Servo.h>
#include <SoftwareSerial.h>

void PressRemote();
void CheckCursor();
int CheckCode();
void UpdateGSM();
void SendSMS();

int rs = 12, e = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; //lcd pins
LiquidCrystal lcd(rs, e, d4, d5, d6, d7);
int cursorX = 0, cursorY = 0;
int codeLength = 4;
int lcdCode[4] = {1, 2, 3, 4};
int lcdInput[32];
int inputLength = 0;

int recPin = 7; //reciver pin
IRrecv irrecv(recPin);
decode_results results;

Servo servo;
int servoPos;
int servoPin = 9;

int greenLED = 13;
int redLED = 8;

int gsmRX = A1;
int gsmTX = A0;
String cmd = "";
SoftwareSerial gsm(gsmTX, gsmRX);

void setup() 
{
  Serial.begin(9600);
  
  lcd.begin(16, 2); //setup number of columns and rows for lcd display
  lcd.cursor();
  
  irrecv.enableIRIn();
  
  servo.attach(servoPin);
  servo.write(90);
  
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  
  gsm.begin(9600);
  Serial.println("Initialising gsm");
  
  gsm.println("AT"); //Once the handshake test is successful, it will back to OK
  UpdateGSM();
  gsm.println("AT+CMGF=1");          // Configuration for sending SMS
  UpdateGSM();
  gsm.println("AT+CNMI=1,2,0,0,0");  // Configuration for receiving SMS
  UpdateGSM();
}

void loop() 
{
  CheckCursor();
  lcd.setCursor(cursorX, cursorY);
  if (irrecv.decode(&results)) {  // Returns 0 if no data ready, 1 if data ready.   
    PressRemote();
    irrecv.resume(); // Restart the ISR state machine and Receive the next value  
  }  
  
  UpdateGSM();
}

void UpdateGSM()
{
  delay(500);
  while (Serial.available()) {
    cmd+=(char)Serial.read();
 
    if(cmd!=""){
      cmd.trim();  // Remove added LF in transmit
      if (cmd.equals("S")) {
        SendSMS();
      } 
      else {
        gsm.print(cmd);
        gsm.println("");
      }
    }
  }
  
  while(gsm.available()) {
    Serial.write(gsm.read());//Forward what Software Serial received to Serial Port
  }
}

void SendSMS(){
  gsm.println("AT+CMGF=1");
  delay(500);
  gsm.println("AT+CMGS=\"+447497901554\"\r");
  delay(500);
  gsm.print("Hi!");
  delay(500);
  gsm.write(26);
}

int CheckCode()
{
  int passed = 1;
  
  if (inputLength != codeLength)
    return 0;
    
  for (int i = 0; i < codeLength; i++){
    if (lcdInput[i] != lcdCode[i]){
      passed = 0;
    }
  }
  return passed;
}

void CheckCursor()
{
  if (cursorX < 0){
    if (cursorY == 1){
      cursorX = 15;
      cursorY = 0;
    }
    else{
      cursorX = 0;
    }
  }
  if (cursorX > 15){
    if (cursorY == 0){
      cursorY = 1;
      cursorX = 0;
    }
    else{
      cursorX = 15;
    }
  }
  if (cursorY < 0){
    cursorY = 0;
  }
  if (cursorY > 1){
    cursorY = 1;
  }
}

void PressRemote()
{
  switch (results.value){
    case 0xFFA25D:  //off
      servo.write(90);
      break;
    case 0xFF629D:  //vol+
      break;
    case 0xFFE21D:  //func/stop
      break;
    case 0xFF22DD:  //|<<
      cursorX--;
      inputLength--;
      lcd.print(" ");
      lcdInput[inputLength] = 0;
      break;
    case 0xFF02FD:  //>||
      lcd.clear();
      cursorX = 0;
      cursorY = 0;
      if (CheckCode()){
        servo.write(0);
        digitalWrite(greenLED, HIGH);
        delay(1000);
        digitalWrite(greenLED, LOW);
      }
      else{
        digitalWrite(redLED, HIGH);
        delay(1000);
        digitalWrite(redLED, LOW);
      }
      //reset lcdInput
      for (int i = 0; i < 32; i ++){
        lcdInput[i] = 0;
      }
      inputLength = 0;
      break;
    case 0xFFC23D:  //>>|
      break;
    case 0xFFE01F:  //down
      break;
    case 0xFFA857:  //vol-
      break;
    case 0xFF906F:  //up
      break;
    case 0xFF6897:  //0
      lcd.print("0");
      lcdInput[inputLength] = 0;
      cursorX++;
      inputLength++;
      break;
    case 0xFF9867:  //eq
      break;
    case 0xFFB04F:  //st/rept
      break;
    case 0xFF30CF:  //1
      lcd.print("1");
      lcdInput[inputLength] = 1;
      cursorX++;
      inputLength++;
      break;
    case 0xFF18E7:  //2
      lcd.print("2");
      lcdInput[inputLength] = 2;
      cursorX++;
      inputLength++;
      break;
    case 0xFF7A85:  //3
      lcd.print("3");
      lcdInput[inputLength] = 3;
      cursorX++;
      inputLength++;
      break;
    case 0xFF10EF:  //4
      lcd.print("4");
      lcdInput[inputLength] = 4;
      cursorX++;
      inputLength++;
      break;
    case 0xFF38C7:  //5
      lcd.print("5");
      lcdInput[inputLength] = 5;
      cursorX++;
      inputLength++;
      break;
    case 0xFF5AA5:  //6
      lcd.print("6");
      lcdInput[inputLength] = 6;
      cursorX++;
      inputLength++;
      break;
    case 0xFF42BD:  //7
      lcd.print("7");
      lcdInput[inputLength] = 7;
      cursorX++;
      inputLength++;
      break;
    case 0xFF4AB5:  //8
      lcd.print("8");
      lcdInput[inputLength] = 8;
      cursorX++;
      inputLength++;
      break;
    case 0xFF52AD:  //9
      lcd.print("9");
      lcdInput[inputLength] = 9;
      cursorX++;
      inputLength++;
      break;
  }
}