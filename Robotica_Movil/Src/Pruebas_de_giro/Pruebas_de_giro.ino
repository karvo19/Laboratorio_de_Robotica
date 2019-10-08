#define BTPWR 22
#define BTTX 0
#define BTRX 1

#define USDPWR 27
#define USDECHO 40
#define USDTRIG 41

#define USIPWR 28
#define USIECHO 42
#define USITRIG 43

#define END 5
#define IN3 23
#define IN4 24

#define ENI 6
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

char input[8];
int i = 0;

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
  double Ki_dist_1 = 7;

//Modo 2
  // Control de distancia
  double Kp_dist_2 = 0;
  double Kd_dist_2 = 0;
  double Ki_dist_2 = 0;

  // Control de angulo
  double Kp_dif_2 = 60;
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

void leer_comando() {
  int numero = 0;
  if(input[0] == 'r' || input[0] == 'R') {
    for(int j = 0; j < 8; j++) {
      if(input[j] == '.') {
        j = 8;
      }
      else if(47 < input[j] && input[j] < 58) {
        numero = numero * 10 + (input[j] - '0');
      }
    }
    if(5 <= numero) Ref_dist = numero;
  }
  if(input[0] == 'm' || input[0] == 'M') {
    for(int j = 0; j < 8; j++) {
      if(input[j] == '.') {
        j = 8;
      }
      else if(47 < input[j] && input[j] < 58) {
        numero = numero * 10 + (input[j] - '0');
      }
    }
    Modo = numero;
  }
  if(input[0] == 'p' || input[0] == 'P') {
    for(int j = 0; j < 8; j++) {
      if(input[j] == '.') {
        j = 8;
      }
      else if(47 < input[j] && input[j] < 58) {
        numero = numero * 10 + (input[j] - '0');
      }
    }
    Kp_dif_2 = numero;
  }
  if(input[0] == 'i' || input[0] == 'I') {
    for(int j = 0; j < 8; j++) {
      if(input[j] == '.') {
        j = 8;
      }
      else if(47 < input[j] && input[j] < 58) {
        numero = numero * 10 + (input[j] - '0');
      }
    }
    Ki_dif_2 = numero;
  }
  if(input[0] == 'd' || input[0] == 'D') {
    for(int j = 0; j < 8; j++) {
      if(input[j] == '.') {
        j = 8;
      }
      else if(47 < input[j] && input[j] < 58) {
        numero = numero * 10 + (input[j] - '0');
      }
    }
    Kd_dif_2 = numero;
  }
  for(int j = 0; j < 8; j++)
    input[j] = '.';
}

void loop() {
  timePrev = tiempo;
  tiempo = millis();
  elapsedTime = (tiempo - timePrev) / 1000;

  if (Serial.available()) {
    input[i] = Serial.read();
    if(input[i] == '.') {
      leer_comando();
      i = 0;
    }
    else i ++;    
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
      
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
      
    case 1:
      u_D = 100;
      u_I = 0;
      
      if(u_D == 0) {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
      }
      else if (u_D > 0) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      }
      else {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }
      
      if(u_I == 0) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
      }
      else if (u_I > 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
      }
      else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
      }
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
      
    case 2:
      u_D = 0;
      u_I = 100;
      
      if(u_D == 0) {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
      }
      else if (u_D > 0) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      }
      else {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }
      
      if(u_I == 0) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
      }
      else if (u_I > 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
      }
      else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
      }
      
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
    case 3:
      u_D = 50;
      u_I = -50;
      
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
      
    case 4:
      u_D = -50;
      u_I = 50;
      
      if(u_D == 0) {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
      }
      else if (u_D > 0) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      }
      else {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }
      
      if(u_I == 0) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
      }
      else if (u_I > 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
      }
      else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
      }
      
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
      
    default: 
      break;
  }
}
