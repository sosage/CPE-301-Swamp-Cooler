#include "RTClib.h"
#define RDA 0x80
#define TBE 0x20

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0; // USART 0A Control and Status Register
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1; // USART 0B Control and Status Register
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2; // USART 0C Control and Status Register
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4; // USART 0 Baud Rate Register Low Byte
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6; // USART I/O Data Register

unsigned long int initialMillis;
unsigned int currentMillis;
unsigned int currentCount;

RTC_DS1307 rtc;
//char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

void setup() {
  // put your setup code here, to run once:
  U0init(9600);

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  printTimeReport();
  
  Serial.print("Setting up. Beginning count at: ");
  Serial.print(startCount());
  Serial.println("ms.");
}

void loop() {
  // put your main code here, to run repeatedly:
  // output real date and time without Serial.print()
  
  // every 60 seconds output something to serial monitor. Following code tested to work up to at least 12 minutes with +-2s accuracy.
  currentCount = checkCount(); // check count
  if(currentCount >= 60000){
    printTimeReport(); // execute command
    Serial.print("Counted to ");
    Serial.print(currentCount);
    Serial.println("ms. Count resetting.");
    startCount(); // restart count
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printTimeReport(void){ // e.g. " at 2424 on 12/12/1212. \n"
  DateTime time = rtc.now();
  String timeNow = time.timestamp(DateTime::TIMESTAMP_TIME);
  String dateNow = time.timestamp(DateTime::TIMESTAMP_DATE);
  printString(" at ");
  printString(timeNow.c_str());
  printString(" on ");
  printString(dateNow.c_str());
  printString(". \n");
  
}

/*void printTimeReport(void){
  Serial.print(" at ");
  Serial.print(currentTime);
  Serial.print(" on ");
  Serial.println(currentDate);
}*/

void printString(const char* stringIn){
  const char* text = stringIn;
  for(int i = 0; text[i] != '\0'; i++){
    U0putchar(text[i]);
  }
}

void printInt(int out_num)
{
  // clear a flag (for printing 0's in the middle of numbers)
  unsigned char print_flag = 0;
  // if its less than 0
  if(out_num < 0)
  {
    // get the 1000's digit, add to '0' 
    U0putchar('-');
    // set the print flag
    print_flag = 1;
    // mod the out num by 1000
    out_num = abs(out_num);
  }
  // if its greater than 1000
  if(out_num >= 1000)
  {
    // get the 1000's digit, add to '0' 
    U0putchar(out_num / 1000 + '0');
    // set the print flag
    print_flag = 1;
    // mod the out num by 1000
    out_num = out_num % 1000;
  }
  // if its greater than 100 or we've already printed the 1000's
  if(out_num >= 100 || print_flag)
  {
    // get the 100's digit, add to '0'
    U0putchar(out_num / 100 + '0');
    // set the print flag
    print_flag = 1;
    // mod the output num by 100
    out_num = out_num % 100;
  } 
  // if its greater than 10, or we've already printed the 10's
  if(out_num >= 10 || print_flag)
  {
    U0putchar(out_num / 10 + '0');
    print_flag = 1;
    out_num = out_num % 10;
  } 
  // always print the last digit (in case it's 0)
  U0putchar(out_num + '0');
}

void U0init(int U0baud)
{
 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}

void U0putchar(unsigned char U0pdata)
{
  while((*myUCSR0A & TBE)==0);
  *myUDR0 = U0pdata;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int startCount(){ //take in a starting time in miliseconds and begin counting up
  initialMillis = millis(); // returns # of milliseconds the sketch has been running
  return initialMillis;
}

int checkCount(void){
  currentMillis = (millis() - initialMillis);
  return currentMillis;
}
