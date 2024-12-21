#define PIN_CAM     4
#define PIN_CRANK   5
#define PIN_POTI    A7

char crankCnt = 1;

// CONFIG -------------------------------------

#define TEETH 60 // 60
#define GAP 2 // -2 ~ minimum 1

int camOffset = 10; // 0 - end of missing pulse

#define INVERT_CRANK 1
#define INVERT_CAM 0

// --------------------------------------------

void setup()
{
  pinMode(PIN_CAM, OUTPUT);
  pinMode(PIN_CRANK, OUTPUT);

  cli(); //stop interrupts

  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

  sei(); //allow interrupts
}

ISR(TIMER0_COMPA_vect)
{
  //timer0 interrupt 2kHz

  // Crank Pulse
  if (crankCnt <= ((TEETH-GAP+1)*2) )
  {
    
    if ((crankCnt%2) != INVERT_CRANK){
      digitalWrite(PIN_CRANK, HIGH);
    }
    else{
      digitalWrite(PIN_CRANK, LOW);
    }
  }
  else if (crankCnt > (TEETH*2) )
  {
    crankCnt = 1;
  }

  // 1x Cam Pulse
  if ( (crankCnt > (camOffset + 2) ) &&
       (crankCnt <= (TEETH + camOffset + 2) ) )
  {
      digitalWrite(PIN_CAM, INVERT_CAM ? LOW : HIGH);
  }
  else
  {
      digitalWrite(PIN_CAM, INVERT_CAM ? HIGH : LOW);
  }

  crankCnt++;
}

void loop()
{
  // Read from a potentiometer the shift between crank and cam signal
  int potiVal = analogRead(PIN_POTI);
  camOffset = map(potiVal, 0, 1023, 0, TEETH);

  delay(10);
}
