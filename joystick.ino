#include <Servo.h>

/* ========= НАСТРОЙКИ ========= */

// Пины сервоприводов (сигналы)
const uint8_t SERVO_PINS[4] = {9, 10, 8, 11};

// Пины джойстиков
// Джойстик 1
const uint8_t J1_X = A0;
const uint8_t J1_Y = A1;
const uint8_t J1_SW = 4;     // кнопка (замыкает на GND)
// Джойстик 2
const uint8_t J2_X = A2;
const uint8_t J2_Y = A3;
const uint8_t J2_SW = 7;     // кнопка (замыкает на GND)

// Калибровка стопа и пределов для КАЖДОГО мотора (можно подстроить отдельно)
int STOP_PWM[4] = {90, 90, 90, 90};  // при этих значениях моторы стоят
int MAX_FWD[4]  = {110,110,110,110}; // максималка вперёд  (> STOP)
int MAX_BACK[4] = {70, 70, 70, 70};  // максималка назад   (< STOP)

// Мёртвая зона вокруг центра стика (0..1023). 40–80 обычно норм.
const int DEADZONE = 60;

// Инвертирование осей (если «вперёд» крутится «назад» — поставьте true)
bool INV_AXIS[4] = {false, false, false, false};
// порядок: [S1=J1_X, S2=J1_Y, S3=J2_X, S4=J2_Y]

/* ========= ВНУТРЕННЕЕ ========= */

Servo srv[4];

/* Преобразуем положение оси (0..1023) → PWM для сервопривода непрерывного вращения.
   512 — центр. В DEADZONE возвращаем стоп.
   За пределами — линейно до MAX_FWD/MAX_BACK. */
int axisToPwm(int raw, int idx, bool invert) {
  // центрируем
  int c = raw - 512;

  // инвертирование
  if (invert) c = -c;

  // мёртвая зона
  if (abs(c) <= DEADZONE) return STOP_PWM[idx];

  // "обрезаем" DEADZONE, нормируем к диапазону [0..1]
  float norm;
  if (c > 0) {
    norm = float(c - DEADZONE) / float(512 - DEADZONE);
    // вперёд: STOP -> MAX_FWD
    float pwm = STOP_PWM[idx] + norm * (MAX_FWD[idx] - STOP_PWM[idx]);
    if (pwm > MAX_FWD[idx]) pwm = MAX_FWD[idx];
    return int(pwm + 0.5f);
  } else {
    norm = float((-c) - DEADZONE) / float(512 - DEADZONE);
    // назад: STOP -> MAX_BACK
    float pwm = STOP_PWM[idx] - norm * (STOP_PWM[idx] - MAX_BACK[idx]);
    if (pwm < MAX_BACK[idx]) pwm = MAX_BACK[idx];
    return int(pwm + 0.5f);
  }


  
}

void writeStopAll() {
  for (int i = 0; i < 4; ++i) srv[i].write(STOP_PWM[i]);
}

void setup() {

  //Serial.begin(9600);
  //Serial.println("=== Joystick control debug start ===");
  
  // Сервы
  for (int i = 0; i < 4; ++i) {
    srv[i].attach(SERVO_PINS[i]);
  }
  writeStopAll();

  // Кнопки джойстиков — внутренние подтяжки (замыкают на GND при нажатии)
  pinMode(J1_SW, INPUT_PULLUP);
  pinMode(J2_SW, INPUT_PULLUP);

  // Аналоговые пины — просто читаем analogRead(...)
  // Можно добавить усреднение/фильтр при желании

  // Небольшая пауза на старт
  delay(300);
}

void loop() {
  // 1) Чтение осей
  int j1x = analogRead(J1_X);  // 0..1023
  int j1y = analogRead(J1_Y);
  int j2x = analogRead(J2_X);
  int j2y = analogRead(J2_Y);

  // 2) Кнопки: если нажали любой (LOW) — стоп всем
  bool stopPressed = (digitalRead(J1_SW) == LOW) || (digitalRead(J2_SW) == LOW);
  if (stopPressed) {
    writeStopAll();
  } else {
    // 3) Преобразуем оси -> PWM и пишем на сервы
    // Соответствие: S1=J1_X, S2=J1_Y, S3=J2_X, S4=J2_Y
    int pwm1 = axisToPwm(j1x, 0, INV_AXIS[0]);
    int pwm2 = axisToPwm(j1y, 1, INV_AXIS[1]);
    int pwm3 = axisToPwm(j2x, 2, INV_AXIS[2]);
    int pwm4 = axisToPwm(j2y, 3, INV_AXIS[3]);

      // Печать отладки
    //Serial.println(pwm1);
    //Serial.println(pwm2);
    //Serial.println(pwm3);
    //Serial.println(pwm4);

    srv[0].write(pwm1);
    srv[1].write(pwm2);
    srv[2].write(pwm3);
    srv[3].write(pwm4);
  }

  // период опроса 10–20 мс — норм
  delay(15);
}
