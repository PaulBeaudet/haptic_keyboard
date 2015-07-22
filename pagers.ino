// hardware.ino  Copyright Paul Beaudet 2014 See license for reuse info
// depends on wire and Adafruit_PWM
// pins A4 and A5 or expantion pins on leo
//---------------- Haptics / Pagers / Vib motors -----------------
#include "Adafruit_PWM.h"             // see readme for harware notes
#define NUMPAGERS 8                   // can use up to 16
#define COUNTBACKPAGERS 8 - 1
Adafruit_PWM pagers = Adafruit_PWM(); // create pagers object

void pagersUp() // to speed up i2c, go into 'fast 400khz I2C' mode
{               // might not work when sharing the I2C bus
  pagers.begin();
  pagers.setPWMFreq(40); // ? 1600 is the maximum PWM frequency ? says example
}                        // ? learn.adafuit says 40-1000hz ?

int pagerIntensity(int intensityChange)
{
  static int intensity = 4095;  // 0 being off and 4095 being most intense
  if(intensityChange){intensity = intensityChange;}
  else{return intensity;} //TODO this function needs work...
}

void patternVibrate(int pins)
{ // set state of all pagers in one function
  byte j = COUNTBACKPAGERS; // For oppisite direction, if forward remove
  for (byte i=0; i<NUMPAGERS; i++)
  { // incoming int set bit by bit: high bits: pagers need to be active
    if (pins & (1 << i)) { pagers.setPWM( j, 0, pagerIntensity(MONITOR_MODE));}
    else                 { pagers.setPWM( j, 0, 0); }//change j to i if forward
    j--; // remove if forward oriented
  }
}

boolean vibActive = false;

boolean noUserMessages(){
  if(vibActive){
    return false;
  }else{return true;}
}

boolean ptimeCheck(uint32_t durration)
{                                 // used for checking and setting timer
  static uint32_t ptimer[2]={1,0};// create timer to modify default check=true
  if(durration)                   // given param other than zero
  {                               // time is being set
    ptimer[1]=durration;          // set durration
    ptimer[0]=millis();           // note the time set
  }                               // if the durration has elapsed return true
  else if(millis() - ptimer[0] > ptimer[1]){return true;}//time has passed
  return false;                   // time has yet to pass
}

boolean hapticMessage(byte letter, int spacing = 0)
{ // updating function; passing a string sets course of action
  static boolean animated = false; // animated or typical letter?
  static int timing = 400;         // default durration of letter play

  if(spacing){timing = spacing; return false;} // change timing call

  if(letter)         // given a value comes through: init case
  {
    if(byte validPatern = charToPattern(letter))
    {                              // if the letter converts to a pattern
      ptimeCheck(timing);          // start the timer for specified durration
      patternVibrate(validPatern); // signal the pagers to vibrate
      vibActive = true;            // signal true for to user message
      animated = false;            // this is a non-animated pattern
    }
    else if(byte validAnimation = getFrame(0, letter))
    {     // if first frame is availible for this value then it is an animation
      int adjustedTime = timing / 8 + timing; // calculate total normal timing
      ptimeCheck(adjustedTime/NUMPAGERS);  // set frame durration (total/frames)
      patternVibrate(validAnimation);      // vibrate first frame
      vibActive = true;                    // signal true to a to user message
      animated = true;                     // This is an animation
    }                                      // invalid entries are skipped
    return false; // why bother checking time... we just set it
  }
  //--- 0 or "monitor" case --- given no value passed as letter, check timing
  if (animated){return animatedProcess(timing);} // run animation logic
  else {return typicalLetter(timing);}           // run letter display logic
}

boolean typicalLetter(int timing)
{
  static boolean touchPause= 0; // pause between displays

  if(ptimeCheck(0)) // check if time is up
  {                 // time to "display" a touch / pause elapsed
    if(touchPause)  // given that we are at second pause stage
    {               // this case allows for a pause after "display"
      touchPause=!touchPause; // reset stage
      vibActive = false;      // set 'to user message' back to false
      return true;  // Send confirmation this letter has been "played"
    }
    else                      // durring the letter buzz phase
    {
      touchPause=!touchPause; // flag pause time to start
      patternVibrate(0);      // stop letter feedback
      ptimeCheck(timing/8);   // set pause time (1/8 of active durration)
    };
  }
  return false; // signals letter still in process of being played
}

boolean animatedProcess(int timing)
{
  static byte frame = 0;

  if(ptimeCheck(0))        // if timer has been tripped
  {
    frame++;               // zero frame is accounted for in hapticMessage
    if(frame == NUMPAGERS) // reached maxium number of frames
    {
      frame = 0;           // Start back at frame zero
      getFrame(0,TRIGGER); // reset framer
      patternVibrate(0);   // turn pagers off
      vibActive = false;    // signal end of activity
      return true;         // animation complete
    }
    patternVibrate(getFrame(frame));       // start to play frame
    int adjustedTime = timing / 8 + timing;// calculate total time
    ptimeCheck(adjustedTime/NUMPAGERS);    // devide time by frames to be played
  }
  return false;
}

boolean streamOut(byte letter){  // return true when qued letter is finished
  if(letter){                                 // given a letter
    if(letter != 13){keyOut(letter);} // exclude CARIAGE_RETURN lf/cr
    return hapticMessage(letter);             // start vibration
  }
  else{ return hapticMessage(MONITOR_MODE); } // if done return true
}

//----------------adjusting settings with pontentiometer---------
// set ADJUST_POT in pin_definitions.h

void potentiometer(byte mode)             // Main loop potentiometer check
{
  static boolean intensity = true;        // default adjust mode: intensity
  int potValue = analogRead(ADJUST_POT);  // read current pot value

  if(mode == MONITOR_MODE)
  { // check to do adjustments on either intensity or spacing
    if (intensity) {pagerIntensity(map(potValue, 0, 1023, 1500, 3500));}
    else {hapticMessage(0, map(potValue, 0, 1023, 10, 700));}
  }
  else if (mode == DEFAULT_MODE){potReturn(potValue);} // "flash value"
  else if (mode == ADJUST_PWM)        // intensity mode case
  {
    intensity = true;                 // set function to adjust intensity
    potReturn(potValue);              // "flash" current value
  }
  else if (mode == ADJUST_TIMING)     // timing/spacing mode case
  {
    intensity = false;                // set function to adjust timing
    potReturn(potValue);              // "flash" current value
  }
}

void potReturn(int potValue)
{
  byte rawNumber = map(potValue,0,1023,0,9);
  keyOut(rawNumber + 48);                // convert to ascii number
  hapticMessage(rawNumber + 48);         // start feedback
  // TODO !!!!! BLOCKING OPPERATION !!!!!!! <-------
  while(!hapticMessage(MONITOR_MODE)){;} // break loop when letter is finished
  keyOut(BACKSPACE);                     // key message
}
