/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98
This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)
Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!
Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
Modified by Dan Peirce B.Sc.
The code now works like a terminal.
* text is being sent from Serial Monitor
* Intent is to have it sent from another MCU
* This tested on Arduino Uno
* Intent is to use Adafruit 5V Pro Trinket
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>

#define OLED_RESET 4

void defaultState();
void shiftoutS();
void xposS();
void yposS();
void xposL1S();
void yposL1S();

Adafruit_SSD1306 display(OLED_RESET);

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {                
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for 3rd party 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // text display tests
  display.setRotation(0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,9);
  display.setFont(&FreeSerif9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("KPU");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,29);
  display.println("PHYS1600");
  display.setTextSize(1);
  display.println("  PMT");  
  display.display();
  delay(2000);
  display.clearDisplay();

    display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);

}

uint8_t incomingByte, count=1;

void (*statePnt)() = defaultState;

void loop() 
{
    statePnt(); // additional states will be added
}

void defaultState()
{
      // send data only when you receive data:
    if (Serial.available() > 0) 
    {
          // read the incoming byte:
          incomingByte = Serial.read();
          if ( incomingByte == 0x0C)   // using "FF" fast foreword ascii code
          {                            
            display.clearDisplay();
            display.display();
            display.setCursor(0,0);
          }
          else if ( incomingByte == 0x11)   // control device 1 -- size 1
          {                            
            display.setTextSize(1);
          }
          else if ( incomingByte == 0x12)   // control device 2 -- size 2
          {                            
            display.setTextSize(2);
          }
          else if ( incomingByte == 0x13)   //  device control 3 -- size 3
          {                            
            display.setTextSize(3);
          }
          else if ( incomingByte == 0x14)   // device control 4 -- size 4
          {                            
            display.setTextSize(4);
          }
          else if ( incomingByte == 0x09)   // Landscape Mode
          {                            
            display.setRotation(0);
          }
          else if ( incomingByte == 0x0B)   // Portrait Mode
          {                            
            display.setRotation(1);
          }
          else if ( incomingByte == 0x0E)   // Portrait Mode
          {                            
            statePnt = shiftoutS;
          }          
          else
          {
            display.write(incomingByte);
            if (incomingByte == '\n') display.display();
          }
    }

}

// Second level to menu added using SO (shcift out ASCII character)
// This state is entered (transition from default state) if a shift out is recevied.
// So far this state looks for a back tick to indicate set postion command
// It is likely that additional controls will be added
// if the command is not recognized then SO is cancelled and control goes back to 
// default state.
void shiftoutS()
{
    if (Serial.available() > 0) 
    {
          // read the incoming byte:
          incomingByte = Serial.read();
          if ( incomingByte == '`')   // using back tick for set position
          {                            
            statePnt = xposS;
          }
      if ( incomingByte == '-')
      {
        statePnt = xposL1S;  // using hyphen for draw line
      }
          else 
          {                            
            statePnt = defaultState;
          }
    }
}


unsigned char xposV=0, xposV2;
unsigned char yposV=0, yposV2;

// xposL1S is entered if draw line was chosen after SO
// If the next byte received is 32 dec or more 32 dec will be subtracted
// the new value is taken as the x position

void xposL1S()
{
    if (Serial.available() > 0) 
    {
          // read the incoming byte:
          incomingByte = Serial.read();
          if ( incomingByte >= 0x20)   // 
          {
            xposV = incomingByte - 0x20;                            
          }
          else 
          {                            
            xposV = incomingByte;
          }
          statePnt = yposL1S;
    }  
}

// yposL1S is entered if set position was chosen after SO
// and an xposV has already been entered
// If the next byte received is 32 dec or more 32 dec will be subtracted
// the new value is taken as the y position
// once both positions are received the curser position will be updated

void yposL1S()
{
    if (Serial.available() > 0) 
    {
          // read the incoming byte:
          incomingByte = Serial.read();
          if ( incomingByte >= 0x20)   //
          {
            yposV = incomingByte - 0x20;                            
          }
          else 
          {                            
            yposV = incomingByte;
          }
          statePnt = xposL2S;
    }  
}

// If the next byte received is 32 dec or more 32 dec will be subtracted
// the new value is taken as the x position

void xposL2S()
{
    if (Serial.available() > 0) 
    {
          // read the incoming byte:
          incomingByte = Serial.read();
          if ( incomingByte >= 0x20)   // 
          {
            xposV2 = incomingByte - 0x20;                            
          }
          else 
          {                            
            xposV2 = incomingByte;
          }
          statePnt = yposL2S;
    }  
}

// yposL2S is entered if set position was chosen after SO
// and an xposV has already been entered
// If the next byte received is 32 dec or more 32 dec will be subtracted
// the new value is taken as the y position
// once both positions are received the curser position will be updated

void yposL2S()
{
    if (Serial.available() > 0) 
    {
          // read the incoming byte:
          incomingByte = Serial.read();
          if ( incomingByte >= 0x20)   //
          {
            yposV2 = incomingByte - 0x20;                            
          }
          else 
          {                            
            yposV2 = incomingByte;
          }
      display.drawLine(xposV, yposV, xposV2, yposV2,WHITE);
      display.display();
          statePnt = defaultState;
    }  
}

// xposS is entered if set position was chosen after SO
// If the next byte received is 32 dec or more 32 dec will be subtracted
// the new value is taken as the x position

void xposS()
{
    if (Serial.available() > 0) 
    {
          // read the incoming byte:
          incomingByte = Serial.read();
          if ( incomingByte >= 0x20)   // using back tick for set position
          {
            xposV = incomingByte - 0x20;                            
          }
          else 
          {                            
            xposV = incomingByte;
          }
          statePnt = yposS;
    }  
}


// yposS is entered if set position was chosen after SO
// and an zposV has already been entered
// If the next byte received is 32 dec or more 32 dec will be subtracted
// the new value is taken as the y position
// once both positions are received the curser position will be updated

void yposS()
{
    if (Serial.available() > 0) 
    {
          // read the incoming byte:
          incomingByte = Serial.read();
          if ( incomingByte >= 0x20)   // using back tick for set position
          {
            yposV = incomingByte - 0x20;                            
          }
          else 
          {                            
            yposV = incomingByte;
          }
          display.setCursor(xposV,yposV);
          statePnt = defaultState;
    }  
}

