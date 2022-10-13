#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "fscale.h"

/* This code seeks to control Will Cogley's 3D printed animatronic eye in a slightly more advanced way, while also adding simple random automation.
 * The Adafruit PWM Servo Driver library is required, as this code uses a PCA9685 to drive the 6 servos (similar to the original).
 * This design is controlled by a joystick (looking around), a slider (eye wideness), a dial (eyelid bias up/down), and a button (blink) with a momentary two direction switch (or two buttons) selecting the operating mode.
 * Note that my slider was not linear, so I have included fscale to linearize it. This can be tweaked pretty easily by setting sliderExp below to 0.
 * This code is free to use (just make sure my name and the names of those I copied code from stays with it!) for whatever.
 * Written by Nicholas Schwankl (Syber-Space) at Wake Technical Community College - 10/2022
 */

// ++++++++++ Servo Controller Settings ++++++++++ //

Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();

#define SERVOMIN  95 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  435 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates


// ++++++++++ Pin Definitions ++++++++++ //

#define vertJoy       A0
#define horJoy        A1
#define slider        A3
#define dial          A2
#define blinkButton   5
#define selectLeft    3
#define selectRight   4
#define buttonLight   6


// ++++++++++ Servo Names ++++++++++ //

#define vertEye     1
#define horEye      0
#define lowerL      3
#define upperL      2
#define lowerR      5
#define upperR      4


// ++++++++++ Calibration Values ++++++++++ //

const int vertJoyMAX =  0;
const int vertJoyMIN =  1022;
const int horJoyMAX =   1000;
const int horJoyMIN =   0;
const int sliderMAX =   580;
const int sliderMIN =   0;
const int dialMAX =     0;
const int dialMIN =     1023;

const float sliderExp = -8.5;
// Used as fscale(sliderMIN, sliderMAX, [low], [high], analogRead(slider), sliderExp)

const int lidScale = 200;
const float moveInfluence = 0.4;
const float offsetInfluence = 0.5;


// ++++++++++ Servo Effective Limits ++++++++++ //

const int vertEyeMAX =  320; //
const int vertEyeCEN =  265; //
const int vertEyeMIN =  210; //
const int horEyeMAX =   345; //
const int horEyeCEN =   260; //
const int horEyeMIN =   175; //
const int lowerLMAX =   320; //
const int lowerLCEN =   285; //
const int lowerLMIN =   145; //
const int upperLMAX =   380; //
const int upperLCEN =   255; //
const int upperLMIN =   200; //
const int lowerRMAX =   180; //
const int lowerRCEN =   230; //
const int lowerRMIN =   395; //
const int upperRMAX =   150; //
const int upperRCEN =   275; //
const int upperRMIN =   320; //


// ++++++++++ Calculated Constants ++++++++++ //

const int vertDivMAX = max(abs(vertEyeMIN - vertEyeCEN), abs(vertEyeMAX - vertEyeCEN));
const int horDivMAX = max(abs(horEyeMIN - horEyeCEN), abs(horEyeMAX - horEyeCEN));
const int divMAX = max(vertDivMAX, horDivMAX); // Find a scale factor for the largest limit

const int lowerLSign = lowerLCEN > lowerLMIN ? 1 : -1; // Find the sign for the servo direction
const int upperLSign = upperLCEN > upperLMIN ? 1 : -1;
const int lowerRSign = lowerRCEN > lowerRMIN ? 1 : -1;
const int upperRSign = upperRCEN > upperRMIN ? 1 : -1;


// ++++++++++ Variables ++++++++++ //

int horMapped = 0;
int vertMapped = 0;
int openMapped = 0;
int offsetMapped = 0;

int lastXPos = 0;
int lastYPos = 0;
int lastOpenL = 0;
int lastOpenR = 0;
int lastOffsetL = 0;
int lastOffsetR = 0;

volatile bool programMode = false;






// ++++++++++ Functions ++++++++++ // ++++++++++ Functions ++++++++++ // ++++++++++ Functions ++++++++++ // ++++++++++ Functions ++++++++++ //



