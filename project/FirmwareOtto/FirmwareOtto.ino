#define REMOTEXY_MODE__ESP32CORE_BLE

#include <BLEDevice.h>

// RemoteXY connection settings
#define REMOTEXY_BLUETOOTH_NAME "Otto Groupe 11"
#define REMOTEXY_ACCESS_PASSWORD "sixseven"


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 43 bytes V19
  { 255,2,0,0,0,36,0,19,0,0,0,79,116,116,111,32,71,114,111,117,
  112,101,32,49,49,0,31,1,106,200,1,1,1,0,5,26,63,60,60,0,
  2,26,31 };
 
// this structure defines all the variables and events of your control interface
struct {

    // input variables
  int8_t joystick_x; // from -100 to 100
  int8_t joystick_y; // from -100 to 100

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;  
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


#include <ESP32Servo.h>

#define BUZZER_PIN D0

const int trig_pin = D2;
const int echo_pin = D1;

Servo piedDroit;
Servo piedGauche;
Servo jambeDroite;
Servo jambeGauche;

int offsetJD = -4;
int offsetJG = +5 ;
int offsetPD = +8;
int offsetPG = +0;

int correction = 5;

#define NOTE_SOL4 392
#define NOTE_LA4  440
#define NOTE_SI4  494
#define NOTE_DO5  523

int melody[] = {NOTE_SOL4, NOTE_LA4, NOTE_SI4, NOTE_DO5};
int durations[] = {200, 200, 200, 400};
int noire;


void setup() {

  RemoteXY_Init ();  // initialization by macros

  piedDroit.attach(D9);
  piedGauche.attach(D8);
  jambeDroite.attach(D10);
  jambeGauche.attach(D7);

  pinMode(trig_pin, OUTPUT);

  pinMode(echo_pin, INPUT);

  positionRepos();
  delay(1000);
  int tempo = 120;
  noire = 60000 / tempo;

 
  playMelody();
}

void loop() {
  RemoteXY_Handler();  

  int x = RemoteXY.joystick_x;
  int y = RemoteXY.joystick_y;
 
  // zone morte joystick
  if (abs(x) < 20 && abs(y) < 20) {
    positionRepos();
    }
   
    // avancer
    else if (y > 40) {
      marcheAvant();
      }

      // reculer
      else if (y < -40) {
        marcheArriere();
        }
       
        // tourner gauche
        else if (x < -40) {
          tourneGauche();
          }
         
          // tourner droite
          else if (x > 40) {
            tourneDroite();
          }
        }

void positionRepos() {
  piedDroit.write(90);
  piedGauche.write(90);
  jambeDroite.write(90 + offsetJD);
  jambeGauche.write(90 + offsetJG);
}

void marche() {
  int amplitude = 25;
  int vitesse = 10;

  for (int angle = 0; angle < 360; angle += 5) {

    float rad = angle * PI / 180.0;

    int jd = 90 + offsetJD - amplitude * sin(rad);
    int jg = 90 + offsetJG + amplitude * sin(rad);

    int pd = 90 + offsetPD + 15 * sin(rad + PI/2);
    int pg = 90 + offsetPG - 15 * sin(rad + PI/2);

    jambeDroite.write(jd);
    jambeGauche.write(jg);
    piedDroit.write(pd);
    piedGauche.write(pg);

    delay(vitesse);
  }
}

void marcheAvant() {

  static int angle = 0;

  int amplitude = 20;

  float rad = angle * PI / 180.0;

  int jd = 90 + offsetJD - amplitude * sin(rad) - correction;
  int jg = 90 + offsetJG + amplitude * sin(rad);

  int pd = 90 + offsetPD + 12 * sin(rad + PI/2);
  int pg = 90 + offsetPG - 12 * sin(rad + PI/2);

  jambeDroite.write(jd);
  jambeGauche.write(jg);

  piedDroit.write(pd);
  piedGauche.write(pg);

  angle += 10;

  if (angle >= 360) {
    angle = 0;
  }

  delay(15);
}

void marcheArriere() {

  static int angle = 360;

  int amplitude = 20;

  float rad = angle * PI / 180.0;

  int jd = 90 + offsetJD - amplitude * sin(rad);
  int jg = 90 + offsetJG + amplitude * sin(rad);

  int pd = 90 + offsetPD + 12 * sin(rad + PI/2);
  int pg = 90 + offsetPG - 12 * sin(rad + PI/2);

  jambeDroite.write(jd);
  jambeGauche.write(jg);

  piedDroit.write(pd);
  piedGauche.write(pg);

  angle -= 10;

  if (angle <= 0) {
    angle = 360;
  }

  delay(15);
}

void tourneGauche() {

  static int angle = 0;
  angle += 10;

  float rad = angle * PI / 180.0;

  jambeDroite.write(90 + offsetJD - 15 * sin(rad));
  jambeGauche.write(90 + offsetJG - 15 * sin(rad));

  piedDroit.write(90 + offsetPD - 10);
  piedGauche.write(90 + offsetPG + 10);

  delay(15);
}

void tourneDroite() {

  static int angle = 0;
  angle += 10;

  float rad = angle * PI / 180.0;

  jambeDroite.write(90 + offsetJD + 15 * sin(rad));
  jambeGauche.write(90 + offsetJG + 15 * sin(rad));

  piedDroit.write(90 + offsetPD + 10);
  piedGauche.write(90 + offsetPG - 10);

  delay(15);
}


float lireDistance() {

  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  long duration = pulseIn(echo_pin, HIGH, 30000);

  float distance = (duration * 0.0343) / 2;

  return (distance == 0) ? 100 : distance;
}

void playMelody() {
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, melody[i], durations[i]);
    delay(durations[i] * 1.3);
    noTone(BUZZER_PIN);
  }
}
