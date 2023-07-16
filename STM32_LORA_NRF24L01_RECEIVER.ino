#include<EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <ArduinoJson.h>
#include <Servo.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const int nextPageButton = PB11;
const int MainMenuButton = PB1;
const int SubMenuButton = PB10;

int pagesCount = 0;
int MainMenuSelect = 0;
int SubMenuSelect = 0;

uint8_t totalPages = 6;
uint8_t totalMenus = 10;
uint8_t totalSubMenus = 4;


int forwardLeft_address = 0x01;
int forwardRight_address = 0x02;
int backwardLeft_address = 0x03;
int backwardRight_address = 0x04;
int leftwardLeft_address = 0x05;
int leftwardRight_address = 0x06;
int rightwardLeft_address = 0x07;
int rightwardRight_address = 0x08;
int pageCount_address = 0x17;

int servo1_address = 0x18;
int servo2_address = 0x19;

int receiverMode_address = 0x20;
int sendData_address = 0x21;


byte forwardLeft_value = 0;
byte forwardRight_value = 0;
byte backwardLeft_value = 0;
byte backwardRight_value = 0;
byte leftwardLeft_value = 0;
byte leftwardRight_value = 0;
byte rightwardLeft_value = 0;
byte rightwardRight_value = 0;
byte servo1_value = 0;
byte servo2_value = 0;
byte receiverMode_value = 0;
byte sendData_value = 0;

String receiverMode_Type;
String sendData_Type;

#define LED1 PC13
#define LED2 PC14
#define LED3 PC15
#define LED4 PA0
#define IN1 PB12
#define IN2 PB13
#define IN3 PB14
#define IN4 PB15
#define LEFT_SERVO PA1
#define RIGHT_SERVO PA2

Servo myservo1;
Servo myservo2;


//LORA***********************
const int csPin = PB8;          // LoRa radio chip select
const int resetPin = PB9;       // LoRa radio reset
const int irqPin = PB5;         // change for your board; must be a hardware interrupt pin
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xBB;     // address of this device
byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
unsigned long previousMillisTX = 0;

//NRF24L01*********************
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(PA11, PA12); // CE, CSN=>STM32
const byte address[6] = "12345";



  int joyx_state;
  int joyy_state;
  int pot1_state;
  int pot2_state;
  int btn1_state;
  int btn2_state;
  int btn3_state;
  int btn4_state;

  int joy1 = 0;
  int joy2 = 0;
  int pot1 = 0;
  int pot2 = 0;
  int btn1 = 0;
  int btn2 = 0;
  int btn3 = 0;
  int btn4 = 0;
  int rot = 0;
  int servo1;
  int servo2;
  
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(115200);
  pinMode(nextPageButton, INPUT_PULLUP);
  pinMode(MainMenuButton, INPUT_PULLUP);
  pinMode(SubMenuButton, INPUT_PULLUP);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  myservo1.attach(LEFT_SERVO);
  myservo2.attach(RIGHT_SERVO);
  
lora_init();
NRF24L01_init();
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(35, 30);
  display.println("Restarted");
  display.display();

  forwardLeft_value = EEPROM.read(forwardLeft_address);//EEPROM.read(ADDRESS 0-255);
  forwardRight_value = EEPROM.read(forwardRight_address);//EEPROM.read(ADDRESS 0-255);
  backwardLeft_value = EEPROM.read(backwardLeft_address);//EEPROM.read(ADDRESS 0-255);
  backwardRight_value = EEPROM.read(backwardRight_address);//EEPROM.read(ADDRESS 0-255);
  leftwardLeft_value = EEPROM.read(leftwardLeft_address);//EEPROM.read(ADDRESS 0-255);
  leftwardRight_value = EEPROM.read(leftwardRight_address);//EEPROM.read(ADDRESS 0-255);
  rightwardLeft_value = EEPROM.read(rightwardLeft_address);//EEPROM.read(ADDRESS 0-255);
  rightwardRight_value = EEPROM.read(rightwardRight_address);//EEPROM.read(ADDRESS 0-255);
  pagesCount = EEPROM.read(pageCount_address);//EEPROM.read(ADDRESS 0-255); 

  receiverMode_value = EEPROM.read(receiverMode_address);//EEPROM.read(ADDRESS 0-255);
  sendData_value = EEPROM.read(sendData_address);//EEPROM.read(ADDRESS 0-255);
}





