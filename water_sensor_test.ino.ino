#define RDA 0x80
#define TBE 0x20

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0; // USART 0A Control and Status Register
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1; // USART 0B Control and Status Register
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2; // USART 0C Control and Status Register
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4; // USART 0 Baud Rate Register Low Byte
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6; // USART I/O Data Register
 
volatile unsigned char *my_ADMUX    = (unsigned char*) 0x7C; // ADC Multiplexer Selection Register
volatile unsigned char *my_ADCSRB   = (unsigned char*) 0x7B; // ADC Control and Status Register B
volatile unsigned char *my_ADCSRA   = (unsigned char*) 0x7A; // ADC Control and Status Register A
volatile unsigned int  *my_ADC_DATA = (unsigned int*)  0x78; // ADC Data Register

volatile unsigned char* port_a = (unsigned char*) 0x22; // Port A Data Register
volatile unsigned char* ddr_a  = (unsigned char*) 0x21; // Port A Direction Register

void setup() 
{
  // setup the UART
  U0init(9600);
  // setup the ADC
  adc_init();
  // ouput
  *ddr_a |= 0b00000010; //pin D23 set to output
}

void loop() 
{
  Serial.println(checkWtr());
  delay(1000);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

float checkWtr(void){
  *port_a |= 0b00000010; //apply voltage to sensor, pin D23 set HIGH
  delay(500); //allow voltage to stabilise
  unsigned int adc_reading = adc_read(0); //get the reading from the ADC, analog input pin A0
  *port_a &= 0b11111101; //cut off voltage to sensor pin D23 set LOW
  float wtrVoltage = adc_reading * (5.0 / 1023.0); //convert 0-1023 analog signal to voltage
  return wtrVoltage;
}

void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11011111; // clear bit 5 to 0 to set prescaler selection to slow reading
  
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111;// clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111;// clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11011111;// clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}

unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}

void print_int(unsigned int out_num)
{
  // clear a flag (for printing 0's in the middle of numbers)
  unsigned char print_flag = 0;
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
  // print a newline
  U0putchar('\n');
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

/* Not needed?
unsigned char U0kbhit()
{
  return *myUCSR0A & RDA;
}

unsigned char U0getchar()
{
  return *myUDR0;
}*/
