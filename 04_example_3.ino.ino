#define PIN_LED 13

unsigned int count, toggle;

// 함수 선언 (loop보다 아래에 위치하므로 원형 선언 필요)
int toggle_state(int toggle);

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200); // Initialize serial port
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  Serial.println("Hello World!");
  count = toggle = 0;
  digitalWrite(PIN_LED, toggle); // turn off LED.
}

void loop() {
  Serial.println(++count);
  toggle = toggle_state(toggle); // toggle LED value.
  digitalWrite(PIN_LED, toggle); // update LED status.
  delay(1000); // wait for 1,000 milliseconds
}

// toggle 값을 반전(0 -> 1, 1 -> 0)시켜 반환하는 함수
int toggle_state(int toggle) {
  return !toggle;
}
