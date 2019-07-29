
#include "include/Tiny4kOLED.h"
#include "include/ina219.h"

uint32_t WaitHighCurrSettleDelay = 1000; //wait this long for current to settle
uint32_t WaitLowCurrSettleDelay = 1500; //wait this long for current to settle
uint32_t WaitHighCurrSettleStartTime; //wait this long for current to settle
uint32_t WaitLowCurrSettleStartTime; //wait this long for current to settle


extern float targetCurr;
extern float measCurr;
extern float measCurrFlt;

float MeasuredInitalVoltage = 0; //voltage under no load
float MeasuredLowCurrVoltage = 0; //voltage under low current draw
float MeasuredHighCurrCurrent = 0;
float MeasuredLowCurrCurrent = 0;
float MeasuredHighCurrVoltage = 0;
float CalculatedIRmohms; //calculated IR in milliohms

byte state = 0;
float LowCurr = 0.5;
float HighCurr = 2;


float CalcSlope(float y2, float y1, float x2, float x1) {
  return (y2 - y1) / (x2 - x1);
}

void setup() {

  ina219.begin();
  MeasuredInitalVoltage = ina219.read_bus_voltage();

  // Send the initialization sequence to the oled. This leaves the display turned off
  oled.begin();
  oled.setFont(FONT8X16);
  // Clear the memory before turning on the display
  oled.clear();
  // Turn on the display
  oled.on();
  // Switch the half of RAM that we are writing to, to be the half that is non currently displayed
  oled.switchRenderFrame();
  ina219.begin();
  pinMode(PIN_B3, OUTPUT);

  oled.setCursor(0, 1);
  oled.print(F("V: "));
  oled.print(MeasuredInitalVoltage);

}

void loop() {

  switch (state) {
    case 0:
      break;
    case 1: //set the low current and record when we started
      targetCurr = LowCurr;
      WaitLowCurrSettleStartTime = millis();
      state ++;
      break;
    case 2: // we wait and allow the current to settle to the 'low' value
      if (millis() > (WaitLowCurrSettleStartTime + WaitLowCurrSettleDelay)) {
        MeasuredLowCurrVoltage = ina219.read_bus_voltage();
        HandleControlLoop(); // for good measure
        MeasuredLowCurrCurrent = measCurr;
        state ++;
      }
      break;
    case 3:
      targetCurr = HighCurr;
      WaitHighCurrSettleStartTime = millis();
      state ++;
      //draw();
      break;
    case 4:
      if (millis() > (WaitHighCurrSettleStartTime + WaitHighCurrSettleDelay)) {
        MeasuredHighCurrVoltage = ina219.read_bus_voltage();
        HandleControlLoop(); // for good measure
        MeasuredHighCurrCurrent = measCurr;
        state ++;
      }
      break;
    case 5:
      targetCurr = 0;
      HandleControlLoop(); //shut off pwm
      CalculatedIRmohms = CalcSlope(MeasuredHighCurrVoltage, MeasuredLowCurrVoltage, MeasuredHighCurrCurrent, MeasuredLowCurrCurrent) * 1000.0;
      state ++;
      break;
    case 6:
      updateDisplay();
      delay(100);
      break;
    default:
      break;
  }

  //updateDisplay();
  //delay(50);
  HandleControlLoop();
}

void updateDisplay() {
  // Clear the half of memory not currently being displayed.
  oled.clear();
  // Position the text cursor
  // In order to keep the library size small, text can only be positioned
  // with the top of the font aligned with one of the four 8 bit high RAM pages.
  // The Y value therefore can only have the value 0, 1, 2, or 3.
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  oled.setCursor(0, 1);
  // Write text to oled RAM (which is not currently being displayed).
  oled.print(F("ms: "));
  // Write the number of milliseconds since power on.
  oled.print(millis());
  // Swap which half of RAM is being written to, and which half is being displayed.
  // This is equivalent to calling both switchRenderFrame and switchDisplayFrame.
  oled.switchFrame();
}
