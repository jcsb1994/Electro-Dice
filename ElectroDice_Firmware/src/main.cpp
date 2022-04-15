//***********************************************************************************
// Copyright 2022 JCSB1994
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//***********************************************************************************
//
// Description:
//    This project operates an electronic dice that produces random numbers when shaken.
//    LEDs light up to show the dice roll.
//
//    This Project demonstrates the use of C language synthax and direct register manipulation
//    on the ATtiny13A microcontroller.
//
//***********************************************************************************

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

/**************************************************************************************************************************************************************/
////CONSTANTS & VARIABLES
/**************************************************************************************************************************************************************/

#define SHIFT_REG_DATA_PIN  (3)
#define SHIFT_REG_LATCH_PIN (0)
#define SHIFT_REG_CLOCK_PIN (1)

#define VIBRATION_SWITCH_PIN (2)

#define DICE_MAX_SIDE_NB    (6)

uint8_t diceSides[DICE_MAX_SIDE_NB] = {
  0B1000000,
  0B0001100,
  0B1001100,
  0B0101101,
  0B1101101,
  0B0111111
};

ISR(PCINT0_vect)
{
}

void updateShiftRegister(uint8_t regValue)
{
  PORTB &= ~(1 << SHIFT_REG_LATCH_PIN);

  // Shift out: bit bang the register value from data pin with a clock cycle on each bit
  for (uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(SHIFT_REG_DATA_PIN, !!(regValue & (1 << i)));
    // Bit bang a clock cycle
    PORTB |= (1 << SHIFT_REG_CLOCK_PIN);
    PORTB &= ~(1 << SHIFT_REG_CLOCK_PIN);
  }
  PORTB |= (1 << SHIFT_REG_LATCH_PIN);
}


void diceRoll()
{
  uint8_t registerValue = 0;
  uint8_t i = 0;
  
  for (; i <= DICE_MAX_SIDE_NB; i++)
  {
    registerValue = 1 << i;
    updateShiftRegister(registerValue);
    delay(8);
  }

  for (; i != 0; i--)
  {
    registerValue = 1 << i;
    updateShiftRegister(registerValue);
    delay(10);
  }

  // int rollValue = analogRead(RANDOM_SEED_ANALOG_INPUT);

  int rollValue = rand() % DICE_MAX_SIDE_NB;

  registerValue = diceSides[rollValue];

  updateShiftRegister(registerValue); 
  delay(200);
}

/**************************************************************************************************************************************************************/
////INITIALIZATION
/**************************************************************************************************************************************************************/


void setup()
{
  // Set Shift register GPIOs as outputs
  DDRB |= 1 << SHIFT_REG_CLOCK_PIN;
  DDRB |= 1 << SHIFT_REG_LATCH_PIN;
  DDRB |= 1 << SHIFT_REG_DATA_PIN;

  // Set vibration switch GPIO as input with pullup enabled
  DDRB &= ~(1 << VIBRATION_SWITCH_PIN);
  PORTB |= (1 << VIBRATION_SWITCH_PIN);

  // Enable interrupts, enable input state change interrupt on GPIO 2
  GIMSK |= (1 << PCIE);
  PCMSK |= _BV(PCINT2);
  sei();

  // sleep setup
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

/**************************************************************************************************************************************************************/
////LOOP
/**************************************************************************************************************************************************************/

void loop()
{
  // Set outputs to low
  PORTB &= ~(1 << SHIFT_REG_CLOCK_PIN);
  PORTB &= ~(1 << SHIFT_REG_LATCH_PIN);
  PORTB &= ~(1 << SHIFT_REG_DATA_PIN);

  ADCSRA &= ~_BV(ADEN); // Turn ADC off, saves ~230uA
  sleep_cpu();
  ADCSRA |= _BV(ADEN); // Turn ADC back on


  diceRoll();

  updateShiftRegister(0);

  sleep_cpu();

}
