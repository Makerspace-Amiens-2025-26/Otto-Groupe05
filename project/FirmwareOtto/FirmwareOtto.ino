#include <ESP32Servo.h>

//#define REMOTEXY__DEBUGLOG
#define REMOTEXY_MODE__ESP32CORE_BLE
#define REMOTEXY_BLUETOOTH_NAME "Tigre RemoteLE"
#define REMOTEXY_ACCESS_PASSWORD "lamano"

#include <BLEDevice.h>
#include <RemoteXY.h>

// ===== BUZZER (ESP32 compatible) =====
#define BUZZER_PIN 4
#define BUZZER_CHANNEL 0
#define BUZZER_RESOLUTION 8

#define DUREE_TEMPS 300

// ===== NOTES =====
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330

// ===== SERVOS =====
Servo legLeft;
Servo legRight;
Servo footLeft;
Servo footRight;

// ===== PINS ESP32 =====
const int trig_pin = 5;
const int echo_pin = 18;
const int pinLegLeft = 19;
const int pinLegRight = 21;
const int pinFootLeft = 22;
const int pinFootRight = 23;

// ===== REMOTEXY =====
#pragma pack(push, 1)
uint8_t const RemoteXY_CONF_PROGMEM[] = {
  255,1,0,0,0,26,0,19,0,0,0,84,105,103,114,101,0,31,1,106,
  200,1,1,1,0,4,44,125,14,36,32,2,26
};

struct {
  int8_t slider_servo_D7;
  uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)

// ===== BUZZER FUNCTIONS =====
void setupBuzzer() {
  ledcSetup(BUZZER_CHANNEL, 2000, BUZZER_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
}

void playTone(int freq, int duration) {
  if (freq == 0) {
    ledcWriteTone(BUZZER_CHANNEL, 0);
  } else {
    ledcWriteTone(BUZZER_CHANNEL, freq);
  }
  delay(duration);
}

// ===== STARTUP MUSIC =====
void startupMusic() {
  playTone(NOTE_C4, DUREE_TEMPS);
  delay(100);
  playTone(NOTE_C4, DUREE_TEMPS);
  delay(100);
  playTone(NOTE_C4, DUREE_TEMPS);
  delay(100);
  playTone(NOTE_D4, DUREE_TEMPS);
  delay(100);
  playTone(NOTE_E4, 2 * DUREE_TEMPS);
  delay(200);
}

// ===== TEST SERVOS =====
void testServos() {
  Serial.println("Test servos...");
  
  legLeft.write(0);
  legRight.write(0);
  footLeft.write(0);
  footRight.write(0);
  delay(1000);

  legLeft.write(180);
  legRight.write(180);
  footLeft.write(180);
  footRight.write(180);
  delay(1000);
}

// ===== DISTANCE =====
float getDistance() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);

  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  long duration = pulseIn(echo_pin, HIGH);
  return (duration * 0.0343) / 2;
}

// ===== WALK =====
void walkForwardFast() {
  int tempo = 120;
  int lift = 20;
  int stride = 25;
  int drift = 0;

  footLeft.write(90 - lift);
  footRight.write(90 - lift);
  RemoteXY_delay(tempo);

  legRight.write(90 + stride + drift);
  legLeft.write(90 + stride - drift);
  RemoteXY_delay(tempo);

  footLeft.write(90 + lift);
  footRight.write(90 + lift);
  RemoteXY_delay(tempo);

  legLeft.write(90 - stride - drift);
  legRight.write(90 - stride + drift);
  RemoteXY_delay(tempo);
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  setupBuzzer();
  startupMusic();

  RemoteXY_Init();

  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  legLeft.attach(pinLegLeft);
  legRight.attach(pinLegRight);
  footLeft.attach(pinFootLeft);
  footRight.attach(pinFootRight);

  testServos();

  legLeft.write(90);
  legRight.write(90);
  footLeft.write(90);
  footRight.write(90);

  Serial.println("Robot prêt !");
}

// ===== LOOP =====
void loop() {
  RemoteXY_Handler();

  float d = getDistance();
  Serial.print("Distance: ");
  Serial.println(d);

  if (d > 20) {
    walkForwardFast();
  } else {
    Serial.println("Obstacle !");
  }
}
