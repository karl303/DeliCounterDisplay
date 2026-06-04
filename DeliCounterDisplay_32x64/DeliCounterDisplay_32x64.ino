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

int count = 10;
int lastCount = count;

int lastReading = 1;

unsigned long debounceDelay = 350;    // the debounce time; increase if the output flickers

// Copying serial-handler from "Serial Input Basics - updated"
// https://forum.arduino.cc/t/serial-input-basics-updated/382007

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

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
  displayDigits(count);
}

void loop() {
  int updateDisplay = 0;

  recvWithEndMarker();

  if (newData == true) {
        updateDisplay = 1;
        newData = false;
  }

  int reading = digitalRead(buttonPin);

  if(lastReading == HIGH && reading == LOW)
  {
    count = count + 1;
    updateDisplay = 1;
  }

  lastReading = reading;

  if(updateDisplay == 1)
  {
    // Set text black and write last count to clear display
    matrix.setTextColor(matrix.Color444(0,0,0));
    displayDigits(lastCount);
    // Set text green and write new count
    matrix.setTextColor(Wheel(8));
    displayDigits(count);

    lastCount = count;

    if(reading == LOW)
    {
      delay(debounceDelay);
    }
  }
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            count = 0;
            for(int i = 0; i < ndx; i++)
            {
              count = (count * 10) + (int)(receivedChars[i] - '0');
            }
            ndx = 0;
            newData = true;
        }
    }
}

int displayDigits(int number)
{
  int digits = 0;
  int k = 0;
  int temp = 0;
  
  if(number < 10)
  {
    //Serial.println("number < 10");
    digits = 1;
  }
  else if(number < 100)
  {
    //Serial.println("number < 100");
    digits = 2;
  }
  else if(number < 1000)
  {
    //Serial.println("number < 1000");
    digits = 3;
  }
  else if(number < 10000)
  {
    digits = 4;
  }
  else
  {
    return -1;
  }

  matrix.setCursor(28 - 3*(digits-1),18);

  for(k = 0; k < digits; k++)
  {
    temp = number;
    for(int n = 1; n < (digits - k); n++)
    {
      temp = temp / 10;
    }
    temp = temp % 10;

    matrix.print((char)('0' + temp));
  }
  return number;
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
