void diceRoll() {

  //newMilli = newMilli  millis();
  /*Design a rolling LED pattern */
  for (byte i = 0; i < 7; i ++) {
    leds = 1 << i;
    updateShiftRegister();
    delay(40);
  }

  for (byte i = 6; i > 0; i --) {
    leds = 1 << i;
    updateShiftRegister();
    delay(40);
  }

  //random function is too large. use millis reading to simulate memory-cheaper way.
  //byte rollValue = random(1, 7);

 

 byte rollValue = millis() % 11;
  
//use 12 values instead of 6 because millis()%6 oddly didn't work
//and if statements were cheaper on memory than switch cases after test

    if (rollValue == 1 | rollValue == 2) {
    leds = side1;
    }
    else if (rollValue == 3 | rollValue == 4) {
    leds = side2;
    }
    else if (rollValue == 5 | rollValue == 6) {
    leds = side3;
    }
    else if (rollValue == 7 | rollValue == 8) {
    leds = side4;
    }
    else if (rollValue == 9 | rollValue == 10) {
    leds = side5;
    }
    else if (rollValue == 11| rollValue == 0) {
    leds = side6;
    }

 

  updateShiftRegister();

}
