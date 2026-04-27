

#include <Servo.h>
#include <Adafruit_LiquidCrystal.h>

// ── Pines ──────────────────────────────────────
#define TRIG_A      2
#define ECHO_A      3
#define TRIG_B      4
#define ECHO_B      5
#define LED_VERDE_A 6
#define LED_ROJO_A  7
#define BUZZER      8
#define PIN_SERVO   9
#define LED_VERDE_B 10
#define LED_ROJO_B  11

// LCD sin I2C: RS, EN, D4, D5, D6, D7
Adafruit_LiquidCrystal lcd(0);
Servo cambiavia;

// ── Constantes ─────────────────────────────────
#define DIST_TREN      30
#define DIST_INTRUSION 10
#define T_DESPEJE      3000

// ── Estado ─────────────────────────────────────
enum Estado { IDLE, VIA_A_ACTIVA, VIA_B_ACTIVA };
Estado estadoActual = IDLE;
unsigned long tiempoInicio  = 0;

// ── Timestamps de detección ────────────────────
unsigned long llegadaA = 0;  // momento en que A cruzó los 30cm
unsigned long llegadaB = 0;  // momento en que B cruzó los 30cm
bool trenAPresente = false;
bool trenBPresente = false;

// ───────────────────────────────────────────────
long medirDistancia(int trig, int echo) {
  digitalWrite(trig, LOW);  delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long dur = pulseIn(echo, HIGH, 30000);
  if (dur == 0) return 999;
  return dur * 0.034 / 2;
}

void semaforo(int pinR, int pinG, bool verde) {
  digitalWrite(pinR, verde ? LOW  : HIGH);
  digitalWrite(pinG, verde ? HIGH : LOW);
}

void activarViaA() {
  estadoActual = VIA_A_ACTIVA;
  tiempoInicio = millis();
  cambiavia.write(30);
  semaforo(LED_ROJO_A, LED_VERDE_A, true);
  semaforo(LED_ROJO_B, LED_VERDE_B, false);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Via A: LIBRE");
  lcd.setCursor(0, 1);
  lcd.print("Via B: BLOQUEADA");
  Serial.println(">> VIA A ACTIVA (llego primero)");
}

void activarViaB() {
  estadoActual = VIA_B_ACTIVA;
  tiempoInicio = millis();
  cambiavia.write(150);
  semaforo(LED_ROJO_A, LED_VERDE_A, false);
  semaforo(LED_ROJO_B, LED_VERDE_B, true);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Via B: LIBRE");
  lcd.setCursor(0, 1);
  lcd.print("Via A: BLOQUEADA");
  Serial.println(">> VIA B ACTIVA (llego primero)");
}

void resetIDLE() {
  estadoActual = IDLE;
  llegadaA = 0;
  llegadaB = 0;
  trenAPresente = false;
  trenBPresente = false;
  cambiavia.write(90);
  semaforo(LED_ROJO_A, LED_VERDE_A, false);
  semaforo(LED_ROJO_B, LED_VERDE_B, false);
  noTone(BUZZER);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema IDLE");
  lcd.setCursor(0, 1);
  lcd.print("Ambas: STOP");
  Serial.println(">> IDLE");
}

// ───────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  pinMode(TRIG_A,      OUTPUT);
  pinMode(ECHO_A,      INPUT);
  pinMode(TRIG_B,      OUTPUT);
  pinMode(ECHO_B,      INPUT);
  pinMode(LED_VERDE_A, OUTPUT);
  pinMode(LED_ROJO_A,  OUTPUT);
  pinMode(LED_VERDE_B, OUTPUT);
  pinMode(LED_ROJO_B,  OUTPUT);
  pinMode(BUZZER,      OUTPUT);

  cambiavia.attach(PIN_SERVO);
  cambiavia.write(90);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Sistema IDLE");
  lcd.setCursor(0, 1);
  lcd.print("Ambas: STOP");

  semaforo(LED_ROJO_A, LED_VERDE_A, false);
  semaforo(LED_ROJO_B, LED_VERDE_B, false);

  // Test visual de arranque
  Serial.println("Iniciando test de pines...");
  digitalWrite(LED_VERDE_A, HIGH); delay(400);
  digitalWrite(LED_VERDE_A, LOW);
  digitalWrite(LED_ROJO_A,  HIGH); delay(400);
  digitalWrite(LED_ROJO_A,  LOW);
  digitalWrite(LED_VERDE_B, HIGH); delay(400);
  digitalWrite(LED_VERDE_B, LOW);
  digitalWrite(LED_ROJO_B,  HIGH); delay(400);
  digitalWrite(LED_ROJO_B,  LOW);
  tone(BUZZER, 1000, 300);
  Serial.println("Test completo. Sistema listo.");
}

// ───────────────────────────────────────────────
void loop() {
  long distA = medirDistancia(TRIG_A, ECHO_A);
  long distB = medirDistancia(TRIG_B, ECHO_B);
  unsigned long ahora = millis();

  // ── Registrar timestamp de primera detección ──
  if (distA < DIST_TREN && !trenAPresente) {
    trenAPresente = true;
    llegadaA = ahora;
    Serial.print("Tren A detectado en t=");
    Serial.println(llegadaA);
  }
  if (distA >= DIST_TREN) trenAPresente = false;

  if (distB < DIST_TREN && !trenBPresente) {
    trenBPresente = true;
    llegadaB = ahora;
    Serial.print("Tren B detectado en t=");
    Serial.println(llegadaB);
  }
  if (distB >= DIST_TREN) trenBPresente = false;

  Serial.print("A="); Serial.print(distA);
  Serial.print("cm | B="); Serial.print(distB);
  Serial.print("cm | Estado="); Serial.println(estadoActual);

  switch (estadoActual) {

    case IDLE:
      // Solo actúa si al menos uno detectó un tren
      if (trenAPresente || trenBPresente) {

        // Ambos presentes: gana el que llegó primero
        if (trenAPresente && trenBPresente) {
          Serial.print("Conflicto! A llego en: "); Serial.print(llegadaA);
          Serial.print(" | B llego en: ");         Serial.println(llegadaB);
          if (llegadaA <= llegadaB) activarViaA();
          else                      activarViaB();
        }
        // Solo A presente
        else if (trenAPresente) activarViaA();
        // Solo B presente
        else                    activarViaB();
      }
      break;

    case VIA_A_ACTIVA:
      if (distB < DIST_INTRUSION) {
        tone(BUZZER, 1000);
        lcd.setCursor(0, 1);
        lcd.print("!INTRUSION B    ");
        Serial.println("!! INTRUSION B");
      } else if (distB < DIST_TREN) {
        noTone(BUZZER);
        lcd.setCursor(0, 1);
        lcd.print("Tren B espera   ");
      } else {
        noTone(BUZZER);
      }
      if (millis() - tiempoInicio > T_DESPEJE) resetIDLE();
      break;

    case VIA_B_ACTIVA:
      if (distA < DIST_INTRUSION) {
        tone(BUZZER, 1000);
        lcd.setCursor(0, 1);
        lcd.print("!INTRUSION A    ");
        Serial.println("!! INTRUSION A");
      } else if (distA < DIST_TREN) {
        noTone(BUZZER);
        lcd.setCursor(0, 1);
        lcd.print("Tren A espera   ");
      } else {
        noTone(BUZZER);
      }
      if (millis() - tiempoInicio > T_DESPEJE) resetIDLE();
      break;
  }

  delay(200);
}


