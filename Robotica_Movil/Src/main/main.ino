/* DEFINICIONES DE LOS PUERTOS */
// Bluetooth
#define BTPWR 22
#define BTTX 0
#define BTRX 1

// Ultrasonidos derecho
#define USDPWR 27
#define USDECHO 40
#define USDTRIG 41

// Ultrasonidos izquierdo
#define USIPWR 28
#define USIECHO 42
#define USITRIG 43

// Servomotor derecho
#define END 5
#define IN3 23
#define IN4 24

// Servomotor izquierdo
#define ENI 6
#define IN1 25
#define IN2 26

/* VARIABLES PARA LA EJECUCION DEL PROGRAMA */
// Variales para medir el tiempo
float elapsedTime, tiempo, timePrev;

// Variables para almacena el error en distancia
float error_dist = 0;
float prev_error_dist = 0;
float int_error_dist = 0;

// Variables para almacenar el error diferencial
float error_dif = 0;
float prev_error_dif = 0;
float int_error_dif = 0;

// Señales de control
float u_dif;    // señal de control diferencial
float u_dist;   // señal de control en distancia
float u_D;      // señal de control del servo derecho
float u_I;      // señal de control del servo izquierdo

// Velocidad para los modos de movimiento continuo
float velocidad = 180;

// Distancias
int D;   // distancia del centro a la pared en linea recta
int D_dif; // distancia diferencial
int DD;  // distancia del sensor ultrasonidos derecho a la pared en linea recta
int DI;  // distancia del sensor ultrasonidos izquierdo a la pared en linea recta

// Variables para la lectura de los ultrasonidos
float duracionD, duracionI;

/* Variables para la comunicacion por bluetooth */
char input[8];
int i = 0;

/* DESCRIPCION DE LOS MODOS DE FUNCIONAMIENTO */
int Modo = 0;   // 0 -> parado   
                // 1 -> control fontal
                // 2 -> control frontal perpendicular
                // 3 -> paralelo lateral a 50 cm
                // 4 -> paralelo lateral con referencia ajustable

/* REFERENCIAS INICIALES */
int Ref_dist = 30;  // referencia en distancia
int Ref_dif = 0;    // referencia diferencial

/* GANANCIAS DE CONTROL */
// MODO 1
  // Control de distancia
  double Kp_dist_1 = 10;
  double Kd_dist_1 = 2;
  double Ki_dist_1 = 7;

// MODO 2
  // Control de distancia
  double Kp_dist_2 = 0;
  double Kd_dist_2 = 0;
  double Ki_dist_2 = 0;

  // Control de angulo
  double Kp_dif_2 = 80;
  double Kd_dif_2 = 5;
  double Ki_dif_2 = 1;

// MODO 3
  // Control de distancia
  double Kp_dist_3 = 0;
  double Kd_dist_3 = 0;
  double Ki_dist_3 = 0;

  // Control de angulo
  double Kp_dif_3 = 0;
  double Kd_dif_3 = 0;
  double Ki_dif_3 = 0;

// MODO 4
  // Control de distancia
  double Kp_dist_4 = 0;
  double Kd_dist_4 = 0;
  double Ki_dist_4 = 0;

  // Control de angulo
  double Kp_dif_4 = 0;
  double Kd_dif_4 = 0;
  double Ki_dif_4 = 0;

// Variables necesaria para actualizar las ganancias desde comandos
  int flag_ganancias = 0;
  // Control de distancia
  double Kp_dist_aux = 0;
  double Kd_dist_aux = 0;
  double Ki_dist_aux = 0;


