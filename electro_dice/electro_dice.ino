/*Functions
  The dice is in sleep mode, but never turned off
  A tact switch, which is a vib sensor, triggers the interrupt
  dice is rolling
  Results shown, interrupts are off, and a WDT is on for 8 seconds (call the wdt_delay fct)
  SHOULD CHANGE PRESCALER TO 4
*/

/**************************************************************************************************************************************************************/
////PINS
/**************************************************************************************************************************************************************/


/**************************************************************************************************************************************************************/
////CONSTANTS & VARIABLES
/**************************************************************************************************************************************************************/

//74hc595 variables
byte leds = 0;
/*DICE SIDES*/
#define side1 1
#define side2 0B11000
#define side3 0B11001
#define side4 0B1011010
#define side5 0B1011011
#define side6 0B1111110


byte rollValue;

/**************************************************************************************************************************************************************/
////LIBRARIES
/**************************************************************************************************************************************************************/

#include <avr/sleep.h>
#include <avr/interrupt.h>


/**************************************************************************************************************************************************************/
////SETUP
/**************************************************************************************************************************************************************/

void setup() {

  //sleep setup
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

}

/**************************************************************************************************************************************************************/
////LOOP
/**************************************************************************************************************************************************************/

void loop() {
  //go to sleep until dice is shaken
  //Pin Change interrupt setup
  //Enable interrupts
  GIMSK |= (1 << PCIE);
  PCMSK |= _BV(PCINT2);
  sei();

  ADCSRA &= ~_BV(ADEN);      //Turn ADC off, saves ~230uA
  sleep_cpu();
  ADCSRA |= _BV(ADEN);        //will ADC need to be turned on after sleep?

  //Disable pinchange interrupts
  GIMSK &= ~(1 << PCIE);
  PCMSK &= ~(1 << PCINT2);

  diceRoll();

  wdt_delay();
}

/**************************************************************************************************************************************************************/
////FUNCTIONS
/**************************************************************************************************************************************************************/

void wdt_delay() {
  //This order of commands is important and cannot be combined
  cli();
  MCUSR &= ~(1 << WDRF); //Clear the watch dog reset
  WDTCR |= (1 << WDCE) | (1 << WDE); //Set WD_change enable, set WD enable
  WDTCR = 0B100001; //Set prescaler to 8 sec (see p.46 of datasheet to change prescaler), and OVERWRITE WDTCR value ( dont just use |= )
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
  sei();

  ADCSRA &= ~_BV(ADEN);      //Turn ADC off, saves ~230uA
  sleep_cpu();
  ADCSRA |= _BV(ADEN);        //will ADC need to be turned on after sleep?
  
  WDTCR &= ~(1 << WDCE) & ~(1 << WDE); //disable wdt: Set WD_change enable and WD enable back to 0
}


ISR(PCINT0_vect) { //wake up when the dice is moved!
}


ISR(WDT_vect) {  //no delay or whatsoever because we just wake up after a sleep delay
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}