void loop() 
{

int receiverMode = 0;

if(sendData_value == 1){
   sendDataToMaster();
//vTaskDelay(10);
}

if(receiverMode_value == 0){
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
} 
if(receiverMode_value == 1){
//NRF24L01****
receiveNrf24Data();
}
  
    if (digitalRead(nextPageButton) == LOW) {
      pagesCount++;
      if (pagesCount > 6) {
        pagesCount = 0;
      }
    }

    if (digitalRead(MainMenuButton) == LOW) {
      MainMenuSelect++;
      if (MainMenuSelect > 8) {
        MainMenuSelect = 0;
      }
   }
    
    if (digitalRead(SubMenuButton) == LOW) {
      SubMenuSelect++;
      if (SubMenuSelect > 2) {
        SubMenuSelect = 0;
      }
    }
  
  delay(100);

//  Serial.println("PAGES:"+String(pagesCount));
//  Serial.println("MENU:"+String(MainMenuSelect));
//  Serial.println("SUBMENU:"+String(SubMenuSelect));


receiverMode_Type = (receiverMode_value == 0)?"LORA":"NRF24";
sendData_Type = (sendData_value == 0)?"NO":"YES";
    
  
 
  
  switch(pagesCount){
  
    case 0:
            EEPROM.write(pageCount_address, 0);
            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setTextSize(1);
            display.setCursor(0, 0);//COL,ROW
            display.println("DASHBOARD");
            display.setCursor(110,0);display.print(String(pagesCount) + "/" + String(totalPages));
            display.setCursor(0,12); display.print("RSSI:"+ String(LoRa.packetRssi()) +   " ,SNR:"+ String(LoRa.packetSnr()));   
            display.setCursor(0,21); display.print("B1:"+ String(btn1) +   " ,B2:"+ String(btn2));   
            display.setCursor(0,31); display.print("S1:"+ String(btn3) +  " ,S2:"+ String(btn4)); 
            display.setCursor(0,41); display.print("P1:"+ String(pot1) +  " ,P2:"+ String(pot2)+  " ,R:"+ String(rot)); 
            display.setCursor(0,51); display.print("Ser1:"+ String(servo1) +  " ,ser2:"+ String(servo2)); 

//            display.setCursor(0,51); display.print("ROTARY:"+ String(rot)); 
          break;
    
    case 1:
          EEPROM.write(pageCount_address, 1);
          motorSettings();
          break;      
    case 2:
          EEPROM.write(pageCount_address, 2);
          settings();
          break;      
    case 3:
           pagesCount = 0;
          break;                        
  }
 display.display();
}


