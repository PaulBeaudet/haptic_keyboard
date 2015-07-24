/********tenkey_Neotype.ino*********
* NOTE This is the main .ino sketch project
  |--> parent folder name should be "tenkey_Neotype" to recognize as such
* Compiled with Arduino.cc 1.6 IDE
* Copyright Paul Beaudet originally created fall 2014 <inof8or@gmail.com>
* Keyer that recieves and transmits text information from its user
* This program is free open source software: See licenses.txt to understand
  reuse rights of any file associated with this project
********* See readme.md for hardware discription **************/
#include <Wire.h>           // i2c
#include <EEPROM.h>         // store mouse calibration
#include "keyDefinitions.h" // define key numbers for various output types
#include "pin_definitions.h"// Pin arrangements per type of board

#define MONITOR_BUTTONS 33 // signal to monitor buttons
#define MONITOR_MODE   0   // goto default behavior for multi-mode functions
#define TRIGGER        1   // set enter key to press : enterBehavior()
#define NUMBERS_MODE   2   // outputFilter: Numbers
#define ADJUST_PWM     2   // potentiometer()
#define ADJUST_TIMING  3   // potentiometer()
#define AFRAMES        4   // frames in special char Animations

void setup(){          //setup the needed hardware
  pagersUp();          // pagers.ino: brings vibrating motor interface up
  buttonUp();          // buttons.ino: brings button polling intreface up
  serialInterfaceUp(); // yun/uno/leo.ino: brings serial output interface/s up
  setupSD();           // bring up file system for notes
  pinMode(COMMAND_PIN, INPUT_PULLUP); // setup pin for command mode =>PASH only
}

void loop(){
  mainIOroutine();               // handles key input and output
  if(noUserMessages()){                // as long as something else isn't playing
    feedbackAndRelease();        // controls pager feedback and release
  }
  potentiometer(MONITOR_MODE);   // monitor potentiometer: pagers.ino
  PASH(MONITOR_MODE);            // check for sd PASH OUTPUT
}

//---- main chord interpertation flow control ----
void mainIOroutine(){
  byte pressState = chordLoop(); // captures the current state of the buttons
  if(pressState){                // given a press event
    if (pressState < 128){       // narrows values to letters
      keyOut(pressState);        // actuate the press as a keystroke
      PASH(pressState);          // give PASH input if active
    } else{macros(pressState);}  // in all other cases check for macros
  }
}

byte chordLoop(){
  static byte doubleActuation = 0;

  byte actuation = 0;            // establish var that will be returned
  if(doubleActuation){           // backspace needed to be sent last iteration
    actuation = doubleActuation; // double actuation holds the intended char
    doubleActuation = 0;
    return actuation;
  }

  buttonUpdate();                      // updates internal button state
  int chord = trueChord(MONITOR_MODE); // monitor chord posibilities
  byte pressState = patternToChar(chord);
  byte hold = 0;
  if(doubleActuation = doubleToASCII(doubleEvent(pressState))){
    actuation = BACKSPACE;
    hold = doubleActuation;
  } else if(pressState){
    actuation = pressState;
    hold = pressState;
  }
  byte varifiedHold = 0;
  hold = holdHandlr(hold);                  // check if there was a hold
  if(hold){varifiedHold = heldASCII(hold);} // only feed in hold events
  else if(!buttonState(MONITOR_BUTTONS)){heldASCII(0);}//feed in release cases

  if(varifiedHold){actuation = varifiedHold;}
  return actuation;
}

//---- feedback & state handling ----
void feedbackAndRelease(){
  static boolean held = false;
  // general press hold, for holdstates see conversions.ino -> heldASCII()
  int currentState = buttonState(MONITOR_BUTTONS);
  if( held && currentState == 0 ){ //at the moment of a release
    patternVibrate(0);
    releaseKey();
    held = false;
  }else if(patternToChar(currentState)){ // if that state is a pattern
    patternVibrate(currentState);
    held = true;
  }
}

//---- Special macro functions ----
void macros(byte letter){
  if     (letter == 'a' + SPACEBAR){convertionMode(TRIGGER);} // toggle numbers
  //else if(letter == 'd' + SPACEBAR){PASH(TRIGGER);}
  else if(letter == 'i' + SPACEBAR){potentiometer(ADJUST_PWM);} // Toggle to pwm
  else if(letter == 'j' + SPACEBAR){comboPress(LEFT_ALT,0,0);}
  else if(letter == 'k' + SPACEBAR){keyOut(letter);} // toggle keyboard mode
  else if(letter == 'l' + SPACEBAR){comboPress(LEFT_CTRL|LEFT_ALT,0,0);}
  else if(letter == 'p' + SPACEBAR){potentiometer(TRIGGER);} // show value
  else if(letter == 's' + SPACEBAR){potentiometer(ADJUST_TIMING);} // toggle
}
