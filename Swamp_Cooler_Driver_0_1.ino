// LIBRARIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <LiquidCrystal.h> // LCD Display
#include <Stepper.h> // Stepper Motor

// GLOBAL VARIABLES & MACROS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* NOTE -> each state has an assigned integer for ease of programming.
 *  see below table for guide. */
#define DISABLED 0
#define IDLING 1
#define ERRORSTATE 2
#define RUNNING 3 

#define IN1 22 // ULN2003 Stepper Motor Driver Pins
#define IN2 24
#define IN3 26
#define IN4 28

// LED # Codes
// NOTE - > all LEDs on Port C, 0 thru 3 (pins D34 - 37)
volatile unsigned char* port_c = (unsigned char*) 0x28; // LED Port
volatile unsigned char* ddr_c = (unsigned char*) 0x27;
#define RED 3
#define YLW 2
#define GRN 1
#define BLU 0

int state;
int runCount;

// Stepper Motor
const int stepsPerRevolution = (2048 / 2);  // change this to fit the number of steps per revolution, or set a max range of motion (2048 = 1 revolution for the 28BYJ-48 Stepper Motor)
const float maxPotentVoltage = 5.00; // max voltage expected from potentiometer
int motorPosition;

// LCD and Temperature/Humidity Sensor
float temp;
float humid;

// FUNCTION INITIALIZATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int stateCheckAndSwitch(void); // check if state switch is needed and return state #

void LEDon(int);
void LEDoff(int);
void allLEDoff(void);

void lcdDisplayError(void);
void lcdDisplayTempHumid(float, float);
void wipeLCD(void);

int updateVentPosition(void); // change vent position according to potentiometer setting and return steps changed
int getVentPosition(void); // return stepper motor position
void reportVentChange(int); // print changes to vent steps

// PIN INITIALIZATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LiquidCrystal lcd(52, 53, 48, 49, 46, 47);
  /*  ^^^ establishing pins for use by LCD in the following order:
   *  lcd(RS, E, DB4, DB5, DB6, DB7) - 4 data lines 
   *  for other syntaxes, see documentation for library:
   *  https://www.arduino.cc/reference/en/libraries/liquidcrystal/
   */
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4); // establishing signal pins for use by stepper motor driver componenent

// volatile unsigned char *portDDRB  = (unsigned char *) 0x24; // Reference syntax for GPIO
// volatile unsigned char *portB     = (unsigned char *) 0x25; // Reference syntax for GPIO

// SETUP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  Serial.begin(9600);
  Serial.println("Setup Block"); // For test purposes
  myStepper.setSpeed(5); // step motor settings
  myStepper.step(0);
  *ddr_c |= 0b00001111; // LED pins set to output
  delay(1000); // Delay to allow time for serial monitor to open
  
  // LCD
  lcd.begin(16, 2); //Tell the LCD that it is a 16x2 LCD
  temp = 72.5911;
  humid = 25.8744;

  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.println("Main Loop");
  Serial.print("State set to "); 
}

// MAIN LOOP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
  state = RUNNING; // Set initial state for test purposes
  switch (state) {
  /* NOTE ->  FOR ALL STATES, realtime clock must be used to report via serial port the
  * time of each state transition and any changes to the stepper motor
  * position for the vent. FOR ALL STATES EXCEPT FOR DISABLED, temp and humid
  * is continuously monitored and reported to LCD once per minute, system responds to
  * changes in vent position control, and the stop button should turn fan motor off (if on) and
  * system should go to DISABLED state.*/

  case DISABLED : // 0
    Serial.println("DISABLED"); // For test purposes
    allLEDoff();
    LEDon(YLW);
    reportVentChange(updateVentPosition());
    // report state changes
    break;

  case IDLING : // 1
    Serial.println("IDLING"); // For test purposes
    // report state changes
    allLEDoff();
    LEDon(GRN);
    lcdDisplayTempHumid(temp, humid);
    reportVentChange(updateVentPosition());
    // monitor temp and humidity
    // report temp and humidity to LCD once per minute
    // stop button turns off fan (if on) and change to DISABLED
    break;

  case ERRORSTATE : // 2
    Serial.println("ERRORSTATE"); // For test purposes
    // report state changes
    allLEDoff();
    LEDon(RED);
    lcdDisplayError();
    lcdDisplayTempHumid(temp, humid);
    reportVentChange(updateVentPosition());
    // monitor temp and humidity
    // report temp and humidity to LCD once per minute
    // stop button turns off fan (if on) and change to DISABLED
    break;

  case RUNNING : // 3
    Serial.println("RUNNING"); // For test purposes
    // report state changes
    allLEDoff();
    LEDon(BLU);
    lcdDisplayTempHumid(temp, humid);
    reportVentChange(updateVentPosition());
    // blue LED on, all other LEDs off
    // fan motor on
    // monitor temp and humidity
    // report temp and humidity to LCD once per minute
    // stop button turns off fan (if on) and change to DISABLED
    // change to IDLE if temp drops below threshold
    // change to ERROR if water becomes too low
    break;
 
  } // End switch-case block
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// MISC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int stateCheckAndSwitch(){ // check if state switch is needed and return state #
  
}

