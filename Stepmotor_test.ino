#include <Stepper.h>
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// ULN2003 Motor Driver Pins
#define IN1 22
#define IN2 24
#define IN3 26
#define IN4 28

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
float voltage = 0.00;
int potentValue;
int motorPosition = 0;
int desiredMotorPosition;
int motorChange;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  myStepper.setSpeed(5);
  myStepper.step(0);
  // allow time for serial monitor to open
  delay(3000);
}


void loop() {
  //calibrate motor
  //myStepper.step(stepsPerRevolution);
  //delay(500);
  //myStepper.step(-stepsPerRevolution);
  //delay(500);

  // read the input on analog pin 8:
  delay(3000); // allow time for setting to be selected
  potentValue = analogRead(A8);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = potentValue * (5.0 / 1023.0);

  // print out the value you read:
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.print("Potentiometer voltage: ");
  Serial.println(voltage);

  //convert potentiometer voltage to changes in motor (0V = -stepsPerRevolution, 5V = stepsPerRevolution):
  Serial.print("Motor position: ");
  Serial.println(motorPosition);
  desiredMotorPosition = ((819.2 * voltage) - stepsPerRevolution);
  Serial.print("Desired motor position: ");
  Serial.println(desiredMotorPosition);
  motorChange = (desiredMotorPosition - motorPosition);
  // step motor to desired positon:
  Serial.print("Motor changing: ");
  Serial.println(motorChange);
  myStepper.step(motorChange);
  motorPosition = motorPosition + motorChange;
  delay(1000);

}
