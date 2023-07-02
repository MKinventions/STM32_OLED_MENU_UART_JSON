#include <EEPROM.h>

//int led = PC13;
//int btn = PA2;
int pot1 = PA3;
int pot2 = PA2;

int joy1 = PA1;
int joy2 = PB0;
int joy3 = PA4;
int joy4 = PA0;

int joybtn1 = PA8;
int joybtn2 = PB15;
int rotbtn1 = PB14;
int rotbtn2 = PB13;
 

int pages = PB12;
int menu = PC13;
int submenu = PC14;
int btn = PA11;




void setup() {
  Serial.begin(115200);
//pinMode(btn, INPUT_PULLUP);
pinMode(pot1, INPUT);
pinMode(pot2, INPUT);
pinMode(joy1, INPUT);
pinMode(joy2, INPUT);
pinMode(joy3, INPUT);
pinMode(joy4, INPUT);
//pinMode(led, OUTPUT);

pinMode(joybtn1, INPUT_PULLUP);
pinMode(joybtn2, INPUT_PULLUP);
pinMode(rotbtn1, INPUT_PULLUP);
pinMode(rotbtn2, INPUT_PULLUP);

pinMode(pages, INPUT_PULLUP);
pinMode(menu, INPUT_PULLUP);
pinMode(submenu, INPUT_PULLUP);
pinMode(btn, INPUT_PULLUP);

}

void loop() {


//int button1 = digitalRead(btn);
//uint16_t potentiometer1 = analogRead(pot1);

int potentiometer1 = map(analogRead(pot1), 4095, 0, 255, 0);
int potentiometer2 = map(analogRead(pot2), 4095, 0, 255, 0);
int joystick1 = map(analogRead(joy1), 4095, 0, 255, 0);
int joystick2 = map(analogRead(joy2), 4095, 0, 255, 0);
int joystick3 = map(analogRead(joy3), 4095, 0, 255, 0);
int joystick4 = map(analogRead(joy4), 4095, 0, 255, 0);

int button1 = digitalRead(joybtn1);
int button2 = digitalRead(joybtn2);
int button3 = digitalRead(rotbtn1);
int button4 = digitalRead(rotbtn2);

int nextpage = digitalRead(pages);
int menuSelect = digitalRead(menu);
int subMenuSelect = digitalRead(submenu);
int button5 = digitalRead(btn);


//Serial.println("POT1:"+String(potentiometer1)+", POT2:"+String(potentiometer2)+", JOY1:"+String(joystick1)+", JOY2:"+String(joystick2)+", JOY3:"+String(joystick3)+", JOY4:"+String(joystick4));
//Serial.println("JOY4:"+String(joystick4));
Serial.println("BTN1:"+String(button1)+", BTN2:"+String(button2)+", BTN3:"+String(button3)+", BTN4:"+String(button4));
//Serial.println("PAGES:"+String(nextpage)+", MENU:"+String(menuSelect)+", SUBMENU:"+String(subMenuSelect)+", BTN:"+String(button5));

delay(100);
}
