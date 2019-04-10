#define TOGGLE_LED_STATE 't'

int incoming = 0;
byte in[10];

void setup() {
  Serial.begin(57600);
  Serial.println("INITALIZING");
  Serial.println("READY");
  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
  if (Serial.available () > 0 ) {
    incoming = Serial.read();
    Serial.print("incoming: ");
    Serial.println(in);
    if (incoming == TOGGLE_LED_STATE) {
      digitalWrite(BUILTIN_LED, HIGH);
      delay(5000);
      digitalWrite(BUILTIN_LED, LOW);
      delay(5);
      Serial.println("LED 13 blink");
    }
  }
}

