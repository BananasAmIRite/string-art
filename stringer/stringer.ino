#include <TMCStepper.h>
#include <AccelStepper.h>
#include <SoftwareSerial.h>




#define MICROSTEPS 4
#define THREADER_MICROSTEPS 4
#define BIG_GEAR 192
#define SMALL_GEAR 20
#define NUM_OF_PEGS 200
long stepsPerRotation = ((long)MICROSTEPS*200*BIG_GEAR)/SMALL_GEAR;




#define EN_PIN 6
#define STEP_PIN 3
#define DIR_PIN 2
#define SW_RX 4
#define SW_TX 5
#define DRIVER_ADDRESS 0b00  
#define R_SENSE 0.11f
SoftwareSerial SoftSerial(SW_RX, SW_TX);
TMC2209Stepper TMCdriver(&SoftSerial, R_SENSE, DRIVER_ADDRESS);
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);




#define EN_PIN_1 11
#define STEP_PIN_1 8
#define DIR_PIN_1 7
#define SW_RX_1 10
#define SW_TX_1 9
#define DRIVER_ADDRESS_1 0b00
#define R_SENSE_1 0.11f
SoftwareSerial SoftSerial1(SW_RX_1, SW_TX_1);
TMC2209Stepper TMCdriver1(&SoftSerial1, R_SENSE_1, DRIVER_ADDRESS_1);
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1);




int pegBuffer[2] = {-1,-1};
bool drawBuffer[2] = {true, true};




void configureAngleStepper() {
  SoftSerial.begin(11520);           // initialize software serial for UART motor control
  TMCdriver.beginSerial(11520);      // Initialize UART
 
  pinMode(EN_PIN, OUTPUT);           // Set pinmodes
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);         // Enable TMC2209 board




  TMCdriver.begin();                 // UART: Init SW UART (if selected) with default 115200 baudrate
  TMCdriver.toff(5);                 // Enables driver in software
  TMCdriver.rms_current(800);        // Set motor RMS current
  TMCdriver.microsteps(MICROSTEPS);  // Set microsteps




  TMCdriver.en_spreadCycle(false);
  TMCdriver.pwm_autoscale(true);     // Needed for stealthChop




  stepper.setMaxSpeed(MICROSTEPS*1250/2);
  stepper.setAcceleration(2500);
  stepper.setPinsInverted(true, false, false); // invert the stepper direction to flip image
}




void configureThreaderStepper() {
  SoftSerial1.begin(11520);           // initialize software serial for UART motor control
  TMCdriver1.beginSerial(11520);      // Initialize UART
 
  pinMode(EN_PIN_1, OUTPUT);          // Set pinmodes
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  digitalWrite(EN_PIN_1, LOW);        // Enable TMC2209 board  




  TMCdriver1.begin();                 // UART: Init SW UART (if selected) with default 115200 baudrate
  TMCdriver1.toff(5);                 // Enables driver in software
  TMCdriver1.rms_current(500);        // Set motor RMS current
  TMCdriver1.microsteps(THREADER_MICROSTEPS);  // Set microsteps




  TMCdriver1.en_spreadCycle(false);
  TMCdriver1.pwm_autoscale(true);     // Needed for stealthChop




  stepper1.setMaxSpeed(2500);
  stepper1.setAcceleration(15000);
}




void setup() {
  Serial.begin(115200);
  configureAngleStepper();
  configureThreaderStepper();
}




long pegsToSteps(long pegs) {
  return (long) ((pegs*stepsPerRotation) / NUM_OF_PEGS);
}




long stepsToPegs(long steps) {
  return (long) (steps * NUM_OF_PEGS /stepsPerRotation);
}




long currentPeg() {
  return stepsToPegs(stepper.currentPosition()) % NUM_OF_PEGS;
}




void moveTo(long pegNumber) {
  pegNumber = pegNumber % NUM_OF_PEGS;
  if (stepper.currentPosition() - pegsToSteps(pegNumber) < -stepsPerRotation/2) {
    stepper.setCurrentPosition(stepper.currentPosition() + stepsPerRotation);
  } else if (stepper.currentPosition() - pegsToSteps(pegNumber) > stepsPerRotation/2) {
    stepper.setCurrentPosition(stepper.currentPosition() - stepsPerRotation);
  }
  stepper.moveTo(pegsToSteps(pegNumber));
  stepper.runToPosition();
}




void moveBy(long pegNumber) {
  moveTo((stepsToPegs(stepper.currentPosition())+pegNumber)%NUM_OF_PEGS);
}




void extend() {
  stepper1.move(THREADER_MICROSTEPS*200*0.22);
  stepper1.runToPosition();
}




void retract() {
  stepper1.moveTo(0);
  stepper1.runToPosition();
}




