// SDnotes.ino ~ Copyright 2015 Paul Beaudet ~ See Licence for details
// This adds SD card support to the Arduino Micro version of the project
// Purpose is to be able to take and read backs notes independent of a screen
#include <SPI.h>
#include <SD.h>

File workingDir;

void setupSD()
{
  pinMode(SS, OUTPUT);
  if(SD.begin(SS)){
    workingDir = SD.open("/");
  }
}

File carosel(byte request) throw(){  // centralizes file system opperations
  // static File workingDir = SD.open("/");
  static File nextFile = workingDir.openNextFile();
  static byte index = 0;
  static byte markedIndex = 0;
  static byte highestIndex = 0;

  if(request == 'n'){
    nextFile.close();
    nextFile = workingDir.openNextFile();
    if(nextFile){index++;}
    else
    {
      workingDir.rewindDirectory();
      nextFile = workingDir.openNextFile();
      highestIndex = index;
      index = 0;
    }
  }
  if(request == 'c'){
    if(index){
      markedIndex = index;
      while(index != markedIndex - 1){carosel('n');}
    } else { // in the case index is zero
      carosel('n');
      while(index != highestIndex){carosel('n');}
    }
  }

  return nextFile;
}

unsigned long bookmarkIt(char* filename, unsigned long bookmark){
  Serial.println("GI");
  char bkmrks[] = "MARKS.TXT";
  //File marks = SD.open(bkmrks, FILE_WRITE); marks.close(); // create if not
  unsigned long foundMark = 0;
  Serial.println("#"); // ---------------!!!!!!!!
  unsigned long existingEntry = findString(bkmrks, filename);
  Serial.print("ex-");
  Serial.println(existingEntry); //---------------!!!!!!!!
  if(bookmark){ //in the case of a bookmark write mark to sd
    writeALong(bkmrks, filename, existingEntry, bookmark);
  }else{
    foundMark = findALong(bkmrks, existingEntry);
  }
  return foundMark;
}

void writeALong(char* file, char* name, unsigned long pos, unsigned long num){
  //timeCheck(10); // let run for x milliseconds
  File target = SD.open(file, FILE_WRITE);
  if(pos){             // edit entry
    target.seek(pos);  // seek to edit pos
  }else{               // new entry
    target.print(name);
    target.print('-');
  }
  if(num){target.println(num);}
  target.close();
}


unsigned long findALong(char* filename, unsigned long pos){
  File target = SD.open(filename);
  if(target){Serial.println("FL");}
  target.seek(pos);                 // assumes addres is correct

  byte index = 0;                   // denotes highest place
  char longNumber[11];              // up to the billions plus null
  while(target.available()){        // so long as we can read chars
    char number = target.read();    // read next possible number
    Serial.print("ps-");
    Serial.println(number);        //----!!!!!!!!!!
    if(number < 58 && number > 47){ // given we are dealing with numbers
      longNumber[index] = number;   // add the numbers to the array
      index++;                      // increment index
    } else {break;} // if not a number basically expecting cr/lf
  }
  target.close();
  Serial.println("AC");
  longNumber[index] = '\0'; // Null terminate
  return atol(longNumber);
}

unsigned long findString(char* filename, char* string){
  File target = SD.open(filename);
  if(target){Serial.println("FS");}

  unsigned long pos = 0;
  byte matched = 0;
  while(target.available()){
    if(string[matched] == target.read()){
      Serial.print("*");
      Serial.println(target.position()); //----------!!!!
      if(matched + 1 == strlen(string)){
        pos = target.position() + 1;
        Serial.println("!");
        break;
      }
      matched++;
    }
    else{matched = 0;}
  }
  target.close();
  Serial.println("AC");
  return pos;
}

// ******** Command Functions *********
byte cat(byte print){
  static File myFile = SD.open("Alice.txt"); // find a default file
  static boolean printing = false;

  char* name = myFile.name();
  if(print == 'c' || print == 'q' || print == 'e'){
    printing = !printing;
    if(printing){myFile = carosel('c');}
    else{
      unsigned long pos = myFile.position();
      myFile.close();
      if(print == 'e'){bookmarkIt(name, pos);}
      return 0;
    }
  }
  if(print == 'b'){
    myFile.close();
    unsigned long pos= bookmarkIt(name, 0);
    myFile = SD.open(name);
    myFile.seek(pos);
  }

  if(printing){
    if(streamOut(MONITOR_MODE)){
      byte nextLetter = myFile.read();
      if(nextLetter == 0xff){
        myFile.close();
        printing = false;
        return 0;                 //end signal
      } else {
        streamOut(nextLetter);
      }
    }
  }
  return 'c';
}

byte currentFile(byte print){
  static boolean printing = false;
  static byte index = 0;

  if(print){
    printing = !printing;
    if(!printing){return index=0;} // handle quit signal
  }

  if(printing){                  // given TRIGGER start print next file
    if(streamOut(MONITOR_MODE)){
      File current = carosel(MONITOR_MODE);
      char* name = current.name();
      if(name[index]){
        streamOut(name[index]);
        index++;
      } else {
        if(current.isDirectory()){keyOut('/');}
        keyOut(CARIAGE_RETURN);
        carosel('n');
        printing = false;
        return index = 0;
      }
    }
  }
  return 'n';
}

byte welcomePASH(byte print){
  static boolean printing = false;
  static byte index = 0;

  if(print){
    printing = !printing;
    if(!printing){return index=0;} // handle quit signal
  }

  if(printing){
    if(streamOut(MONITOR_MODE)){
      char message[] = "PASH";
      // TODO fix animation hang (space)
      if(message[index]){
        streamOut(message[index]);
        index++;
      } else {
        printing = false;
        keyOut(CARIAGE_RETURN);
        return index = 0;  // returns zero ending the routine
      }
    }
  }
  return 'z';
}

// *************** ~ PASH ~ ********************** //
// "Paul's Awful Shell" or with the toung, which ever you prefer
void PASH(byte cmd){                               // PASH activity toggle
  static boolean active = false;                   // is PASH running
  static byte activeCMD = 'z';                     //
  static byte previousCMD = 0;                     //
  if(cmd == TRIGGER){
    active = !active;
    if(active){cmd = 'z';}
  } // first code welcomePASH

  if(active){
    byte feedback = 0;
    if(!activeCMD){
      activeCMD = cmd;
      if(cmd && cmd != BACKSPACE){keyOut(BACKSPACE);} // remove entry
    } //allows cmd to be its own trigger

    if      (activeCMD == 'c'){feedback = cat(cmd);}
    else if (activeCMD == 'n'){feedback = currentFile(cmd);}
    else if (activeCMD == 'z'){feedback = welcomePASH(cmd);}
    else {activeCMD = 0;} // cmd not found -> quit

    if(feedback == '|'){previousCMD = activeCMD;} // concurant cmd signal
    else
    {
      if(!feedback && previousCMD){        // given concurant cmd is finised
        activeCMD = previousCMD;           // resume previous cmd
        previousCMD = 0;
      }
      else{ activeCMD = feedback; }
    }
    if(cmd == 'q'){activeCMD = 0;}        // quit signal
  }// routine is responsible for closing itself
}
