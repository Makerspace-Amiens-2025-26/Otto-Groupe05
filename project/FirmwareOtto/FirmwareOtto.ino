#include <Arduino.h>
#include <ESP32Servo.h>

// RemoteXY select connection mode and include library
#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>

// ==========================================
// ⚙️ PARAMÈTRES BLUETOOTH
// ==========================================
#define REMOTEXY_BLUETOOTH_NAME "Otto"
// (Le mot de passe a été retiré)

#include <RemoteXY.h>

// Définition des fréquences des notes de musiques
#include "notes.h"
#define DUREE_TEMPS 300  
#define BUZZER_PIN D0  

// ==========================================
// ⚙️ DÉCLARATION DES MOTEURS ET CAPTEURS
// ==========================================
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

// ==========================================
// 📱 CONFIGURATION REMOTEXY (À METTRE À JOUR)
// ==========================================
// Remplacer ce tableau par celui généré SANS le bouton sur le site RemoteXY
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =  
  { 255,4,0,0,0,56,0,19,0,0,0,84,105,103,114,79,116,116,111,0,
  79,1,106,200,1,1,3,0,2,37,167,32,18,0,2,26,31,31,79,78,
  0,79,70,70,0,5,42,70,60,60,32,2,26,31,1,7,87,24,24,0,
  2,31,0 };
 
struct {
  uint8_t switch_01;     // =1 if switch ON and =0 if OFF
  int8_t joystick_01_x;  // from -100 to 100
  int8_t joystick_01_y;  // from -100 to 100
  uint8_t connect_flag;  // =1 if wire connected, else =0
} RemoteXY;  
#pragma pack(pop)

// ==========================================
// ⚙️ PARAMÈTRES DE MARCHE EN 8 TEMPS
// ==========================================
int centreFootLeft = 90;  
int centreFootRight = 90;
int centreLegLeft = 90;  
int centreLegRight = 90;  

int tempo = 150;       // Vitesse d'exécution des pas (en ms)
int stride = 30;       // Amplitude des ciseaux      
int liftGauche = 30;   // Amplitude de levée gauche    
int liftDroite = 30;   // Amplitude de levée droite
int drift = 0;         // Correction de trajectoire


// ==========================================
// 🎵 MUSIQUE DE DÉMARRAGE
// ==========================================
void startupMusic() {
  tone(BUZZER_PIN, OCTAVE_4_DO, DUREE_TEMPS);
  delay(DUREE_TEMPS*1.3);
  tone(BUZZER_PIN, OCTAVE_4_DO, DUREE_TEMPS);
  delay(DUREE_TEMPS*1.3);
  tone(BUZZER_PIN , OCTAVE_4_DO, DUREE_TEMPS);
  delay(DUREE_TEMPS*1.3);
  tone(BUZZER_PIN, OCTAVE_4_RE, DUREE_TEMPS);
  delay(DUREE_TEMPS*1.3);
  tone(BUZZER_PIN, OCTAVE_4_MI, 2 * DUREE_TEMPS);
  delay(2.6 * DUREE_TEMPS);
  tone(BUZZER_PIN, OCTAVE_4_RE, 2 * DUREE_TEMPS);
  delay(2.6 * DUREE_TEMPS);
  tone(BUZZER_PIN, OCTAVE_4_DO, DUREE_TEMPS);
  delay(DUREE_TEMPS*1.3);
  tone(BUZZER_PIN, OCTAVE_4_MI, DUREE_TEMPS);
  delay(DUREE_TEMPS*1.3);
  tone(BUZZER_PIN, OCTAVE_4_RE, DUREE_TEMPS);
  delay(DUREE_TEMPS*1.3);
  tone(BUZZER_PIN, OCTAVE_4_RE, DUREE_TEMPS);
  delay(DUREE_TEMPS*1.3);
  tone(BUZZER_PIN, OCTAVE_4_DO, 4 * DUREE_TEMPS);
  delay(5.2 * DUREE_TEMPS);
  noTone(BUZZER_PIN);    
}

// ==========================================
// 🛑 ARRÊT
// ==========================================
void stopRobot() {
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  legLeft.write(centreLegLeft);
  legRight.write(centreLegRight);
}

// ==========================================
// ⬆️ MARCHE AVANT (8 TEMPS)
// ==========================================
void marcheAvant() {
  // Bascule 1
  footLeft.write(centreFootLeft - liftGauche);
  footRight.write(centreFootRight - liftDroite);
  RemoteXY_delay(tempo);
  // Ciseau 1
  legLeft.write(centreLegLeft - (stride / 2) + drift);
  legRight.write(centreLegRight - (stride / 2) - drift);
  RemoteXY_delay(tempo / 2);
  legLeft.write(centreLegLeft - stride + drift);
  legRight.write(centreLegRight - stride - drift);
  RemoteXY_delay(tempo / 2);
  // Retour centre
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  RemoteXY_delay(tempo / 2);
  // Bascule 2
  footLeft.write(centreFootLeft + liftGauche);
  footRight.write(centreFootRight + liftDroite);
  RemoteXY_delay(tempo);
  // Ciseau 2
  legLeft.write(centreLegLeft + (stride / 2) - drift);
  legRight.write(centreLegRight + (stride / 2) + drift);
  RemoteXY_delay(tempo / 2);
  legLeft.write(centreLegLeft + stride - drift);
  legRight.write(centreLegRight + stride + drift);
  RemoteXY_delay(tempo / 2);
  // Retour centre
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  RemoteXY_delay(tempo / 2);
}

