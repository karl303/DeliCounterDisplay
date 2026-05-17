// karl303
// DeliCounterDisplay_32x64
// 
// Targets Arduino Mega with a 32x64 RGB LED matrix panel
// Implements a "Now serving number:" message, followed by
// a number. The number can be incremented by pressing a
// button attached to digital input #2. The number can be
// reset or set by sending a digit through the serial
// interface

// Uses the Adafruit RGBmatrixPanel library
#include <RGBmatrixPanel.h>

#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

const int buttonPin = 2;  // the number of the pushbutton pin
const int ledPin = 13;    // the number of the LED pin

int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

unsigned long debounceDelay = 500;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);

  matrix.begin();

  int w = 0;

    // fill the screen with 'black'
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // draw some text!
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves

  matrix.setCursor(0, 0);    // start at top left, with 8 pixel of spacing
  char *myStr =  "Now ";
  char *myStr1 = "serving";
  char *myStr2 = "number:";

  for (w=0; w<4; w++) {
    matrix.setTextColor(Wheel(w));
    matrix.print(myStr[w]);
  }

  matrix.setCursor(22,0);

  for (w=4; w<11; w++) {
    matrix.setTextColor(Wheel(w));
    matrix.print(myStr1[w-4]);
  }

  matrix.setCursor(12, 8);    // next line
  
  for (w=0; w<7; w++) {
    matrix.setTextColor(Wheel(w));
    matrix.print(myStr2[w]);
  }

  matrix.setCursor(28, 18);    // next line
  matrix.print('4');

}

char count = '2';
char lastChar = '4';
char nextChar = ' ';

int lastReading = 1;

void loop() {
  int updateDisplay = 0;

  if (Serial.available()) {
    int inByte = Serial.read();
    nextChar = (char)inByte;
    updateDisplay = 1;
    matrix.setCursor(28,18);
    matrix.setTextColor(matrix.Color444(0,0,0));
    matrix.print(lastChar);
    matrix.setCursor(28,18);
    matrix.setTextColor(Wheel(8));
    matrix.print((char)inByte);
    lastChar = inByte;
  }


  int reading = digitalRead(buttonPin);

  if(lastReading == HIGH && reading == LOW)
  {
    nextChar = lastChar + 1;
    updateDisplay = 1;
  }

  lastReading = reading;

  if(updateDisplay == 1)
  {
    matrix.setCursor(28,18);
    matrix.setTextColor(matrix.Color444(0,0,0));
    matrix.print(lastChar);
    matrix.setCursor(28,18);
    matrix.setTextColor(Wheel(8));
    matrix.print(nextChar);
    lastChar = nextChar;

    if(reading == LOW)
    {
      delay(debounceDelay);
    }
  }

}


// Input a value 0 to 24 to get a color value.
// The colours are a transition r - g - b - back to r.
uint16_t Wheel(byte WheelPos) {
  if(WheelPos < 8) {
   return matrix.Color333(7 - WheelPos, WheelPos, 0);
  } else if(WheelPos < 16) {
   WheelPos -= 8;
   return matrix.Color333(0, 7-WheelPos, WheelPos);
  } else {
   WheelPos -= 16;
   return matrix.Color333(WheelPos, 0, 7 - WheelPos);
  }
}
