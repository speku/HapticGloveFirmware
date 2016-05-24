// stuff for soft pwm

int pwmSteps = 32;
int intensity[5] = {16,16,16,16,16}; // translates into duty cycle (0 - 63)

byte incoming;
byte pwmMask[5] = {0,0,0,0,0};
byte newData;

// assigning fingers to pins - starts of from thumb
byte fingers[5] = {1, 2, 3, 4, 5};

// represents the left hand
byte leftRight = 0;

void setup() {

  // setting all pins representing fingers to output
  for (byte i = 0; i < 5; i++){
    pinMode(fingers[i], OUTPUT);
  }

  // pwm init
  // disable interrupts
  cli();
  OCR0A = 5000 / 5; // account for each finger
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
      vibrate(fingers[i], incoming & pwmMask[i], i);
    }
     // returns a constant indicating which hand is represented by this code/device
    newData = 0;
  }
}


ISR(TIMER1_COMPA_vect){
  static byte steps = 0;
  static byte finger = 1;
  finger %= 5;
  pwmMask[finger] = steps < intensity[finger] * 5 ? 0xFF : 0;
  if (steps++ >= pwmSteps * 5 - 1){
    steps = 0;
  }
  finger++;
}

// interrupt routine invoked when new data has been received
void serialEvent(){
  if (!Serial.available()) return;
  byte incoming = Serial.read();
    if(!answerBack(incoming) && !adjustIntensity(incoming))newData = 1;
}

// sets the pin assigned to fingers to high or low depending on the received byte
void vibrate(byte pin, byte incoming, byte bitn){
    digitalWrite(pin, bitRead(incoming, bitn) ? HIGH : LOW);
}

byte answerBack(byte incoming){
  if (incoming == 1) {
    Serial.println(leftRight);
    return 1;
  }
  return 0;
}

byte adjustIntensity(byte incoming){
  byte i = incoming & 0b00000111;
  if (i > 0){ // if the rightmost 3 bits are not all 0 (is the case when vibration on/off info is submitted)
    intensity[i-1] = (incoming & 0b11111000) >> 3;// to offset 0 - has to be accounted for in client app
  }
}

