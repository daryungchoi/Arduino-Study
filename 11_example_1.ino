#include <Servo.h>

// 핀 설정
const int PIN_LED = 9;      // LED 핀[cite: 1]
const int PIN_SERVO = 10;   // 서보모터 핀
const int PIN_TRIG = 12;    // 초음파 센서 Trig 핀
const int PIN_ECHO = 13;    // 초음파 센서 Echo 핀

// 서보모터 펄스폭 설정 (요청값 반영)
const int SERVO_MIN_PULSE = 550;
const int SERVO_MAX_PULSE = 2330;
// neu 1440은 550과 2330의 중간값이므로 90도일 때 자동으로 적용됩니다.

// 거리 기준 (단위: cm를 mm로 변환하여 사용)
const float _DIST_MIN = 180.0; // 18cm[cite: 1, 2]
const float _DIST_MAX = 360.0; // 36cm[cite: 1, 2]

Servo myservo;

// 변수 설정
float dist_raw = 0.0;
float dist_ema = 0.0;
float alpha = 0.2; // EMA 필터용 alpha 값 (필요에 따라 0.0 ~ 1.0 사이로 조절)[cite: 1]

void setup() {
  Serial.begin(115200); // 시리얼 모니터 통신 속도
  
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  // 서보모터 초기화 및 펄스폭 설정
  myservo.attach(PIN_SERVO, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  
  // 초기 거리값 설정
  dist_ema = 0.0;
}

void loop() {
  // 1. 초음파 센서를 통한 거리 측정 (단위: mm)
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  long duration = pulseIn(PIN_ECHO, HIGH);
  float dist_measured = (duration * 0.343) / 2.0; // mm 단위 거리 계산

  // 2. 범위 필터 적용[cite: 1]
  // 측정 실패 등으로 급격하게 이동하는 현상 방지를 위해 측정 범위를 180~360mm로 제한
  if (dist_measured < _DIST_MIN) {
    dist_raw = _DIST_MIN;
  } else if (dist_measured > _DIST_MAX) {
    dist_raw = _DIST_MAX;
  } else {
    dist_raw = dist_measured;
  }
  
  // 측정 범위(18cm ~ 36cm) 안으로 들어올 경우 LED 켬, 아니면 끔[cite: 1]
  if (dist_measured >= _DIST_MIN && dist_measured <= _DIST_MAX) {
    digitalWrite(PIN_LED, HIGH); // LED 켬[cite: 1]
  } else {
    digitalWrite(PIN_LED, LOW);  // LED 끔
  }

  // 3. EMA 필터 적용[cite: 1]
  if (dist_ema == 0.0) {
    dist_ema = dist_raw; // 초기값
  } else {
    dist_ema = (alpha * dist_raw) + ((1 - alpha) * dist_ema); //[cite: 1]
  }

  // 4. 거리에 따른 서보 각도 제어 (비례식 적용)[cite: 1]
  // 거리 18cm(180mm) -> 0도, 36cm(360mm) -> 180도[cite: 1]
  int servo_angle = map(dist_ema, _DIST_MIN, _DIST_MAX, 0, 180);
  
  // 계산된 각도로 서보모터 구동
  myservo.write(servo_angle);

  // 5. 시리얼 플로터를 위한 출력 형식 (제시된 양식과 동일하게 맞춤)[cite: 2]
  Serial.print("Min:");      Serial.print(_DIST_MIN); //[cite: 2]
  Serial.print(",dist:");    Serial.print(dist_raw); //[cite: 2]
  Serial.print(",ema:");     Serial.print(dist_ema); //[cite: 2]
  Serial.print(",Servo:");   Serial.print(myservo.read()); //[cite: 2]
  Serial.print(",Max:");     Serial.print(_DIST_MAX); //[cite: 2]
  Serial.println(""); //[cite: 2]

  delay(50); // 센서 안정화 및 샘플링 주기 대기
}
