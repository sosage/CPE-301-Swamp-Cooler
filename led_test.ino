volatile unsigned char* port_c = (unsigned char*) 0x28; // LED Port
volatile unsigned char* ddr_c = (unsigned char*) 0x27;

// LED # Code
// NOTE - > all LEDs on Port C 0 thru 3 (pins D34 - 37)
#define RED 3
#define YLW 2
#define GRN 1
#define BLU 0

void setup() {
  Serial.begin(9600);
  *ddr_c |= 0b00001111; // LED pins set to output
}

void loop() {
  LEDon(RED);
  delay(1000);
  LEDon(YLW);
  delay(1000);
  LEDon(GRN);
  delay(1000);
  LEDon(BLU);
  delay(1000);

  LEDoff(RED);
  delay(1000);
  LEDoff(YLW);
  delay(1000);
  LEDoff(GRN);
  delay(1000);
  LEDoff(BLU);
  delay(1000);
  
}

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
