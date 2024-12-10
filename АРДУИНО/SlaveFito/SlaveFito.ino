byte ledPin[4] = {2, 3, 4, 5};

void setup() {
  // put your setup code here, to run once:
  for (byte i = 0; i < 4; i++)  pinMode(ledPin[i], OUTPUT);

}

void loop() {
  delay(3*1000);
  if   (analogRead(A1) > 500) {
    digitalWrite(ledPin[0], HIGH);
  }
  else {
    digitalWrite(ledPin[0], LOW);
  }
  if (analogRead(A2) > 500)  {
    digitalWrite(ledPin[3], HIGH);
  } else {
    digitalWrite(ledPin[3], LOW);
  }
  if (analogRead(A3) > 500)  {
    digitalWrite(ledPin[1], HIGH);
  } else {
    digitalWrite(ledPin[1], LOW);
  }
  if (analogRead(A4) > 500) {
    digitalWrite(ledPin[2], HIGH);
  } else {
    digitalWrite(ledPin[2], LOW);
  }
}
