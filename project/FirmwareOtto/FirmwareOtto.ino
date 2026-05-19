#include <Arduino.h>
#include <ESP32Servo.h>

//#define REMOTEXY__DEBUGLOG    

#define REMOTEXY_MODE__ESP32CORE_BLE
#define REMOTEXY_BLUETOOTH_NAME "Tigre RemoteLE"
#define REMOTEXY_ACCESS_PASSWORD "lamano"

#include <BLEDevice.h>
#include <RemoteXY.h>

// ===== NOTES =====
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_OFF 0

#define BUZZER_PIN D0  

Servo legLeft;
Servo legRight;
Servo footLeft;
Servo footRight;

const int trig_pin = D2;
const int echo_pin = D1;
const int pinLegLeft = D7;
const int pinLegRight = D8;
const int pinFootLeft = D9;
const int pinFootRight = D10;

bool debug = false;

// ===== MELODIE STAR WARS =====
int melody[] = {
  NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_D5,
  NOTE_G5,
  NOTE_F5, NOTE_E5, NOTE_D5,
  NOTE_G5,
  NOTE_F5, NOTE_E5, NOTE_D5,
  NOTE_G5,
  NOTE_F5, NOTE_E5, NOTE_F5, NOTE_D5
};

int durations[] = {
  300, 300, 300,
  250,
  600,
  300, 300, 300,
  600,
  300, 300, 300,
  600,
  300, 300, 300, 600
};

int melodyLength = 17;

// ===== MUSIQUE =====
void playStarWars() {
  for (int i = 0; i < melodyLength; i++) {
    if (melody[i] == NOTE_OFF) {
      noTone(BUZZER_PIN);
    } else {
      tone(BUZZER_PIN, melody[i], durations[i]);
    }
    delay(durations[i] + 50);
  }
  noTone(BUZZER_PIN);
}

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] = { 
  255,1,0,0,0,26,0,19,0,0,0,84,105,103,114,101,0,31,1,106,
  200,1,1,1,0,4,44,125,14,36,32,2,26 
};

struct {
  int8_t slider_servo_D7;
  uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)

// ===== DISTANCE =====
void distance(){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  long duration = pulseIn(echo_pin, HIGH);
  float distance = (duration * 0.0343) / 2;

  if (debug){
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
}

// ===== MARCHE =====
void walkForwardFast() {

  int tempo = 350;
  int lift = 10;
  int stride = 30;
  int drift = 10;

 ( footLeft.write(90 - lift);
  footRight.write(90 - lift);

  legRight.write(90 - stride - drift);
  legLeft.write(90 - stride + drift);

  footLeft.write(90 + lift);
  footRight.write(90 + lift);

  legLeft.write(90 + stride - drift);
  legRight.write(90 + stride + drift);) probleme ici à régler
}

// ===== SETUP =====
void setup() {
  // playStarWars();  // musique au démarrage

  // RemoteXY_Init();

  Serial.begin(9600);
  Serial.println("Démarrage");

  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  legLeft.attach(pinLegLeft);
  legRight.attach(pinLegRight);
  footLeft.attach(pinFootLeft);
  footRight.attach(pinFootRight);

  legLeft.write(90);
  legRight.write(90);
  footLeft.write(90);
  footRight.write(90);

  // RemoteXY_delay(2000);
}

// ===== LOOP =====
void loop() {
  // RemoteXY_Handler();
  walkForwardFast();
}