void NRF24L01_init() {
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void receiveNrf24Data() {
  if (radio.available()) {
    char text[32] = "";//print only 32 characters
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}
void sendDataToMaster(){

      String SendJSONData = "";
      StaticJsonDocument<512> LORA_TX;
   
//    LORA_TX["firmware"] = firmwareVersion;   
    LORA_TX["rssi"] = LoRa.packetRssi();
    LORA_TX["snr"] = LoRa.packetSnr();
    JsonArray button = LORA_TX.createNestedArray("btnState");
    button.add(btn1_state);
    button.add(btn2_state);
    button.add(btn3_state);
    button.add(btn4_state);
    JsonArray potentiometers = LORA_TX.createNestedArray("potState");
    potentiometers.add(pot1_state);
    potentiometers.add(pot2_state);
        
    //  doc["distance"] = (int)random(5) + 30.50;
    //  doc["volt"] = in_voltage;
    
    // //  Add an array.
    //  JsonArray data = doc.createNestedArray("cordinates");
    //  data.add(48.756080);
    //  data.add(2.302038);
      
    serializeJson(LORA_TX, SendJSONData);
    //serializeJsonPretty(doc, SendJSONData);
    
  unsigned long currentMillisTX = millis();
  if (currentMillisTX - previousMillisTX >= 800) {
    previousMillisTX = currentMillisTX;
      Serial.println("Sending " + SendJSONData);
      LoRa.beginPacket();                   // start packet
      LoRa.write(destination);              // add destination address
      LoRa.write(localAddress);             // add sender address
      LoRa.write(msgCount);                 // add message ID
      LoRa.write(SendJSONData.length());        // add payload length
      LoRa.print(SendJSONData);                 // add payload
      LoRa.endPacket();                     // finish packet and send it
      msgCount++;                           // increment message ID
      }   
}


void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }



StaticJsonDocument<512> LORA_RX;
  DeserializationError error = deserializeJson(LORA_RX, incoming);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  
//  //const char* sensor = LORA_RX["sensor"];
//  //long time = LORA_RX["time"];
//  joy1 = LORA_RX["joy"][0];
//  joy2 = LORA_RX["joy"][1];
//  pot1 = LORA_RX["pot"][0];
//  pot2 = LORA_RX["pot"][1];
//  btn1 = LORA_RX["btn"][0];
//  btn2 = LORA_RX["btn"][1];
//  btn3 = LORA_RX["btn"][2];
//  btn4 = LORA_RX["btn"][3];
//  rot = LORA_RX["rot"][0];
//  servo1 = LORA_RX["servo"][0];
//  servo2 = LORA_RX["servo"][1];

  btn1 = LORA_RX["TXdata"][0];
  btn2 = LORA_RX["TXdata"][1];
  btn3 = LORA_RX["TXdata"][2];
  btn4 = LORA_RX["TXdata"][3];
  pot1 = LORA_RX["TXdata"][4];
  pot2 = LORA_RX["TXdata"][5];  
  joy1 = LORA_RX["TXdata"][6];
  joy2 = LORA_RX["TXdata"][7];
  rot = LORA_RX["TXdata"][8];
  servo1 = LORA_RX["TXdata"][9];
  servo2 = LORA_RX["TXdata"][10];

if(btn1 == 1){ btn1_state = 0; digitalWrite(LED1, HIGH);}
if(btn1 == 0){ btn1_state = 1; digitalWrite(LED1, LOW);}

if(btn2 == 1){ btn2_state = 0; digitalWrite(LED2, HIGH);}
if(btn2 == 0){ btn2_state = 1; digitalWrite(LED2, LOW);}



if(btn3 == 1){ btn3_state = 0; digitalWrite(LED3, HIGH);}
if(btn3 == 0){ btn3_state = 1; digitalWrite(LED3, LOW);}

if(btn4 == 1){ btn4_state = 0; digitalWrite(LED4, HIGH);}
if(btn4 == 0){ btn4_state = 1; digitalWrite(LED4, LOW);}

btn1_state = (btn1_state == 1)?0:1;
btn2_state = (btn2_state == 1)?0:1;
btn3_state = (btn3_state == 1)?0:1;
btn4_state = (btn4_state == 1)?0:1;
pot1_state = pot1;
pot2_state = pot2;
joyx_state = joy1;
joyy_state = joy2;


myservo1.write(pot1);
myservo2.write(pot2);


if(joy1 > 180){
Motors_Movement(leftwardLeft_value,leftwardRight_value);
Serial.println();
}
else if(joy1 < 80){
Motors_Movement(rightwardLeft_value,rightwardRight_value);
}
else if(joy2 > 180){
Motors_Movement(forwardLeft_value,forwardRight_value);
}
else if(joy2 < 80){
Motors_Movement(backwardLeft_value,backwardRight_value);
}
else{
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
}


  // if message is for this device, or broadcast, print details:
//  Serial.print("Received from: 0x" + String(sender, HEX));
//  Serial.print(", Sent to: 0x" + String(recipient, HEX));
//  Serial.print(", Message ID: " + String(incomingMsgId));
//  Serial.print(", Message length: " + String(incomingLength));
//  Serial.print(", RSSI: " + String(LoRa.packetRssi()));
//  Serial.println(", Snr: " + String(LoRa.packetSnr()));
  Serial.println("Incoming Message: " + incoming);
//  Serial.println();
  
}

void lora_init(){
      Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
}