int mod(int i, int j) {
  int k = i % j;
  if (k<0) {
    k+=j;
  }
  return k;
}




//Given last string was from peg1 and next string is to peg3, jump to peg2
void jumpPeg(int peg1, int peg2, int peg3) {
 
  pegBuffer[0] = pegBuffer[1];
  pegBuffer[1] = peg3;
  if (peg2 == peg1) {
    return;
  }
  bool dir = (mod((peg3-peg2),NUM_OF_PEGS))>NUM_OF_PEGS;
  if (dir) {
    //CW
    retract();
    moveTo(peg2+1);
  } else {
    //CCW
    retract();
    moveTo(peg2);
  }




}




void jumpTo(int peg) {
  drawBuffer[0] = drawBuffer[1];
  drawBuffer[1] = false;
  if (drawBuffer[0]) {
    drawString(pegBuffer[0],pegBuffer[1],peg);
  } else {
    jumpPeg(pegBuffer[0],pegBuffer[1],peg);
  }




}




//Given last string was from peg1 and next string is to peg3, draw to peg2
void drawString(int peg1, int peg2, int peg3) {




  pegBuffer[0] = pegBuffer[1];
  pegBuffer[1] = peg3;
  if (peg2 == peg1) {
    return;
  }
  bool dir = (mod((peg3-peg2),NUM_OF_PEGS)-mod((peg1-peg2),NUM_OF_PEGS))<0;
  Serial.print("dir: ");
  Serial.println(dir);
  Serial.print("drawBuffer[1]: ");
  Serial.println(drawBuffer[1]);
  if (dir == drawBuffer[1]) {
    //CW
    extend();
    moveTo(peg2);
    retract();
    moveTo(peg2+1);
  } else {
    //CCW
    extend();
    moveTo(peg2+1);
    retract();
    moveTo(peg2);
  }




}




void drawTo(int peg) {
  drawBuffer[0] = drawBuffer[1];
  drawBuffer[1] = true;
  if (drawBuffer[0]) {
    drawString(pegBuffer[0],pegBuffer[1],peg);
  } else {
    jumpPeg(pegBuffer[0],pegBuffer[1],peg);
  }
}




void finishPath() {
  jumpTo(0);
}




void drawMod(int mod) {
  retract();
  int peg = NUM_OF_PEGS;
  while(peg != 0) {
    peg=(peg+mod)%NUM_OF_PEGS;
    drawTo(peg);
  }
  finishPath();
}




void drawTable(int mult) {
  for (int i = 0; i < NUM_OF_PEGS; i++) {
    Serial.print("i : ");
    Serial.print(i);
    Serial.print("    (i*mult)%NUM_OF_PEGS: ");
    Serial.println((i*mult)%NUM_OF_PEGS);
    if (i != (i*mult)%NUM_OF_PEGS) {
      // // Serial.print("Jumping to ");
      // Serial.println(i);
      // Serial.print("Pegbuffer: ");
      // Serial.print(pegBuffer[0]);
      // Serial.print(" ");
      // Serial.println(pegBuffer[1]);
      jumpTo(i);
      // delay(5000);




      // Serial.print("Drawing to ");
      // Serial.println(i*mult);
      // Serial.print("Pegbuffer: ");
      // Serial.print(pegBuffer[0]);
      // Serial.print(" ");
      // Serial.println(pegBuffer[1]);
      drawTo(i*mult);
      // delay(5000);




      // moveTo(i+1);
      // retract();
      // moveTo(i);
      // extend();
      // // moveTo(i+1);
      // // extend();




      // moveTo(i*mult+1);
      // retract();
      // moveTo(i*mult);
      // extend();
      // moveTo(i*mult+1);
      // retract();




    }




  }
  finishPath();
}




void loop() {




  if (Serial.available()) {
    String str = Serial.readStringUntil('\n');
    if (str.charAt(0) == 'G') {
      int num = str.substring(1).toInt();
      moveTo(num);
    } else if (str.charAt(0) == 'M') {
      int num = str.substring(1).toInt();
      moveBy(num);
    } else if (str.charAt(0) == 'S') {
      int num = str.substring(1).toInt();
      drawMod(num);
    } else if (str.charAt(0) == 'T') {
      int num = str.substring(1).toInt();
      drawTable(num);
    } else if (str.charAt(0) == 'D') {
      int num = str.substring(1).toInt();
      drawTo(num);
    } else if (str.charAt(0) == 'J') {
      int num = str.substring(1).toInt();
      jumpTo(num);
    } else if (str.charAt(0) == 'E') {
      extend(); 
    } else if (str.charAt(0) == 'R') {
      retract(); 
    }
    delay(300);
    Serial.println("DONE");
  }




}






