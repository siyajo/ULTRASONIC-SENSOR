#include<reg51.h>
#include<intrins.h>

#define lcd_data P2 //LCD data pins connected to port 2

sfr16 DPTR=0x82;

sbit trig=P3^2; //Trigger pin of ultrasonc sensor connceted to P3.5
sbit echo=P3^3; //Echo pin of ultrasonc sensor connceted to P3.2

unsigned int range=0;

sbit rs=P0^0; //Register select
sbit rw=P0^1; //Read/Write
sbit en=P0^2; //Enable-to latch the command/data to LCD

void lcd_init();
void cmd(unsigned char a);
void dat(unsigned char b);
void show(unsigned char *s);
void lcd_delay();

void lcd_init() //Initializes 16x2 LCD
{
  cmd(0x38); // Configure LCD in 8-bit mode
  cmd(0x0e); // Display ON, cursor ON
  cmd(0x06); // Auto increment cursor
  cmd(0x0c); // Display ON, cursor OFF
  cmd(0x80); // Move cursor to the beginning
}

void cmd(unsigned char a)//Sends command to LCD
{
  lcd_data=a;
  rs=0;  // Command mode
  rw=0;  // Write mode
  en=1;
  lcd_delay();
  en=0;
}

void dat(unsigned char b)//Sends data to LCD
{
  lcd_data=b;
  rs=1; // Data mode
  rw=0; // Write mode
  en=1;
  lcd_delay();
  en=0;
}

void show(unsigned char *s)//Prints a string on LCD
{
  while(*s) {
    dat(*s++);
  }
}

void lcd_delay()
{
  unsigned int i;
  for(i=0;i<=1000;i++);
}


void send_pulse(void) //Creates small delay for LCD operations
{
  TH0=0x00;TL0=0x00; // Reset Timer 0
  trig=1; // Send a short HIGH pulse
  nop();nop();nop();nop();nop(); // Small delay using proper intrinsic
  nop();nop();nop();nop();nop();
  trig=0; // End pulse
}

unsigned char ultrasonic()
{
  unsigned char get;
  send_pulse(); // Send trigger pulse
  while(!echo); // Wait for echo signal to start
  while(echo); // Wait for echo signal to end
  DPH=TH0;
  DPL=TL0;
  TH0=TL0=0xff;
  if(DPTR<38000)
    get=DPTR/59; // Convert time to distance (cm)
  else
    get=0;
  return get;
}


void main()
{
  TMOD=0x09; // Timer 0 in 16-bit counter mode (Mode1)
  TH0=TL0=0;
  TR0=1; // Start Timer 0
  lcd_init(); // Initialize LCD
  show("DISTANCE"); // Display "DIS" on LCD
  P3|=(1<<2); // Set P3.2 (Echo pin) as input
  while(1) {
    cmd(0x84); // Move cursor to position 4
    range=ultrasonic(); // Get distance
    // Convert range (integer distance) into individual digits and display on LCD
    dat((range/100)+48);
    dat(((range/10)%10)+48);
    dat((range%10)+48);
  }
}
