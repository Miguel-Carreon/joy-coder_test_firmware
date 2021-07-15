#include <HID-Project.h>
#include <TimerOne.h>
#include <ClickEncoder.h>

// GPIO definitions //
//Pinout definitions for the Joystick
#define xAxisPin A2
#define yAxisPin A3

//Pinout definitions for encoder number 1
#define encoderCLK A1
#define encoderDT A0

//Pinout definitions for encoder number 2 (Optional, not yet added)


// Variable declarations //
//Joystick
int xZero, yZero;
int xValue, yValue;
const int sensitivity = 300;
int invertMouse = -1;         //-1 for noninverted joystick based on orientation | 1 for inverted joystick based on orientation

//Encoder 1
ClickEncoder *encoder; //Variable representing the rotary encoder
int16_t last, value; //Variables for current and last rotation value

void timerIsr() {
  encoder->service();
}

void setup() {
  
  //Initialize serial port, 9600 boud rate
  Serial.begin(9600);

  //Set these pins as inputs
  pinMode(xAxisPin, INPUT);
  pinMode(yAxisPin, INPUT);
  pinMode(encoderCLK, INPUT);
  pinMode(encoderDT, INPUT);
  delay(1000);

  //Initial read for the joystick
  xZero = analogRead(xAxisPin);
  yZero = analogRead(yAxisPin) ;
  
  //Start mouse emulation
  Mouse.begin();

  //Start keyboard emulation
  Keyboard.begin();

  //Initialize encoder
  encoder = new ClickEncoder(encoderDT, encoderCLK); //Encoder pinout definitions

  //Setup for the encoder
  Timer1.initialize(1000); //Initialize the timer
  Timer1.attachInterrupt(timerIsr);
  last = -1;

}

void loop() {
  
  //Read values from joystick
  xValue = analogRead(xAxisPin) - xZero;
  yValue = analogRead(yAxisPin) - yZero;

  //Read values from encoder
  value += encoder->getValue();

  //Joystick activates
  if ((xValue != 0) || (yValue != 0)){
    
    if ((xValue > 10) || (xValue < -10)){
      Mouse.move((invertMouse * (xValue / sensitivity)), 0, 0);
      Mouse.press(MOUSE_MIDDLE);
      delay(5);
    }

    if ((yValue > 10) || (yValue < -10)){
      Mouse.move(0, (invertMouse * (yValue / sensitivity)), 0);
      Mouse.press(MOUSE_MIDDLE);
      delay(5);
    }

    if ((xValue >= -10) && (xValue <= 10) && (yValue >= -10) && (yValue <= 10)){
      delay(200);
      Mouse.release(MOUSE_MIDDLE);
    }
    
  }

  //Encoder activates
  if (value != last){
    
    if (last < value){
      //Serial.println("CW");
      Keyboard.press(KEY_DOWN_ARROW);
    }
    
    else {
      //Serial.println("CCW");
      Keyboard.press(KEY_UP_ARROW);
    }

    delay(100);
    Keyboard.releaseAll();
    last = value;
  }

}