void setup() {
  // Establecemos la velocidad de comunicacion del bluetooth 
  Serial1.begin(38400);

  // Configuracion de los Pines del Servomotor Derecho
  pinMode(END,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  // Configuracion de los Pines del Servomotor Izquierdo
  pinMode(ENI,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);

  // Configuracion de los pines de los Ultrasonidos
  pinMode(USDECHO, INPUT);
  pinMode(USDTRIG, OUTPUT);
  pinMode(USIECHO, INPUT);
  pinMode(USITRIG, OUTPUT);

  // Delay para evitar conflictos con el puerto serie
  delay(300);

  // Encendido de los ultrasonidos y el bluetooth
  pinMode(BTPWR, OUTPUT);
  digitalWrite(BTPWR, HIGH);
  pinMode(USDPWR, OUTPUT);
  pinMode(USIPWR, OUTPUT);
  digitalWrite(USDPWR, HIGH);
  digitalWrite(USIPWR, HIGH);

  // Lectura del tiempo actual para inicializar el tiempo
  tiempo = millis();
}

/* FUNCION PARA ANALIZAR LOS COMANDOS INTRODUCIDOS */
/* TODOS LOS COMANDOS DEBEN COMENZAR POR UNA LETRA Y ACABAR POR UN PUNTO */
void leer_comando() {
  int numero = 0;
  
  /* CAMBIO DE LA DISTANCIA DE REFERENCIA: Sintaxis: "[R/r][referenca]." */
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

  /* CAMBIO DEL MODO DE FUNCIONAMIENTO: Sintaxis: "[M/m][modo]." */
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

  /* CAMBIO DE LA VELOCIDAD DE AVANCE PARA LOS MODOS 3 Y 4. Sintaxis: "[V/v][velocidad]." */
  if(input[0] == 'v' || input[0] == 'V') {
    for(int j = 0; j < 8; j++) {
      if(input[j] == '.') {
        j = 8;
      }
      else if(47 < input[j] && input[j] < 58) {
        numero = numero * 10 + (input[j] - '0');
      }
    }
    if(numero >= 150) velocidad = numero;
    else              velocidad = 150;
  }
  
  /* CAMBIO DE LAS GANANCIAS DE CONTROL: Sintaxis: "[G/g][P/p/I/i/D/d][D/d/A/a][valor]." */
  if(input[0] == 'g' || input[0] == 'G'){
    if(input[0] == 'p' || input[0] == 'P') {
      if(input[0] == 'd' || input[0] == 'D'){
        for(int j = 0; j < 8; j++) {
          if(input[j] == '.') {
            j = 8;
          }
          else if(47 < input[j] && input[j] < 58) {
            numero = numero * 10 + (input[j] - '0');
          }
        }
        switch(Modo){
          case 0: break;
          case 1:
            Kp_dist_1 = numero;
            break;
          case 2:
            Kp_dist_2 = numero;
            break;
          case 3:
            Kp_dist_3 = numero;
            break;
          case 4:
            Kp_dist_4 = numero;
            break;
          default: break;
        }
      }
      else if(input[0] == 'a' || input[0] == 'A'){
        for(int j = 0; j < 8; j++) {
          if(input[j] == '.') {
            j = 8;
          }
          else if(47 < input[j] && input[j] < 58) {
            numero = numero * 10 + (input[j] - '0');
          }
        }
        switch(Modo){
          case 0: break;
          case 1: break;
          case 2:
            Kp_dif_2 = numero;
            break;
          case 3:
            Kp_dif_3 = numero;
            break;
          case 4:
            Kp_dif_4 = numero;
            break;
          default: break;
        }
      }
    }
    if(input[0] == 'i' || input[0] == 'I') {
      if(input[0] == 'd' || input[0] == 'D'){
        for(int j = 0; j < 8; j++) {
          if(input[j] == '.') {
            j = 8;
          }
          else if(47 < input[j] && input[j] < 58) {
            numero = numero * 10 + (input[j] - '0');
          }
        }
        switch(Modo){
          case 0: break;
          case 1:
            Ki_dist_1 = numero;
            break;
          case 2:
            Ki_dist_2 = numero;
            break;
          case 3:
            Ki_dist_3 = numero;
            break;
          case 4:
            Ki_dist_4 = numero;
            break;
          default: break;
        }
      }
      else if(input[0] == 'a' || input[0] == 'A'){
        for(int j = 0; j < 8; j++) {
          if(input[j] == '.') {
            j = 8;
          }
          else if(47 < input[j] && input[j] < 58) {
            numero = numero * 10 + (input[j] - '0');
          }
        }
        switch(Modo){
          case 0: break;
          case 1: break;
          case 2:
            Ki_dif_2 = numero;
            break;
          case 3:
            Ki_dif_3 = numero;
            break;
          case 4:
            Ki_dif_4 = numero;
            break;
          default: break;
        }
      }
    }
    if(input[0] == 'd' || input[0] == 'D') {
      if(input[0] == 'd' || input[0] == 'D'){
        for(int j = 0; j < 8; j++) {
          if(input[j] == '.') {
            j = 8;
          }
          else if(47 < input[j] && input[j] < 58) {
            numero = numero * 10 + (input[j] - '0');
          }
        }
        switch(Modo){
          case 0: break;
          case 1:
            Kd_dist_1 = numero;
            break;
          case 2:
            Kd_dist_2 = numero;
            break;
          case 3:
            Kd_dist_3 = numero;
            break;
          case 4:
            Kd_dist_4 = numero;
            break;
          default: break;
        }
      }
      else if(input[0] == 'a' || input[0] == 'A'){
        for(int j = 0; j < 8; j++) {
          if(input[j] == '.') {
            j = 8;
          }
          else if(47 < input[j] && input[j] < 58) {
            numero = numero * 10 + (input[j] - '0');
          }
        }
        switch(Modo){
          case 0: break;
          case 1: break;
          case 2:
            Kd_dif_2 = numero;
            break;
          case 3:
            Kd_dif_3 = numero;
            break;
          case 4:
            Kd_dif_4 = numero;
            break;
          default: break;
        }
      }
    }
  }

  // Ponemos el vector completo a '.' para evitar errores
  for(int j = 0; j < 8; j++)
    input[j] = '.';
}

void loop() {
  // Medimos el tiempo transcurrido
  timePrev = tiempo;
  tiempo = millis();
  elapsedTime = (tiempo - timePrev) / 1000;

  // Comprobamos si hay comandos para leer
  if (Serial1.available()) {
    input[i] = Serial1.read();
    // Todos los comandos deben acabar con un punto
    if(input[i] == '.') {
      // Cuando el punto indique el fin del comando pasamos a traducirlo
      leer_comando();
      i = 0;
    }
    else i ++;    
  }

  /* LECTURA DE LOS ULTRASONIDOS */
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

  // Conversion de las lecturas a distancias en cm
  DD = duracionD * 10 / 292 / 2;
  DI = duracionI * 10 / 292 / 2;

  /* PROGRAMACION DE LOS DISTINTOS MODOS DE FUNCIONAMIENTO */
  switch(Modo){
    case 0:
      // Todo parado
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
      // Calculo de las señales de control
      D = (DD + DI)/2;
      prev_error_dist = error_dist;
      error_dist = D - Ref_dist;
      int_error_dist += elapsedTime * error_dist;
      u_dist = Kp_dist_1 * error_dist + Ki_dist_1 * int_error_dist + Kd_dist_1 * (error_dist - prev_error_dist) / elapsedTime;
    
      u_D = u_dist;
      u_I = u_dist;

      // Configuracion de lo pines del servomotor derecho
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

      // Configuracion de lo pines del servomotor izquierdo
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
    
      // Anti-windup básico
      if (u_dist < -255 || 255 < u_dist)  int_error_dist -= elapsedTime * error_dist;
    
      // Saturaciones
      if (u_D > 255)  u_D = 255;
      else if (u_D < - 255) u_D = - 255;
      if (u_I > 255)  u_I = 255;
      else if (u_I < - 255) u_I = - 255;

      // Aplicacion de las señales de control a cada servomotor
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
      
    case 2:
      // Calculo de las señales de control en distancia
      D = (DD + DI)/2;
      prev_error_dist = error_dist;
      error_dist = D - Ref_dist;
      int_error_dist += elapsedTime * error_dist;
      u_dist = Kp_dist_2 * error_dist + Ki_dist_2 * int_error_dist + Kd_dist_2 * (error_dist - prev_error_dist) / elapsedTime;

      // Calculo de las señales de control de inclinación
      D_dif = DD - DI;
      prev_error_dif = error_dif;
      error_dif = Ref_dif - D_dif;
      int_error_dif += elapsedTime * error_dif;
      u_dif = Kp_dif_2 * error_dif + Ki_dif_2 * int_error_dif + Kd_dif_2 * (error_dif - prev_error_dif) / elapsedTime;
    
      u_D = (u_dist - u_dif/2);
      u_I = (u_dist + u_dif/2);

      // Configuracion de lo pines del servomotor derecho
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

      // Configuracion de lo pines del servomotor izquierdo
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
    
      // Anti-windup básico
      if (u_dist < -255 || 255 < u_dist)  int_error_dist -= elapsedTime * error_dist;
      if (u_dif < -255 || 255 < u_dif)  int_error_dif -= elapsedTime * error_dif;
    
      // Saturaciones
      if (u_D > 255)  u_D = 255;
      else if (u_D < - 255) u_D = - 255;
      if (u_I > 255)  u_I = 255;
      else if (u_I < - 255) u_I = - 255;

      // Aplicacion de las señales de control a cada servomotor
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
    case 3:
      // Calculo de las señales de control en distancia
      Ref_dist = 50;

      if(abs(DD_anterior - DD) > 10)  DD = DD_anterior;
      D = DD;
      prev_error_dist = error_dist;
      error_dist = D - Ref_dist;
      int_error_dist += elapsedTime * error_dist;
      u_dist = Kp_dist_2 * error_dist + Ki_dist_2 * int_error_dist + Kd_dist_2 * (error_dist - prev_error_dist) / elapsedTime;

      // Calculo de las señales de control de inclinación
      u_D = (velocidad + u_dist/2);
      u_I = (velocidad - u_dist/2);

      // Configuracion de lo pines del servomotor derecho
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

      // Configuracion de lo pines del servomotor izquierdo
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
    
      // Anti-windup básico
      if (u_dist < -255 || 255 < u_dist)  int_error_dist -= elapsedTime * error_dist;
      if (u_dif < -255 || 255 < u_dif)  int_error_dif -= elapsedTime * error_dif;
    
      // Saturaciones
      if (u_D > 255)  u_D = 255;
      else if (u_D < - 255) u_D = - 255;
      if (u_I > 255)  u_I = 255;
      else if (u_I < - 255) u_I = - 255;

      // Aplicacion de las señales de control a cada servomotor
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
      
    case 4:
      // Calculo de las señales de control en distancia

      if(abs(DD_anterior - DD) > 10)  DD = DD_anterior;
      D = DD;
      prev_error_dist = error_dist;
      error_dist = D - Ref_dist;
      int_error_dist += elapsedTime * error_dist;
      u_dist = Kp_dist_2 * error_dist + Ki_dist_2 * int_error_dist + Kd_dist_2 * (error_dist - prev_error_dist) / elapsedTime;

      // Calculo de las señales de control de inclinación
      u_D = (velocidad + u_dist/2);
      u_I = (velocidad - u_dist/2);

      // Configuracion de lo pines del servomotor derecho
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

      // Configuracion de lo pines del servomotor izquierdo
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
    
      // Anti-windup básico
      if (u_dist < -255 || 255 < u_dist)  int_error_dist -= elapsedTime * error_dist;
      if (u_dif < -255 || 255 < u_dif)  int_error_dif -= elapsedTime * error_dif;
    
      // Saturaciones
      if (u_D > 255)  u_D = 255;
      else if (u_D < - 255) u_D = - 255;
      if (u_I > 255)  u_I = 255;
      else if (u_I < - 255) u_I = - 255;

      // Aplicacion de las señales de control a cada servomotor
      analogWrite(END, abs(u_D));
      analogWrite(ENI, abs(u_I));
      break;
      
    default: 
      // Por seguridad, como accion por defecto paramos todo
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

  /* SALIDA DE DATOS */
  // 1. Tiempo transcurrido durante ell ultimo ciclo de control (ms)
  Serial1.print(elapsedTime * 1000);
  Serial1.print(" ");

  // 2. Distancia del ultrasonidos izquierdo a la pared (cm)
  Serial1.print(DI);
  Serial1.print(" ");

  // 3. Distancia del ultrasonidos derecho a la pared (cm)
  Serial1.print(DD);
  Serial1.print(" ");

  // 4. Referencia de distancia (cm)
  Serial1.print(Ref_dist);
  Serial1.print(" ");

  // 5. Modo de funcionamiento
  Serial1.print(Modo);
  Serial1.print(" ");

  // 6. Señal de control del servomotor izquierdo (PWM)
  Serial1.print(u_I);
  Serial1.print(" ");

  // 7. Señal de control del servomotor derecho (PWM)
  Serial1.print(u_D);
  Serial1.print(" ");

  // 8. Distancia a la pared (cm)
  Serial1.print(D);
  Serial1.print(" ");

  // 9. Distancia diferencial (cm)
  Serial1.print(D_dif);
  Serial1.print(" ");

  // 10. Referencia de distancia diferencial (cm)
  Serial1.println(Ref_dif);
}
