#include <Servo.h>

Servo myServo;

const int SERVO_PIN = 9;

// калибровка под твою серву
const int STOP_PWM  = 90;   // при этом стоит
const int FWD_PWM   = 150;  // максимальная вперёд
const int BACK_PWM  = 30;   // максимальная назад

// плавность
const int UPDATE_DT   = 20;   // каждые 20 мс обновляем PWM
const int ACCEL_STEP  = 1;    // так быстро разгоняемся
const int DECEL_STEP  = 5;    // так быстро тормозим

// через сколько мс без команд начинаем тормозить
const unsigned long NO_CMD_TIMEOUT = 120;  // 0.12 секунды

int currentPWM = STOP_PWM;
int targetPWM  = STOP_PWM;

unsigned long lastUpdate   = 0;
unsigned long lastCommand  = 0;

void setup() {
  myServo.attach(SERVO_PIN);
  myServo.write(STOP_PWM);
  Serial.begin(9600);
  Serial.println("Send F/B/S. Auto-stop after key released.");
}

void loop() {
  bool gotCmd = false;

  // 1. читаем, что пришло
  while (Serial.available() > 0) {
    char c = Serial.read();
    gotCmd = true;

    if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';

    if (c == 'F') {
      targetPWM = FWD_PWM;
      lastCommand = millis();
      Serial.println("FWD");
    } else if (c == 'B') {
      targetPWM = BACK_PWM;
      lastCommand = millis();
      Serial.println("BACK");
    } else if (c == 'S') {
      targetPWM = STOP_PWM;
      lastCommand = millis();
      Serial.println("STOP");
    } else if (c == '\r' || c == '\n') {
      // ignore
    } else {
      // unknown
    }
  }

  unsigned long now = millis();

  // 2. если давно ничего не приходило → начинаем тормозить
  if ((now - lastCommand) > NO_CMD_TIMEOUT) {
    targetPWM = STOP_PWM;
  }

  // 3. плавно тянем currentPWM к targetPWM
  if (now - lastUpdate >= UPDATE_DT) {
    lastUpdate = now;

    if (currentPWM != targetPWM) {
      // определим, разгон или торможение
      bool braking =
        (targetPWM == STOP_PWM) ||
        (currentPWM < STOP_PWM && targetPWM > STOP_PWM) ||
        (currentPWM > STOP_PWM && targetPWM < STOP_PWM);

      int step = braking ? DECEL_STEP : ACCEL_STEP;

      if (currentPWM < targetPWM) {
        currentPWM += step;
        if (currentPWM > targetPWM) currentPWM = targetPWM;
      } else {
        currentPWM -= step;
        if (currentPWM < targetPWM) currentPWM = targetPWM;
      }

      myServo.write(currentPWM);
    }
  }
}
