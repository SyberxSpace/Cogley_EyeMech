// This file contains all routines used for random Auto movement.
// General format used from https://arduino.stackexchange.com/a/21097

void moveBlink(){
  int backOpenL = lastOpenL;
  int backOpenR = lastOpenR;
  int backOffsetL = lastOffsetL;
  int backOffsetR = lastOffsetR;
  moveEyesAdv(lastXPos, lastYPos, 0, 0, 0, 0);
  delay(150);
  moveEyesAdv(lastXPos, lastYPos, backOpenL, backOpenR, backOffsetL, backOffsetR);
  delay(random(100, 1000));
}

void moveTwitch(){
  for(int i = 0; i < random(1,4); i++){
    moveEyes(
      constrain(lastXPos + random(-150, 150), -1000, 1000),
      constrain(lastYPos + random(-150, 150), -1000, 1000),
      constrain(lastOpenL + random(-10, 10), 0, 1000),
      constrain(lastOffsetL + random(-10, 10), -1000, 1000));
    delay(random(50, 200));
  }
  delay(random(200, 1000));
}

void moveRandom(){
  linearMove(random(-900, 900), random(-800, 650), random(100, 600), random(-300, 300), random(100, 800));
  delay(random(200, 1000));
}

void moveRandomMiddle(){
  linearMove(random(-700, 700), random(-300, 300), random(100, 600), random(-300, 300), random(100, 600));
  delay(random(200, 1000));
}

void moveStare(){
  delay(random(100,1000));
}


// ++++++++++ // ++++++++++ // ++++++++++ // ++++++++++ // ++++++++++ // ++++++++++ // ++++++++++ // ++++++++++ // ++++++++++ //

typedef void (*num_func) ();

int moveScriptsNum = 10;
num_func moveScript[] = {
  moveRandom,
  moveRandomMiddle,
  moveBlink, moveBlink,
  moveTwitch, moveTwitch, moveTwitch, moveTwitch,
  moveStare, moveStare
};
