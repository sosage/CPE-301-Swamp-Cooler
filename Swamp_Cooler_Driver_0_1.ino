// LIBRARIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <LiquidCrystal.h> // Import LCD library

// GLOBAL VARIABLES & MACROS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* NOTE -> each state has an assigned integer for ease of programming.
 *  see below table for guide.
 *  #define DISABLED 0
 *  #define IDLING 1
 *  #define ERRORSTATE 2
 *  #define RUNNING 3 
 */
unsigned int state;
float temp;
float humid;

// FUNCTION INITIALIZATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void lcdDisplayError(void);
void lcdDisplayTempHumid(float, float);

// PIN INITIALIZATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LiquidCrystal lcd(52, 53, 48, 49, 46, 47);
  /*  ^^^ establishing pins for use by LCD in the following order:
   *  lcd(RS, E, DB4, DB5, DB6, DB7) - 4 data lines 
   *  for other syntaxes, see documentation for library:
   *  https://www.arduino.cc/reference/en/libraries/liquidcrystal/
   */
// volatile unsigned char *portDDRB  = (unsigned char *) 0x24; // Reference syntax for GPIO
// volatile unsigned char *portB     = (unsigned char *) 0x25; // Reference syntax for GPIO

// SETUP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  Serial.begin(9600);
  Serial.println("Setup Block"); // For test purposes
  delay(1000); // Delay to allow time for serial monitor to open
  state = 1; // Set initial state
  
  // LCD
  lcd.begin(16, 2); //Tell the LCD that it is a 16x2 LCD
  temp = 72.5911;
  humid = 25.8744;
}

// MAIN LOOP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"); // For test purposes
  Serial.println("Main Loop"); // For test purposes
  bool state_test = true; // For test purposes
  if(state_test == 1){ // For test purposes
    for(int i = 0; i <= 3; i++){ // For test purposes
      state = i;

  switch (state) {
  /* NOTE ->  FOR ALL STATES, realtime clock must be used to report via serial port the
  * time of each state transition and any changes to the stepper motor
  * position for the vent. FOR ALL STATES EXCEPT FOR DISABLED, temp and humid
  * is continuously monitored and reported to LCD once per minute, system responds to
  * changes in vent position control, and the stop button should turn fan motor off (if on) and
  * system should go to DISABLED state.*/

  case 0: // 0
    Serial.println("DISABLED"); // For test purposes
    // report state changes
    // report stepper motor changes
    break;

  case 1: // 1
    Serial.println("IDLING"); // For test purposes
    // report state changes
    // stepper motor functioning
    // report stepper motor changes
    // monitor temp and humidity
    // report temp and humidity to LCD once per minute
    // stop button turns off fan (if on) and change to DISABLED
    lcdDisplayTempHumid(temp, humid);
    break;

  case 2: // 2
    Serial.println("ERRORSTATE"); // For test purposes
    // report state changes
    // stepper motor functioning
    // report stepper motor changes
    // monitor temp and humidity
    // report temp and humidity to LCD once per minute
    // stop button turns off fan (if on) and change to DISABLED
    lcdDisplayError();
    lcdDisplayTempHumid(temp, humid);
    break;

  case 3: // 3
    Serial.println("RUNNING"); // For test purposes
    // report state changes
    // stepper motor functioning
    // report stepper motor changes
    // blue LED on, all other LEDs off
    // fan motor on
    // monitor temp and humidity
    // report temp and humidity to LCD once per minute
    // stop button turns off fan (if on) and change to DISABLED
    // change to IDLE if temp drops below threshold
    // change to ERROR if water becomes too low
    lcdDisplayTempHumid(temp, humid);
    break;
 
  } // End switch-case block
  
    Serial.print("Switching to state "); // For test purposes
    Serial.println(state + 1);
    } // End for-loop block
  } // End if statement

}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// LCD FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// NOTE -> 16 Characters per line
void lcdDisplayError(){ // Displays low water error message to LCD
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
  lcd.clear();
  
  lcd.setCursor(0, 0);
}

void lcdDisplayTempHumid(float t, float h){ // Displays atmospheric conditions to LCD
  // NOTE -> temp (t) max value is XXXXX.XX, humidity (h) max value is XX.XX
  // Alternatively, t max = XX.XXX... and h max = XX.XXX... will be correct but with rounding error
  lcd.setCursor(0, 0); //Set invisible cursor to first character, first line (x = 0, y = 0)
  lcd.print("Temp: "); lcd.print(t); lcd.print("*F"); // Output "Temp: X*F" at cursor
  lcd.setCursor(0, 1); // Set invisible cursor to first character, second line (x = 0, y = -1)
  lcd.print("Humidity: "); lcd.print(h); lcd.print("%");
  delay(5000); // Display for time
  lcd.clear(); // Reset screen for a moment to indicate refresh
  delay(1000);
}

// WATER SENSOR FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// NOTE -> Do not provide continuous voltage to water sensor, will result in corrosion