void Motors_Movement(int directionLeft, int directionRight){
 
   if(directionLeft == 0){
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      Serial.println("left motor forward");
  }
   if(directionLeft == 1){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      Serial.println("left motor backward");
  }
   if(directionRight == 0){
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      Serial.println("right motor forward");
  }
   if(directionRight == 1){
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      Serial.println("right motor backward");
  }
}

void settings(){
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(0, 0);//COL,ROW
          display.println("SETTINGS");
          display.setCursor(110,0);display.print(String(pagesCount) + "/" + String(totalPages));
          
          display.setCursor(0,12); display.print(" RECEIVE MODE:" + String(receiverMode_Type));   
          display.setCursor(0,21); display.print(" SEND DATA   :" + String(sendData_Type)); 

           switch(MainMenuSelect){
            case 0:
                  display.setCursor(0,55);display.print("Select Setting Type");
                  break;          
              
          case 1:
                 display.setCursor(0,12); display.print(">RECEIVE MODE:" + String(receiverMode_Type));
                 switch(SubMenuSelect){
                     case 0:
                            display.setCursor(10,55);display.print("  LORA   NRF24L01");
                            break;
                     case 1:
                            EEPROM.write(receiverMode_address, 0);
                            display.setCursor(10,55);display.print(" >LORA   NRF24L01");
                            break;
                     case 2:
                            EEPROM.write(receiverMode_address, 1);
                            display.setCursor(10,55);display.print("  LORA  >NRF24L01");
                            break;
                     case 3:
                           SubMenuSelect = 0;
                           break;                                 
              }  
              break;                   
          case 2:
                 display.setCursor(0,21); display.print(">SEND DATA   :" + String(sendData_Type));
                 switch(SubMenuSelect){
                     case 0:
                            display.setCursor(10,55);display.print("  YES   NO");
                            break;
                     case 1:
                            EEPROM.write(sendData_address, 1);
                            display.setCursor(10,55);display.print(" >YES   NO");
                            break;
                     case 2:
                            EEPROM.write(sendData_address, 0);
                            display.setCursor(10,55);display.print("  YES  >NO");
                            break;     
                     case 3:
                           SubMenuSelect = 0;
                           break; 
                 }  
              break;  
                
                       
         case 3:
                  MainMenuSelect = 0;
                  break;   
              }  
       display.display();        
}

