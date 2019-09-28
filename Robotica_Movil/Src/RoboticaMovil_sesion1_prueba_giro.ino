#include <SoftwareSerial.h>

#define BTPWR 22
#define BTTX 0
#define BTRX 1

#define USDPWR 27
#define USDECHO 40
#define USDTRIG 41

#define USDPWR 28
#define USDECHO 42
#define USDTRIG 43

#define END 2
#define IN3 23
#define IN4 24

#define ENI 3
#define IN1 25
#define IN2 26

SoftwareSerial BT1(BTTX, BTRX); // TX del bluetooth al pin 10, RX del bluetooth al pin 11
char input;

void setup() {
  Serial.begin(9600);
  BT1.begin(9600);
  
  pinMode(END,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  
  pinMode(ENI,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);

/////////////////////////////////////
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(END,100);
  analogWrite(ENI,100);

  delay(3000);
  analogWrite(END,0);
  analogWrite(ENI,0);
  
}

void loop() {
  
}
