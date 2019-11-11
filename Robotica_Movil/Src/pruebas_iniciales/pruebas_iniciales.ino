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

void setup() {
  Serial.begin(38400);
  
  pinMode(END,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  
  pinMode(ENI,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);

  tiempo = millis();
  Modo=0;
}

void loop() {
  timePrev = tiempo;
  tiempo = millis();
  elapsedTime = (tiempo - timePrev) / 1000;

  switch(Modo){
    case 0: //avanzar en linea recta
      
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      
      analogWrite(END, 100);
      analogWrite(ENI, 100);

      delay(2000);
      Modo=1;
      break;
      
    case 1: //retroceder

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      
      analogWrite(END, 100);
      analogWrite(ENI, 100);

      delay(2000);
      Modo=2;
      break;
      
    case 2: //sentido antihorario
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      
      analogWrite(END, 150);
      analogWrite(ENI, 150);

      delay(2000);
      Modo=3;
      break;
    case 3: //sentido horario
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      
      analogWrite(END, 150);
      analogWrite(ENI, 150);
      delay(2000);
      Modo=4;
      break;
      
    case 4: //moverse y girar izquierda
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      
      analogWrite(END, 200);
      analogWrite(ENI, 100);

      delay(2000);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      
      analogWrite(END, 200);
      analogWrite(ENI, 100);

      delay(2000);
      Modo=5;
      
      break;
      case 5: //moverse y girar derecha
      
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      
      analogWrite(END, 100);
      analogWrite(ENI, 200);

      delay(2000);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      
      analogWrite(END, 100);
      analogWrite(ENI, 200);

      delay(2000);
      Modo=0;
      
      break;
      
    default: 
      u_D = 0;
      u_I = 0;
      
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
  }

}
