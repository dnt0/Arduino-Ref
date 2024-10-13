volatile int resistance = 0;
#define MAX_DUTY_CYCLE (98.0)   // max. duty cycle = 1 / ((1/60Hz)*10^6) * (105+100+105)[mosfet_activ+analogReadTime+mosfet_activ] = 98.17%
#define MIN_TICKS_DELAY 7       // min. ticks delay for mosfet_activ =  105 [microsec/mosfet_activation] / (1/60Hz*10^6)[microsec/cycle] * (1/1023)[cycle/timertick] = 6.44 ticks


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;

  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10);
  TCCR1B |= (0 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (0 << CS10);

  OCR1A = 512;  // 10-bit = 1023 (Non-inverted)
  OCR1B = OCR1A + MIN_TICKS_DELAY;  // 10-bit = 1023 (Inverted)

  TIMSK1 |= (1 << OCIE1B) | (1 << OCIE1A);

  sei();
}

ISR(TIMER1_COMPA_vect) {
  PORTB |= (1 << PORTB3);
}

ISR(TIMER1_COMPB_vect) {
  resistance = analogRead(A0);
  PORTB &= ~(1 << PORTB3);
}

void setDuty(float dutyCycle) {
  OCR1A = dutyCycle/100*1023;

  TIMSK1 &= ~((1 << OCIE1B) | (1 << OCIE1A));
  PORTB &= ~(1 << PORTB3);
  
  if (dutyCycle <= MAX_DUTY_CYCLE) {
    OCR1B = OCR1A + MIN_TICKS_DELAY;
    TIMSK1 |= (1 << OCIE1B) | (1 << OCIE1A);
  } 
}

void loop() {
  if (Serial.available() > 0) {
    float input = Serial.readString().toFloat();
    Serial.println(input);
    setDuty(input);
  }
}
