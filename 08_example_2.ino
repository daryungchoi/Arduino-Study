// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

float USS_measure(int TRIG, int ECHO);

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // Active Low: 255 값으로 시작하여 LED OFF 상태 초기화
  analogWrite(PIN_LED, 255);
  
  // initialize serial port
  Serial.begin(57600);
}

void loop() { 
  float distance;
  int pwm_value;

  // wait until next sampling time. // polling
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  // 거리에 따른 Active Low PWM 값 계산
  if (distance <= _DIST_MIN || distance >= _DIST_MAX) {
    // 100mm 이하이거나 300mm 이상인 경우 (범위 밖) -> LED OFF
    pwm_value = 255;
  } 
  else if (distance <= 200.0) {
    // 100mm ~ 200mm 구간: 100mm일 때 255(OFF) -> 200mm일 때 0(최대 밝기)
    pwm_value = (int)(255.0 - ((distance - _DIST_MIN) / 100.0) * 255.0);
  } 
  else {
    // 200mm ~ 300mm 구간: 200mm일 때 0(최대 밝기) -> 300mm일 때 255(OFF)
    pwm_value = (int)(((distance - 200.0) / 100.0) * 255.0);
  }

  // 안전을 위한 범위 제한 (0~255)
  pwm_value = constrain(pwm_value, 0, 255);

  // Active Low 적용 (analogWrite)
  analogWrite(PIN_LED, pwm_value);

  // 시리얼 플로터 / 시리얼 모니터 출력
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",PWM:");       Serial.print(pwm_value);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.println("");
  
  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
