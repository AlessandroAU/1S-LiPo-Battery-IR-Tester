

//Control loop used to PWM a fet with a physical low pass to meet the setpoint current.

#include "include/ina219.h"
#include "Filter.h"


#define outPin PIN_B3 //PWM output pin
byte PWMvalue = 0;
float targetCurr = 0;
float measCurr = 0; //measured current
float measCurrFlt = 0; //measured current after filtering


void HandleControlLoop() {
  if (targetCurr > 0) {
    measCurr = ina219.read_current();
    if (measCurr < targetCurr) {
      PWMvalue++;
    } else {
      PWMvalue--;
    }
    analogWrite(outPin, PWMvalue);
  } else {
    PWMvalue = 0;
    analogWrite(outPin, PWMvalue);
  }
}
