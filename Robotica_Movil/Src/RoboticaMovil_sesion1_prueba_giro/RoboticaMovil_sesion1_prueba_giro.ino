#include <SoftwareSerial.h>

#define BTPWR 22
#define BTTX 0
#define BTRX 1

#define USDPWR 27
#define USDECHO 40
#define USDTRIG 41

#define USIPWR 28
#define USIECHO 42
#define USITRIG 43

#define END 2
#define IN3 23
#define IN4 24

#define ENI 3
#define IN1 25
#define IN2 26

float elapsedTime, tiempo, timePrev;
float error_dist = 0;
float prev_error_dist = 0;
float int_error_dist = 0;

float error_dif = 0;
float prev_error_dif = 0;
float int_error_dif = 0;

float u_dif;
float u_dist;
float u_D;
float u_I;


int D;   // distancia del centro a la pared en linea recta
int D_dif; // distancia diferencial
int DD;  // distancia del sensor ultrasonidos derecho a la pared en linea recta
int DI;  // distancia del sensor ultrasonidos izquierdo a la pared en linea recta

SoftwareSerial BT1(BTTX, BTRX); // TX del bluetooth al pin 10, RX del bluetooth al pin 11
char input;

int Modo = 0;   // 0 -> parado   
                // 1 -> control fontal
                // 2 -> control frontal perpendicular
                // 3 -> paralelo lateral a 50 cm
                // 4 -> paralelo lateral con referencia ajustable

int Ref_dist = 30;
int Ref_dif = 0;

double Kp_dist = 0;
double Kd_dist = 0;
double Ki_dist = 0;

double Kp_dif = 25;
double Kd_dif = 0;
double Ki_dif = 0;

void setup() {
  Serial.begin(9600);
  BT1.begin(9600);
  
  pinMode(END,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  
  pinMode(ENI,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);

  pinMode(USDECHO, INPUT);
  pinMode(USDTRIG, OUTPUT);
  pinMode(USIECHO, INPUT);
  pinMode(USITRIG, OUTPUT);

  delay(300);
  digitalWrite(BTPWR, HIGH);
  digitalWrite(USDPWR, HIGH);
  digitalWrite(USDPWR, HIGH);

  tiempo = millis();
}

void loop() {
  timePrev = tiempo;
  tiempo = millis();
  elapsedTime = (tiempo - timePrev) / 1000;
  
  digitalWrite(USDTRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(USDTRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(USDTRIG, LOW);
  
  DD = pulseIn(USDECHO, HIGH) * 10 / 292 / 2;
  
  digitalWrite(USITRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(USITRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(USITRIG, LOW);
  
  DI = pulseIn(USIECHO, HIGH) * 10 / 292 / 2;

  D = (DD + DI)/2;
  prev_error_dist = error_dist;
  error_dist = Ref_dif - D;
  int_error_dist += elapsedTime * error_dist;
  u_dist = Kp_dist * error_dist + Ki_dist * int_error_dist + Kd_dist * (error_dist - prev_error_dist) / elapsedTime;

  D_dif = DD - DI;
  prev_error_dif = error_dif;
  error_dif = Ref_dif - D_dif;
  int_error_dif += elapsedTime * error_dif;
  u_dif = Kp_dif * error_dif + Ki_dif * int_error_dif + Kd_dif * (error_dif - prev_error_dif) / elapsedTime;

  u_D = (u_dist + u_dif/2);
  u_I = (u_dist - u_dif/2);

  if (u_D > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  
  if (u_I > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }

  if (u_D > 255)  u_D = 255;
  else if (u_D < - 255) u_D = - 255;
  if (u_I > 255)  u_I = 255;
  else if (u_I < - 255) u_I = - 255;
  
  analogWrite(END, abs(u_D));
  analogWrite(ENI, abs(u_I));

  Serial.print(elapsedTime * 1000);
  Serial.print("\t");
  
  Serial.print(DI);
  Serial.print("\t");

  Serial.print(DD);
  Serial.print("\t");

  Serial.print(Ref_dist);
  Serial.print("\t");

  Serial.print(Modo);
  Serial.print("\t");

  Serial.print(u_I);
  Serial.print("\t");

  Serial.print(u_D);
  Serial.print("\t");
}