void switchToManual(){ // Switch to manual control when interrupt is triggered
  programMode = false;
  digitalWrite(buttonLight, HIGH);
}



void updateInputs(){ // Update all inputs

  horMapped = map( constrain(analogRead(horJoy), min(horJoyMIN, horJoyMAX), max(horJoyMIN, horJoyMAX)), horJoyMIN, horJoyMAX, -1000, 1000);
  vertMapped = map( constrain(analogRead(vertJoy), min(vertJoyMIN, vertJoyMAX), max(vertJoyMIN, vertJoyMAX)), vertJoyMIN, vertJoyMAX, -1000, 1000);
  openMapped = fscale(sliderMIN, sliderMAX, 0, 1000, constrain(analogRead(slider), min(sliderMIN, sliderMAX), max(sliderMIN, sliderMAX)), sliderExp);
  offsetMapped = map( constrain(analogRead(dial), min(dialMIN, dialMAX), max(dialMIN, dialMAX)), dialMIN, dialMAX, -1000, 1000);
  if(!digitalRead(blinkButton)){
    openMapped = 0;
    offsetMapped = 0;
  }
}



void moveEyesAdv(int xPos, int yPos, int lidOpenL, int lidOpenR, int lidOffsetL, int lidOffsetR){ // Move eyes to coordinate

  lastXPos = xPos;
  lastYPos = yPos;
  lastOpenL = lidOpenL;
  lastOpenR = lidOpenR;
  lastOffsetL = lidOffsetL;
  lastOffsetR = lidOffsetR;

  // These functions map inputs from +- 1000 to the servo limits, scaled based on the furthest limit
  int vertEyeVal = constrain(map(yPos, -1000, 1000, vertEyeCEN-divMAX, vertEyeCEN+divMAX), vertEyeMIN, vertEyeMAX);
  int horEyeVal = constrain(map(xPos, -1000, 1000, horEyeCEN-divMAX, horEyeCEN+divMAX), horEyeMIN, horEyeMAX);

  lidOffsetL = lidOffsetL + yPos*moveInfluence;
  lidOffsetR = lidOffsetR + yPos*moveInfluence;
  
  int upperLVal = map(constrain((lidOpenL + lidOffsetL*offsetInfluence), -1000, 1000), -1000, 1000, upperLCEN - upperLSign*lidScale, upperLCEN + upperLSign*lidScale);
  int lowerLVal = map(constrain((-lidOpenL + lidOffsetL*offsetInfluence), -1000, 1000), -1000, 1000, lowerLCEN - upperLSign*lidScale, lowerLCEN + lowerLSign*lidScale);
  int upperRVal = map(constrain((lidOpenR + lidOffsetR*offsetInfluence), -1000, 1000), -1000, 1000, upperRCEN - upperRSign*lidScale, upperRCEN + upperRSign*lidScale);
  int lowerRVal = map(constrain((-lidOpenR + lidOffsetR*offsetInfluence), -1000, 1000), -1000, 1000, lowerRCEN - lowerRSign*lidScale, lowerRCEN + lowerRSign*lidScale);

  int upperModifier = (vertEyeVal - vertEyeCEN);
  upperModifier = upperModifier > 0 ? 0 : upperModifier;
  
  upperLVal = constrain(upperLVal, min(upperLMIN, upperLMAX + upperLSign*upperModifier), max(upperLMIN, upperLMAX + upperLSign*upperModifier));
  lowerLVal = constrain(lowerLVal, min(lowerLMIN, lowerLMAX), max(lowerLMIN, lowerLMAX));
  upperRVal = constrain(upperRVal, min(upperRMIN, upperRMAX + upperRSign*upperModifier), max(upperRMIN, upperRMAX + upperRSign*upperModifier));
  lowerRVal = constrain(lowerRVal, min(lowerRMIN, lowerRMAX), max(lowerRMIN, lowerRMAX));
  
  servo.setPWM(vertEye, 0, vertEyeVal);
  servo.setPWM(horEye, 0, horEyeVal);
  servo.setPWM(upperL, 0, upperLVal);
  servo.setPWM(lowerL, 0, lowerLVal);
  servo.setPWM(upperR, 0, upperRVal);
  servo.setPWM(lowerR, 0, lowerRVal);
  
}

