int resistance = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10);
  TCCR1B |= (0 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (0 << CS10);

  OCR1A = 512;  // 10-bit = 1023 (Non-inverted)
  OCR1B = 0;  // 10-bit = 1023 (Inverted)

  TIMSK1 |= (1 << OCIE1B) | (1 << OCIE1A); //| (1 << TOIE1);

  sei();
}

ISR(TIMER1_COMPA_vect) {
  if (OCR1B != 0) {     // need to work on this if statement
    PORTB |= (1 << PORTB3);
  }
}

ISR(TIMER1_COMPB_vect) {
  resistance = analogRead(A0);
  PORTB &= ~(1 << PORTB3);
}

//ISR(TIMER1_OVF_vect) {
//  digitalWrite(11, LOW);
//}

void setDuty(float dutyCycle) {
  OCR1A = dutyCycle/100*1023;

  TIMSK1 &= ~((1 << OCIE1B));
  
  if (dutyCycle <= 50.0) {
    OCR1B = OCR1A + 7;
    TIMSK1 |= (1 << OCIE1B);
  } else {              // need to work on this else statement
    OCR1B = 0;
    PORTB &= ~(1 << PORTB3);
  }
}

void loop() {
//  // put your main code here, to run repeatedly:
//  for(;;) {
//    if (OCR1A == 1023) {
//      OCR1A = 0;
//    } else {
//      OCR1A += 10;
//    }
//
//    delay(10);
//  }

  if (Serial.available() > 0) {
    float input = Serial.readString().toFloat();
    Serial.println(input);
    setDuty(input);
  }

}
