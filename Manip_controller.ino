#include <Servo.h>

Servo s[4];
const int PINS[4] = {9, 10, 8, 11};

// Подстрой под свою серву
const int STOP_PWM = 90;
const int FWD_PWM  = 110;
const int BACK_PWM = 70;

// Время ожидания без команд, после которого стопим всё (мс)
const unsigned long TIMEOUT_MS = 120;

unsigned long lastCmdTime = 0;
int currentCmd[4] = {0, 0, 0, 0};  // -1 = back, 0 = stop, +1 = forward

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    s[i].attach(PINS[i]);
    s[i].write(STOP_PWM);
  }
  lastCmdTime = millis();
}

void loop() {
  // --- читаем команды вида <id><cmd> ---
  while (Serial.available() >= 2) {
    char id = Serial.read();
    char cmd = Serial.read();
    if (cmd >= 'a' && cmd <= 'z') cmd -= 32; // верхний регистр

    if (id >= '1' && id <= '4') {
      int i = id - '1';
      if (cmd == 'F') { s[i].write(FWD_PWM);  currentCmd[i] = 1; }
      else if (cmd == 'B') { s[i].write(BACK_PWM); currentCmd[i] = -1; }
      else { s[i].write(STOP_PWM); currentCmd[i] = 0; }
    }
    else if (id == '0') {  // 0S = всем стоп
      for (int i=0;i<4;i++){ s[i].write(STOP_PWM); currentCmd[i]=0; }
    }

    lastCmdTime = millis(); // помечаем, что что-то пришло
  }

  // --- если нет команд слишком долго — стопим всё ---
  if (millis() - lastCmdTime > TIMEOUT_MS) {
    for (int i = 0; i < 4; i++) {
      if (currentCmd[i] != 0) {
        s[i].write(STOP_PWM);
        currentCmd[i] = 0;
      }
    }
  }
}
