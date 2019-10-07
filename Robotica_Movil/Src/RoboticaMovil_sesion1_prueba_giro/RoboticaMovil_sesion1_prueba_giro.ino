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

float duracionD, duracionI;

char input;

int Modo = 0;   // 0 -> parado   
                // 1 -> control fontal
                // 2 -> control frontal perpendicular
                // 3 -> paralelo lateral a 50 cm
                // 4 -> paralelo lateral con referencia ajustable

// Referencias iniciales
int Ref_dist = 30;
int Ref_dif = 0;

// Constantes de control
//Modo 1
  // Control de distancia
  double Kp_dist_1 = 10;
  double Kd_dist_1 = 2;
  double Ki_dist_1 = 3;

//Modo 2
  // Control de distancia
  double Kp_dist_2 = 10;
  double Kd_dist_2 = 2;
  double Ki_dist_2 = 3;

  // Control de angulo
  double Kp_dif_2 = 7;
  double Kd_dif_2 = 0;
  double Ki_dif_2 = 0;

//Modo 3
  // Control de distancia
  double Kp_dist_3 = 0;
  double Kd_dist_3 = 0;
  double Ki_dist_3 = 0;

  // Control de angulo
  double Kp_dif_3 = 0;
  double Kd_dif_3 = 0;
  double Ki_dif_3 = 0;

//Modo 4
  // Control de distancia
  double Kp_dist_4 = 0;
  double Kd_dist_4 = 0;
  double Ki_dist_4 = 0;

  // Control de angulo
  double Kp_dif_4 = 0;
  double Kd_dif_4 = 0;
  double Ki_dif_4 = 0;

void setup() {
  Serial.begin(38400);
  
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
  pinMode(BTPWR, OUTPUT);
  digitalWrite(BTPWR, HIGH);
  pinMode(USDPWR, OUTPUT);
  pinMode(USIPWR, OUTPUT);
  digitalWrite(USDPWR, HIGH);
  digitalWrite(USIPWR, HIGH);

  tiempo = millis();
}

int leer_numero() {
  int numero = 0;
  while(Serial.available()) {
    numero = numero * 10 + (Serial.read() - '0');
  }
  return numero;
}

void loop() {
  timePrev = tiempo;
  tiempo = millis();
  elapsedTime = (tiempo - timePrev) / 1000;

  if (Serial.available()) {
    if(Serial.read() == 'm' || Serial.read() == 'M') {
      while(!(Serial.available()));
        input = leer_numero();
        if(0 <= input && input <= 4)  Modo = input;
    }
    if(Serial.read() == 'r' || Serial.read() == 'R') {
      while(!(Serial.available()));
        Ref_dist = leer_numero();        
    }
  }

  // Lectura de los ultrasonidos
  digitalWrite(USDTRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(USDTRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(USDTRIG, LOW);
  
  duracionD = pulseIn(USDECHO, HIGH);
  
  digitalWrite(USITRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(USITRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(USITRIG, LOW);
  
  duracionI = pulseIn(USIECHO, HIGH);

  DD = duracionD * 10 / 292 / 2;
  DI = duracionI * 10 / 292 / 2;

  switch(Modo){
    case 0:
      u_D = 0;
      u_I = 0;
      break;
      
    case 1:
      D = (DD + DI)/2;
      prev_error_dist = error_dist;
      error_dist = D - Ref_dist;
      int_error_dist += elapsedTime * error_dist;
      u_dist = Kp_dist_1 * error_dist + Ki_dist_1 * int_error_dist + Kd_dist_1 * (error_dist - prev_error_dist) / elapsedTime;
    
      u_D = u_dist;
      u_I = u_dist;
    
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
    
      // Anti-windup
      if (u_dist < -255 || 255 < u_dist)  int_error_dist -= elapsedTime * error_dist;
    
      // Saturaciones
      if (u_D > 255)  u_D = 255;
      else if (u_D < - 255) u_D = - 255;
      if (u_I > 255)  u_I = 255;
      else if (u_I < - 255) u_I = - 255;
      
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
      
    case 2:
      D = (DD + DI)/2;
      prev_error_dist = error_dist;
      error_dist = D - Ref_dist;
      int_error_dist += elapsedTime * error_dist;
      u_dist = Kp_dist_2 * error_dist + Ki_dist_2 * int_error_dist + Kd_dist_2 * (error_dist - prev_error_dist) / elapsedTime;
    
      D_dif = DD - DI;
      prev_error_dif = error_dif;
      error_dif = Ref_dif - D_dif;
      int_error_dif += elapsedTime * error_dif;
      u_dif = Kp_dif_2 * error_dif + Ki_dif_2 * int_error_dif + Kd_dif_2 * (error_dif - prev_error_dif) / elapsedTime;
    
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
    
      // Anti-windup
      if (u_dist < -255 || 255 < u_dist)  int_error_dist -= elapsedTime * error_dist;
      if (u_dif < -255 || 255 < u_dif)  int_error_dif -= elapsedTime * error_dif;
    
      // Saturaciones
      if (u_D > 255)  u_D = 255;
      else if (u_D < - 255) u_D = - 255;
      if (u_I > 255)  u_I = 255;
      else if (u_I < - 255) u_I = - 255;
      
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
    case 3:
      break;
      
    case 4:
      break;
      
    default: break;
  }

  Serial.print(elapsedTime * 1000);
  Serial.print(" ");
  
  Serial.print(DI);
  Serial.print(" ");

  Serial.print(DD);
  Serial.print(" ");

  Serial.print(Ref_dist);
  Serial.print(" ");

  Serial.print(Modo);
  Serial.print(" ");

  Serial.print(u_I);
  Serial.print(" ");

  Serial.println(u_D);
}