// ==========================================
// ⬇️ MARCHE ARRIÈRE (8 TEMPS)
// ==========================================
void marcheArriere() {
  footLeft.write(centreFootLeft - liftGauche);
  footRight.write(centreFootRight - liftDroite);
  RemoteXY_delay(tempo);
 
  legLeft.write(centreLegLeft + (stride / 2) + drift);
  legRight.write(centreLegRight + (stride / 2) - drift);
  RemoteXY_delay(tempo / 2);
  legLeft.write(centreLegLeft + stride + drift);
  legRight.write(centreLegRight + stride - drift);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft + liftGauche);
  footRight.write(centreFootRight + liftDroite);
  RemoteXY_delay(tempo);
 
  legLeft.write(centreLegLeft - (stride / 2) - drift);
  legRight.write(centreLegRight - (stride / 2) + drift);
  RemoteXY_delay(tempo / 2);
  legLeft.write(centreLegLeft - stride - drift);
  legRight.write(centreLegRight - stride + drift);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  RemoteXY_delay(tempo / 2);
}

// ==========================================
// ⬅️ TOURNER GAUCHE (8 TEMPS)
// ==========================================
void tournerGauche() {
  footLeft.write(centreFootLeft - liftGauche);
  footRight.write(centreFootRight - liftDroite);
  RemoteXY_delay(tempo);
 
  legLeft.write(centreLegLeft + (stride / 2));
  legRight.write(centreLegRight - (stride / 2));
  RemoteXY_delay(tempo / 2);
  legLeft.write(centreLegLeft + stride);
  legRight.write(centreLegRight - stride);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft + liftGauche);
  footRight.write(centreFootRight + liftDroite);
  RemoteXY_delay(tempo);
 
  legLeft.write(centreLegLeft - (stride / 2));
  legRight.write(centreLegRight + (stride / 2));
  RemoteXY_delay(tempo / 2);
  legLeft.write(centreLegLeft - stride);
  legRight.write(centreLegRight + stride);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  RemoteXY_delay(tempo / 2);
}

// ==========================================
// ➡️ TOURNER DROITE (8 TEMPS)
// ==========================================
void tournerDroite() {
  footLeft.write(centreFootLeft - liftGauche);
  footRight.write(centreFootRight - liftDroite);
  RemoteXY_delay(tempo);
 
  legLeft.write(centreLegLeft - (stride / 2));
  legRight.write(centreLegRight + (stride / 2));
  RemoteXY_delay(tempo / 2);
  legLeft.write(centreLegLeft - stride);
  legRight.write(centreLegRight + stride);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft + liftGauche);
  footRight.write(centreFootRight + liftDroite);
  RemoteXY_delay(tempo);
 
  legLeft.write(centreLegLeft + (stride / 2));
  legRight.write(centreLegRight - (stride / 2));
  RemoteXY_delay(tempo / 2);
  legLeft.write(centreLegLeft + stride);
  legRight.write(centreLegRight - stride);
  RemoteXY_delay(tempo / 2);
 
  footLeft.write(centreFootLeft);
  footRight.write(centreFootRight);
  RemoteXY_delay(tempo / 2);
}


// ==========================================
// 🚀 INITIALISATION
// ==========================================
void setup() {
  // startupMusic();
  RemoteXY_Init();  

  Serial.begin(9600);  
  Serial.println("Démarrage de la communication série");

  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  legLeft.attach(pinLegLeft);
  legRight.attach(pinLegRight);
  footLeft.attach(pinFootLeft);
  footRight.attach(pinFootRight);

  stopRobot(); // Verrouille les moteurs sur leur centre dès l'allumage
 
  Serial.println("Calibrage des moteurs terminé");
  RemoteXY_delay(2000);
}

// ==========================================
// 🔄 BOUCLE PRINCIPALE
// ==========================================
void loop() {
  RemoteXY_Handler();
 
  if (RemoteXY.switch_01 == 0) {
    // Mode Auto : Avance en continu
    marcheAvant();
  } else {
    // Mode Manuel : Contrôle via le Joystick
    int axeX = RemoteXY.joystick_01_x;
    int axeY = RemoteXY.joystick_01_y;
    int seuil = 50;

    if (axeY > seuil && abs(axeX) < seuil) {
      marcheAvant();
    }
    else if (axeY < -seuil && abs(axeX) < seuil) {
      marcheArriere();
    }
    else if (axeX > seuil && abs(axeY) < seuil) {
      tournerDroite();
    }
    else if (axeX < -seuil && abs(axeY) < seuil) {
      tournerGauche();
    }
    else {
      stopRobot();
    }
  }
}
