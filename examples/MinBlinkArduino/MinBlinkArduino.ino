// compare sizes - MinBlinkArduino vs MinBlinkChNil

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}
bool level = true;
void loop() {
  digitalWrite(LED_BUILTIN, level = !level);
  delay(500);
}
