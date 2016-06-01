#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

byte pwmSteps = 32; // number of steps/ticks during a pwm period
byte pwmFrequency = 100;
byte cpuFrequency = 8000000;
byte cpuTicksPerPWMTick = cpuFrequency / (pwmSteps * pwmFrequency);
byte intensity[5] = {16,16,16,16,16}; // translates into duty cycle (0 - 31)
// assigning fingers to pins
// D2 = SDA
byte fingers[5] = {2, 10, 9, 6, 12};
// represents the right hand
byte leftRight = 1;
volatile int steps = 0;

Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN);

void setup(void)
{
  delay(500); 
  Serial.begin(9600); // begin serial communication for debugging via usb

  ble.begin(); // start bluetooth transmission
  ble.echo(false);

  // setting all pins representing fingers to output
  for (byte i = 0; i < 5; i++){
    pinMode(fingers[i], OUTPUT);
  }

  cli(); // disable interrupts globally
  OCR0A = cpuTicksPerPWMTick; // setup output compare register
  TCCR1B = 1;
  TIMSK1 |= (1<<OCIE1A);
  sei(); // enable interrupts globally
}


void loop() {
  while (ble.available()) // while data is in the receive buffer of the bluetooth module
  {
    byte incoming = ble.read(); // read the command
    adjustIntensity(incoming);
    respond(incoming);
  }
   
   digitalWrite(fingers[0], steps < intensity[0]);
   digitalWrite(fingers[0], steps < intensity[1]);
   digitalWrite(fingers[0], steps < intensity[2]);
   digitalWrite(fingers[0], steps < intensity[3]);
   digitalWrite(fingers[0], steps < intensity[4]);
}

ISR(TIMER1_COMPA_vect){
  if (steps++ >= pwmSteps - 1){
    steps = 0;
  }
}


void respond(byte incoming){
  if (incoming == 0) {
    ble.println(leftRight);
    Serial.println("glove represents " + String(leftRight ? "right" : "left") + " hand");
  }
}

void adjustIntensity(byte incoming){
  if (incoming > 0){ // 0 byte is used to ask for left hand / right hand status
    byte _finger = (incoming & 0b00000111) - 1;
    byte _intensity = incoming >> 3;
    intensity[_finger] = _intensity;
    Serial.println("intensity of " + String(_finger == 0 ? "thumb" 
                                          : _finger == 1 ? "index" 
                                          : _finger == 2 ? "middle" 
                                          : _finger == 3 ? "index" 
                                          : "pinky") + " set to " + _intensity);
  }
}

