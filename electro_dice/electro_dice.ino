/*Functions
  The dice is in sleep mode, but never turned off
  A tact switch, which is a vib sensor, triggers the interrupt
  dice is rolling
  Results shown, interrupts are off, and a WDT is on for 8 seconds (call the wdt_delay fct)
  SHOULD CHANGE PRESCALER TO 4
*/

/**************************************************************************************************************************************************************/
////LIBRARIES
/**************************************************************************************************************************************************************/

#include <avr/sleep.h>
#include <avr/interrupt.h>



/**************************************************************************************************************************************************************/
////PINS
/**************************************************************************************************************************************************************/

#define dataPin 3
#define latchPin 0
#define clockPin 1

#define switchPin 2

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



/**************************************************************************************************************************************************************/
////SETUP
/**************************************************************************************************************************************************************/

void setup() {


  DDRB |= 1 << clockPin;
  DDRB |= 1 << latchPin;
  DDRB |= 1 << dataPin;

  DDRB &= ~(1 << switchPin);
  PORTB |= (1 << switchPin);

  //Enable interrupts
  GIMSK |= (1 << PCIE);
  PCMSK |= _BV(PCINT2);
  sei();

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
  //ATtiny45-85: Enable pinchange interrupts
  /*  GIMSK |= (1 << PCIE);
    PCMSK |= _BV(PCINT2);
    sei();   */



  ADCSRA &= ~_BV(ADEN);      //Turn ADC off, saves ~230uA
  
  sleep_cpu();
  ADCSRA |= _BV(ADEN);        //will ADC need to be turned on after sleep?

  PORTB &= ~(1 << clockPin);
  PORTB &= ~(1 << latchPin);
  PORTB &= ~(1 << dataPin);

  //dice awaken, disable interrupts until dice is rolled

  //Disable pinchange interrupts
  // GIMSK &= ~(1 << PCIE);
  //PCMSK &= ~(1 << PCINT2);

  diceRoll();
  setup_watchdog(8);
  ADCSRA &= ~_BV(ADEN);      //Turn ADC off, saves ~230uA
  sleep_cpu();
  ADCSRA |= _BV(ADEN);        //will ADC need to be turned on after sleep?
/*  WDTCR &= ~(1 << WDCE);
  WDTCR &= ~(1 << WDE); //disable wdt: Set WD_change enable and WD enable back to 0
  MCUSR |= (1 << WDRF); //Clear the watch dog reset   */
  WDTCR &= ~(1 << WDTIE);

  //wait while results are shown and then turn leds off. (loop restarts and goes to sleep)
  //wdt_delay();  //only works with attiny
  //delay(1000); //debug only
  leds = 0;
  updateShiftRegister();


  /*//DEBUG----------------------------------
    leds = 0B11111111;
    updateShiftRegister();
    delay(1000);
    leds = 0;
    updateShiftRegister();
    delay(1000);
    //------------------------------------*/
}

/**************************************************************************************************************************************************************/
////FUNCTIONS
/**************************************************************************************************************************************************************/

/*
  void wdt_delay() {


  //ATtiny45-85 wdt activation

  cli();  //disable interrupts also done with __disable_interrupt();
  MCUSR &= ~(1 << WDRF); //Clear the watch dog reset  //watchdog reset also done with __watchdog_reset();
  //start timed sequence //This order of commands is important and cannot be combined
  WDTCR |= (1 << WDCE) | (1 << WDE); //Set WD_change enable, set WD enable
  WDTCR |= (1 << WDP3); //Set prescaler to 4 sec (see p.41 of attiny13 datasheet to change prescaler), and OVERWRITE WDTCR value ( dont just use |= )
  WDTCR |= _BV(WDTIE); //(WDIE for attiny45-85) Set the interrupt enable, this will keep unit from resetting after each int
  sei();  //enable interrupts also done with __enable_interrupt();


  ADCSRA &= ~_BV(ADEN);      //Turn ADC off, saves ~230uA
  sleep_cpu();
  ADCSRA |= _BV(ADEN);        //will ADC need to be turned on after sleep?


  //ATtiny45-85 wdt disabling
  //WDTCR &= ~(1 << WDCE) & ~(1 << WDE); //disable wdt: Set WD_change enable and WD enable back to 0
  }*/




ISR(PCINT0_vect) { //wake up when the dice is moved!
}




ISR(WDT_vect) {  //no delay or whatsoever because we just wake up after a sleep delay
}





void updateShiftRegister()
{
  PORTB &= ~(1 << latchPin);

  //shiftOut(dataPin, clockPin, LSBFIRST, leds); //too ehavy, need my own version of shiftout
  for (byte i = 0; i < 8; i++)  {

    digitalWrite(dataPin, !!(leds & (1 << i)));
    PORTB |= (1 << clockPin);
    PORTB &= ~(1 << clockPin);

  }
  PORTB |= (1 << latchPin);
}





void setup_watchdog(int timerPrescaler) {

  if (timerPrescaler > 9 ) timerPrescaler = 9; //Correct incoming amount if need be

  byte bb = timerPrescaler & 7;
  if (timerPrescaler > 7) bb |= (1 << 5); //Set the special 5th bit if necessary

  //This order of commands is important and cannot be combined
  MCUSR &= ~(1 << WDRF); //Clear the watch dog reset
  WDTCR |= (1 << WDCE) | (1 << WDE); //Set WD_change enable, set WD enable
  WDTCR = bb; //Set new watchdog timeout value
  WDTCR |= _BV(WDTIE); //Set the interrupt enable, this will keep unit from resetting after each int
}


/*
  void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
  {
    uint8_t i;

    for (i = 0; i < 8; i++)  {
      if (bitOrder == LSBFIRST)
        digitalWrite(dataPin, !!(val & (1 << i)));
      else
        digitalWrite(dataPin, !!(val & (1 << (7 - i))));

      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }
  }*/