// LCD FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// NOTE -> 16 Characters per line
void lcdDisplayError(){ // Displays low water error message to LCD
  lcd.clear();
  lcd.setCursor(0, 0); //Set invisible cursor to first character, first line (x = 0, y = 0)
  lcd.print("     ERROR"); // Output text at cursor
  lcd.setCursor(0, 1); // Set invisible cursor to first character, second line (x = 0, y = -1)
  lcd.print(" Low Water Level");
  delay(5000); // Display time
  lcd.clear(); // Wipe display
  
  lcd.setCursor(0, 0); // Reset cursor
  lcd.print("  Refill Water");
  lcd.setCursor(0, 1);
  lcd.print("  & Press Reset");
  delay(5000);
  
  lcd.setCursor(0, 0);
}

void lcdDisplayTempHumid(float t, float h){ // Displays atmospheric conditions to LCD
  // NOTE -> temp (t) max value is XXXXX.XX, humidity (h) max value is XX.XX
  // Alternatively, t max = XX.XXX... and h max = XX.XXX... will be correct but with rounding error
  lcd.clear();
  lcd.setCursor(0, 0); //Set invisible cursor to first character, first line (x = 0, y = 0)
  lcd.print("Temp: "); lcd.print(t); lcd.print("*F"); // Output "Temp: X*F" at cursor
  lcd.setCursor(0, 1); // Set invisible cursor to first character, second line (x = 0, y = -1)
  lcd.print("Humidity: "); lcd.print(h); lcd.print("%");
  delay(5000); // Display for time
}

void wipeLCD(){
  lcd.clear();
}

// WATER SENSOR FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// NOTE -> Do not provide continuous voltage to water sensor, will result in corrosion

// LED FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void LEDon(int num){
  switch (num){
    case BLU: // 0
      *port_c |= 0b00000001;
      break;
      
    case GRN: // 1
      *port_c |= 0b00000010;
      break;
      
    case YLW: // 2
      *port_c |= 0b00000100;
      break;
      
    case RED: // 3
      *port_c |= 0b00001000;
      break;
  }
}

void LEDoff(int num){
  switch (num){
    case BLU: // 0
      *port_c &= 0b11111110;
      break;
      
    case GRN: // 1
      *port_c &= 0b11111101;
      break;
      
    case YLW: // 2
      *port_c &= 0b11111011;
      break;
      
    case RED: // 3
      *port_c &= 0b11110111;
      break;
  }
}

void allLEDoff(void){
  *port_c &= 0b11110000;
}

// STEPPER MOTOR/POTENTIOMETER FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int updateVentPosition(void){ // change vent position according to potentiometer setting and return steps changed
  int potentValue;
  int desiredMotorPosition;
  int motorChange;

  if(state != DISABLED){ // if NOT in disabled state, change vent according to potentiometer and report step changes to vent
    //Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"); // For test purposes
    Serial.println("Obtaining potentiometer voltage... "); // For test purposes
    delay(3000); // allow time for setting to be selected
    potentValue = analogRead(A8);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - max V):
    float voltage = potentValue * (maxPotentVoltage / 1023.0);

    //convert potentiometer voltage to changes in motor (0V = 0, max V = stepsPerRevolution):
    if(voltage < 1){ // if voltage is less than 1.00V, full close vent
      desiredMotorPosition = 0;
    } else if(voltage > 4){ // if voltage greater than 4.00V, full open vent
      desiredMotorPosition = stepsPerRevolution;
    } else { // otherwise, non-preset angle of vent
      desiredMotorPosition = ((stepsPerRevolution / maxPotentVoltage) * voltage); // linear y = mx + b equation to achieve the 0V = 0 steps and max V = maxSteps behavior
    }
    motorChange = (desiredMotorPosition - motorPosition);
  
    // step motor to desired positon:
    myStepper.step(motorChange);
    motorPosition = motorPosition + motorChange;
    delay(1000); // To prevent motor stress if position is changing rapidly
    return motorChange;
  } else {
    return 0; // if in disabled state, do not read potentiometer and report 0 steps of vent change
  }
  
}

int getVentPosition(void){ // report stepper motor position
  return motorPosition;
}

void reportVentChange(int change){
    Serial.print("Vent changed by ");
    Serial.print(change);
    Serial.println(" steps.");
}