void moveEyes(int xPos, int yPos, int lidOpen, int lidOffset){
  moveEyesAdv(xPos,yPos,lidOpen,lidOpen,lidOffset,lidOffset);
}



void linearMoveAdv(int xPos, int yPos, int lidOpenL, int lidOpenR, int lidOffsetL, int lidOffsetR, int moveTime){
  
  moveTime = moveTime / 10;
  
  float xDif = float(xPos - lastXPos) / moveTime;
  float yDif = float(yPos - lastYPos) / moveTime;
  float lidOpenLDif = float(lidOpenL - lastOpenL) / moveTime;
  float lidOpenRDif = float(lidOpenR - lastOpenR) / moveTime;
  float lidOffsetLDif = float(lidOffsetL - lastOffsetL) / moveTime;
  float lidOffsetRDif = float(lidOffsetR - lastOffsetR) / moveTime;

  float storeX = lastXPos;
  float storeY = lastYPos;
  float storeOpenL = lastOpenL;
  float storeOpenR = lastOpenR;
  float storeOffsetL = lastOffsetL;
  float storeOffsetR = lastOffsetR;
  
  for(int i = 0; i < moveTime; i++){
    moveEyesAdv(
      storeX + (xDif * i),
      storeY + (yDif * i),
      storeOpenL + (lidOpenLDif * i),
      storeOpenR + (lidOpenRDif * i),
      storeOffsetL + (lidOffsetLDif * i),
      storeOffsetR + (lidOffsetRDif * i)
      );
    delay(10);
  }
  moveEyesAdv(xPos, yPos, lidOpenL, lidOpenR, lidOffsetL, lidOffsetR);
  
}

void linearMove(int xPos, int yPos, int lidOpen, int lidOffset, int moveTime){
  linearMoveAdv(xPos, yPos, lidOpen, lidOpen, lidOffset, lidOffset, moveTime);
}

#include "moveScripts.h"



// ++++++++++ Run ++++++++++ // ++++++++++ Run ++++++++++ // ++++++++++ Run ++++++++++ // ++++++++++ Run ++++++++++ // ++++++++++ Run ++++++++++ //

void setup() {

  pinMode(vertJoy,INPUT);
  pinMode(horJoy, INPUT);
  pinMode(slider, INPUT);
  pinMode(dial,   INPUT);
  pinMode(blinkButton,  INPUT_PULLUP);
  pinMode(selectLeft,   INPUT_PULLUP);
  pinMode(selectRight,  INPUT_PULLUP);
  pinMode(buttonLight,  OUTPUT);

  attachInterrupt(digitalPinToInterrupt(selectLeft), switchToManual, CHANGE);
  
  servo.begin();
  servo.setOscillatorFrequency(27000000);
  servo.setPWMFreq(SERVO_FREQ);

  delay(10);

  for(int i = 0; i<6; i++){
    servo.setPWM(i, 0, (SERVOMIN + SERVOMAX)/2);
    delay(100);
  }

  Serial.begin(9600);
  
  delay(1000);

  updateInputs();

}


void loop() {
  
  while (!programMode){

    analogWrite(buttonLight, 80);
    
    updateInputs();
    moveEyes(horMapped, vertMapped, openMapped, offsetMapped);
    
    if(!digitalRead(selectRight)){
      programMode = true;
    }
    
    delay(10);
  }
  
  digitalWrite(buttonLight, LOW);
  
  while (programMode){

    moveScript[random(moveScriptsNum)]();
    analogWrite(buttonLight, 120);
    delay(10);
    digitalWrite(buttonLight, LOW);
    
  }

}
