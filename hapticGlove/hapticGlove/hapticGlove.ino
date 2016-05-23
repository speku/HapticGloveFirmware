// stuff for soft pwm

int pwmSteps = 64;
int intensity = 20; // translates into duty cycle (0 - 63)

byte incoming;
byte pwmMask;
byte newData;

// assigning fingers to pins - starts of from thumb
byte fingers[5] = {1, 2, 3, 4, 5};

// represents the left hand
byte leftRight = 0;
byte handBit = 7;

void setup() {

  // setting all pins representing fingers to output
  for (byte i = 0; i < 5; i++){
    pinMode(fingers[i], OUTPUT);
  }

  // pwm init
  // disable interrupts
  cli();
  OCR0A = 2500;
  TCCR1B = 1;
  TIMSK1 |= (1<<OCIE1A);
  // enable interrupts
  sei();

  // initializing serial connection
  Serial.begin(9600);
  // wait for serial port to establish a connection
  while(!Serial);
}

void loop() {
  if (newData){
    // check the bits (only the 5 leftmost bits are relevant) to trigger vibration
    for (byte i = 0; i < 5; i++){
      vibrate(fingers[i], incoming & pwmMask, i);
    }
     // returns a constant indicating which hand is represented by this code/device
    answerBack(incoming);
    newData = 0;
  }
}


ISR(TIMER1_COMPA_vect){
  static uint8_t steps = 0;
  pwmMask = steps > intensity ? 0xFF : 0;
  if (steps++ >= pwmSteps - 1){
    steps = 0;
  }
}

// interrupt routine invoked when new data has been received
void serialEvent(){
  if (!Serial.available()) return;
  byte incoming = Serial.read();
  newData = 1;
}

// sets the pin assigned to fingers to high or low depending on the received byte
void vibrate(byte pin, byte incoming, byte bitn){
    digitalWrite(pin, bitRead(incoming, bitn) ? HIGH : LOW);
}

void answerBack(byte incoming){
  if (bitRead(incoming, handBit)) Serial.println(leftRight);
}