void motorSettings(){
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(0, 0);//COL,ROW
          display.println("MOTORS");
          display.setCursor(110,0);display.print(String(pagesCount) + "/" + String(totalPages));

          display.setCursor(0,12); display.print("FORWARD  : " + String(forwardLeft_value) +   ",  "+ String(forwardRight_value));   
          display.setCursor(0,21); display.print("BACKWARD : " + String(backwardLeft_value) +  ",  "+ String(backwardRight_value)); 
          display.setCursor(0,31); display.print("LEFTWARD : " + String(leftwardLeft_value) +  ",  "+ String(leftwardRight_value)); 
          display.setCursor(0,41); display.print("RIGHTWARD: " + String(rightwardLeft_value) + ",  "+ String(rightwardRight_value)); 

            switch(MainMenuSelect){
            case 0:
                  display.setCursor(0,55);display.print("Press To Select Menu");
                  break;       
            case 1:
                  display.setCursor(0,12); display.print("FORWARD  :>" + String(forwardLeft_value) +   ",  "+ String(forwardRight_value));
                  switch(SubMenuSelect){
                         case 0:
                                display.setCursor(30,55);display.print("  0  1");
                                break;
                         case 1:
                                EEPROM.write(forwardLeft_address, 0);
                                display.setCursor(30,55);display.print(" >0  1");
                                break;
                         case 2:
                                EEPROM.write(forwardLeft_address, 1);
                                display.setCursor(30,55);display.print("  0 >1");
                                break;     
                  }  
                  break;                   
            case 2:
                  display.setCursor(0,12); display.print("FORWARD  : " + String(forwardLeft_value) +   ", >"+ String(forwardRight_value));
                  switch(SubMenuSelect){
                         case 0:
                                display.setCursor(30,55);display.print("  0  1");
                                break;
                         case 1:
                                EEPROM.write(forwardRight_address, 0);
                                display.setCursor(30,55);display.print(" >0  1");
                                break;
                         case 2:
                                EEPROM.write(forwardRight_address, 1);
                                display.setCursor(30,55);display.print("  0 >1");
                                break;     
                  }  
                  break;  
            case 3:
                  display.setCursor(0,21); display.print("BACKWARD :>" + String(backwardLeft_value) +  ",  "+ String(backwardRight_value));
                  switch(SubMenuSelect){
                         case 0:
                                display.setCursor(30,55);display.print("  0  1");
                                break;
                         case 1:
                                EEPROM.write(backwardLeft_address, 0);
                                display.setCursor(30,55);display.print(" >0  1");
                                break;
                         case 2:
                                EEPROM.write(backwardLeft_address, 1);
                                display.setCursor(30,55);display.print("  0 >1");
                                break;     
                  }  
                  break;  
            case 4:
                  display.setCursor(0,21); display.print("BACKWARD : " + String(backwardLeft_value) +  ", >"+ String(backwardRight_value));
                  switch(SubMenuSelect){
                         case 0:
                                display.setCursor(30,55);display.print("  0  1");
                                break;
                         case 1:
                                EEPROM.write(backwardRight_address, 0);
                                display.setCursor(30,55);display.print(" >0  1");
                                break;
                         case 2:
                                EEPROM.write(backwardRight_address, 1);
                                display.setCursor(30,55);display.print("  0 >1");
                                break;     
                  }  
                  break;  


            case 5:
                  display.setCursor(0,31); display.print("LEFTWARD :>" + String(leftwardLeft_value) +  ",  "+ String(leftwardRight_value));
                  switch(SubMenuSelect){
                         case 0:
                                display.setCursor(30,55);display.print("  0  1");
                                break;
                         case 1:
                                EEPROM.write(leftwardLeft_address, 0);
                                display.setCursor(30,55);display.print(" >0  1");
                                break;
                         case 2:
                                EEPROM.write(leftwardLeft_address, 1);
                                display.setCursor(30,55);display.print("  0 >1");
                                break;     
                  }  
                  break;                   

            case 6:
                  display.setCursor(0,31); display.print("LEFTWARD : " + String(leftwardLeft_value) +  ", >"+ String(leftwardRight_value));
                  switch(SubMenuSelect){
                         case 0:
                                display.setCursor(30,55);display.print("  0  1");
                                break;
                         case 1:
                                EEPROM.write(leftwardRight_address, 0);
                                display.setCursor(30,55);display.print(" >0  1");
                                break;
                         case 2:
                                EEPROM.write(leftwardRight_address, 1);
                                display.setCursor(30,55);display.print("  0 >1");
                                break;     
                  }  
                  break;   
            case 7:
                  display.setCursor(0,41); display.print("RIGHTWARD:>" + String(rightwardLeft_value) + ",  "+ String(rightwardRight_value));
                  switch(SubMenuSelect){
                         case 0:
                                display.setCursor(30,55);display.print("  0  1");
                                break;
                         case 1:
                                EEPROM.write(rightwardLeft_address, 0);
                                display.setCursor(30,55);display.print(" >0  1");
                                break;
                         case 2:
                                EEPROM.write(rightwardLeft_address, 1);
                                display.setCursor(30,55);display.print("  0 >1");
                                break;     
                  }  
                  break;   
            case 8:
                  display.setCursor(0,41); display.print("RIGHTWARD: " + String(rightwardLeft_value) + ", >"+ String(rightwardRight_value));
                  switch(SubMenuSelect){
                         case 0:
                                display.setCursor(30,55);display.print("  0  1");
                                break;
                         case 1:
                                EEPROM.write(rightwardRight_address, 0);
                                display.setCursor(30,55);display.print(" >0  1");
                                break;
                         case 2:
                                EEPROM.write(rightwardRight_address, 1);
                                display.setCursor(30,55);display.print("  0 >1");
                                break;     
                  }  
                  break;                                 
            case 9:
                  MainMenuSelect = 0;
                  break;                         
                  }
        display.display(); 
     
}
