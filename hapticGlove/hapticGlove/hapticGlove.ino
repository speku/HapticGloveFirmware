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

  // initializing serial connection
  Serial.begin(9600);
  // wait for serial port to establish a connection
  while(!Serial);
}

void loop() {
  // nothing needs to be done here
}

// interrupt routine invoked when new data has been received
void serialEvent(){
  // get the incoming byte
  byte incoming = Serial.read();
  // check the bits (only the 5 leftmost bits are relevant) to trigger vibration
  for (byte i = 0; i < 5; i++){
    vibrate(fingers[i], incoming, i);
  }
  // returns a constant indicating which hand is represented by this code/device
  answerBack(incoming);
}

// sets the pin assigned to fingers to high or low depending on the received byte
void vibrate(byte pin, byte incoming, byte bitn){
    digitalWrite(pin, bitRead(incoming, bitn) ? HIGH : LOW);
}

void answerBack(byte incoming){
  if (bitRead(incoming, handBit)) Serial.println(leftRight);
}

