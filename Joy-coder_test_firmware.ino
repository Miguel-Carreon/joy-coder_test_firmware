#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

// GPIO definitions //
//Pinout definitions for the Joystick
#define xAxisPin A2
#define yAxisPin A3

//Pinout definitions for encoder number 1
#define encoderCLK 15
#define encoderDT 14

//Pinout definitions for encoder number 2 (Optional, not yet added)
#define encoder2CLK A1
#define encoder2DT A0

// Variable declarations //
//Joystick
int xZero, yZero;
int xValue, yValue;
const int sensitivity = 300; //The lower the number the higher the sensitivity
int invertMouse = -1;         //-1 for noninverted joystick based on orientation | 1 for inverted joystick based on orientation

//Encoders
ClickEncoder *encoder; //Variable representing the rotary encoder
ClickEncoder *encoder2; //Variable representing the seccond rotary encoder
int16_t last, value; //Variables for current and last rotation value
int16_t last2, value2; //Variables for current and last rotation value for the seccond encoder

void timerIsr() {
  encoder->service(); //Service method from the library for the encoder
  encoder2->service(); //Service method from the library for the seccond encoder
}

void setup() {
  
  //Initialize serial port, 115200 baud rate
  Serial.begin(115200);

  //Set these pins as inputs
  pinMode(xAxisPin, INPUT);
  pinMode(yAxisPin, INPUT);
  pinMode(encoderCLK, INPUT);
  pinMode(encoderDT, INPUT);
  pinMode(encoder2CLK, INPUT);
  pinMode(encoder2DT, INPUT);
  delay(1000);

  //Initial read for the joystick
  xZero = analogRead(xAxisPin);
  yZero = analogRead(yAxisPin);
  
  //Start mouse emulation
  Mouse.begin();

  //Start keyboard emulation
  Keyboard.begin();

  //Initialize encoder
  encoder = new ClickEncoder(encoderDT, encoderCLK); //Encoder pinout definitions
  encoder2 = new ClickEncoder(encoder2DT, encoder2CLK); //Encoder 2 pinout definitions

  //Interrupt sub-routine setup
  Timer1.initialize(1000); //Initialize the timer
  Timer1.attachInterrupt(timerIsr); //Attaches the timer sub-routine with the Timer 1 interruption
  
  //Encoders starting position
  last = -1; //Stores last value of the encoder's position
  last2 = -1; //Stores last value of the seccond encoder's position

}

void loop() {
  
  //Debounce timing variables
  static unsigned long lastIteration; //Stores the time elapsed in the las iteration
  static unsigned long lastIteration2;
  unsigned long iterationTime = millis(); //Stores the time elapsed until the current iteration
  unsigned long iterationTime2 = millis();

  //Read values from joystick
  xValue = analogRead(xAxisPin) - xZero;
  yValue = analogRead(yAxisPin) - yZero;

  //Read values from encoders
  value += encoder->getValue();
  value2 += encoder2->getValue();


  // Joystick activates //
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
      //delay(200); //fix this delay issue
      Mouse.release(MOUSE_MIDDLE);
    }
    
  }

  // Encoders activate //

  //Debounced encoder 1 funtion (50 ms debounce)
  if (value != last){

    if (iterationTime - lastIteration > 50){
      if (last < value){
        //CW function
        Keyboard.press(KEY_DOWN_ARROW);
        Keyboard.release(KEY_DOWN_ARROW);
      }

      else {
        //CCW function
        Keyboard.press(KEY_UP_ARROW);
        Keyboard.release(KEY_UP_ARROW);
      }

      lastIteration = iterationTime; //Store last iteration's time value
      
    }

    last = value; //Store last position
    
  }

  //Debounced encoder 2 function (50 ms debounce)
  if (value2 != last2){

    if (iterationTime2 - lastIteration2 > 50){
      if (last2 < value2){
        //CW function
        Keyboard.press(KEY_RIGHT_ARROW);
        Keyboard.release(KEY_RIGHT_ARROW);
      }

      else {
        //CCW function
        Keyboard.press(KEY_LEFT_ARROW);
        Keyboard.release(KEY_LEFT_ARROW);
      }

      lastIteration2 = iterationTime2; //Store last iteration's time value
      
    }

    last2 = value2; //Store last position
    
  }

}