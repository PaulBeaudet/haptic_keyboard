// SDnotes.ino ~ Copyright 2015 Paul Beaudet ~ See Licence for details
// This adds SD card support to the Arduino Micro version of the project
// Purpose is to be able to take and read backs notes independent of a screen
#include <SPI.h>
#include <SD.h>

char bkmrks[] = "MARKS.TXT";

void setupSD()
{
  pinMode(SS, OUTPUT);
  if(SD.begin(SS)){
    //SD.remove(bkmrks); //uncoment to start bookmarks from scratch
    //File marks = SD.open(bkmrks, FILE_WRITE); marks.close(); // create if not
    workingFile(TRIGGER); // que up first file
  }
}

// return a pointer to the name of the current file
char* workingFile(boolean next){ // merry go round style excluding directories
  static File workingDirectory = SD.open("/");
  static char fileCurrent[13];  // store the name of the file instead of the file

  if(next){
    File next = workingDirectory.openNextFile();
    if(!next){
      workingDirectory.rewindDirectory();
      next = workingDirectory.openNextFile();
    }
    while(next.isDirectory()){                 // make sure this file isn't a dir
      next.close();
      next = workingDirectory.openNextFile();  //
      if(!next){
        workingDirectory.rewindDirectory();
        next = workingDirectory.openNextFile();
      }
    }
    char* filename = next.name();
    byte i = 0;
    while(filename[i]){
      fileCurrent[i] = filename[i];
      i++;
    }
    fileCurrent[i] = '\0'; // null terminate
    next.close();
  }
  return fileCurrent;
}

unsigned long bookmarkIt(char* filename, unsigned long bookmark){
  unsigned long foundMark = 0;
  unsigned long existingEntry = findString(bkmrks, filename);
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
  if(pos){              // edit entry
    target.seek(pos);   // seek to edit pos
  }else{                // new entry
    target.print(name); // denote name of file
    target.print('-');  // make it human parsible
  }
  if(num){
    target.println(num);               // this overwrites bytes after pos
    if(!pos){                          // given that this is a new entry
      for(byte i = 0; i < 9; i++){target.print('*');} // add chars to bleed into
      target.println();                // yes this is crusty, but it will work
    }
  }
  target.close(); // open one file at a time or this will crash your $h!T!
}


unsigned long findALong(char* filename, unsigned long pos){
  File target = SD.open(filename);
  target.seek(pos);                 // assumes addres is correct

  byte index = 0;                   // denotes highest place
  char longNumber[11];              // up to the billions plus null
  while(target.available()){        // so long as we can read chars
    char number = target.read();    // read next possible number
    if(number < 58 && number > 47){ // given we are dealing with numbers
      longNumber[index] = number;   // add the numbers to the array
      index++;                      // increment index
    } else {break;} // if not a number basically expecting cr/lf
  }
  target.close();
  longNumber[index] = '\0'; // Null terminate
  return atol(longNumber);
}

unsigned long findString(char* filename, char* string){
  File target = SD.open(filename);

  unsigned long pos = 0;
  byte matched = 0;
  while(target.available()){
    if(string[matched] == target.read()){
      Serial.println(target.position()); //----------!!!!
      if(matched + 1 == strlen(string)){
        pos = target.position() + 1;
        break;
      }
      matched++;
    }
    else{matched = 0;}
  }
  target.close();
  return pos;
}

char charOfCurrent(unsigned long position){
  File myFile = SD.open(workingFile(MONITOR_MODE)); // open the workingFile
  myFile.seek(position);       // seek to read position
  char letter = myFile.read(); // read byte
  myFile.close();              // close file; IMPORTANT
  return letter;               // return letter for position
}

// ******** Command Functions *********
byte resumeRead(byte print){
  static boolean printing = false;
  static unsigned long mark = 0;

  if(print){
    printing = !printing;
    if(printing){
      mark = bookmarkIt(workingFile(MONITOR_MODE), 0);
    }else{
      bookmarkIt(workingFile(MONITOR_MODE), mark);
      while(!streamOut(MONITOR_MODE)){;} // block until last letter is done
      return 0;
    }
  }

  if(printing){
    if(streamOut(MONITOR_MODE)){
      char letter = charOfCurrent(mark);
      if(letter){
        streamOut(letter);
        mark++;
      } else {
        printing = false;
        return 0;
      }
    }
  }
  return 'r';
}

byte cat(byte print){
  static boolean printing = false;
  static unsigned long position = 0;

  if(print){
    printing = !printing;
    if(!printing){
      position=0;
      while(!streamOut(MONITOR_MODE)){;} // block until last letter is done
      return 0;
    } // handle quit signal
  }

  if(printing){
    if(streamOut(MONITOR_MODE)){
      char letter = charOfCurrent(position); // get current letter
      if(letter){                 // check if we are pointing to a letter
        streamOut(letter);        // stream that letter out
        position++;               // increment pointer to next letter
      } else {
        printing = false;
        position = 0;
        return 0;
      }
    }
  }
  return 'c';
}

byte fileNext(byte print){
  static boolean printing = false;
  static char* name = workingFile(MONITOR_MODE);

  if(print){
    printing = !printing;
    if(printing){
      name = workingFile(TRIGGER); //because why would a pointer to a static var change?
    }else{
      while(!streamOut(MONITOR_MODE)){;} // block until last message is done
      return 0;
    } // handle quit signal
  }

  if(printing){
    if(streamOut(MONITOR_MODE)){
      if(name[0]){           // check if we are pointing to a letter
        streamOut(name[0]);  // stream that letter out
        name++;              // increment pointer to next letter
      } else {
        keyOut(CARIAGE_RETURN);
        printing = false;
        return 0;
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
      char message[] = "PASH ";
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
    else if (activeCMD == 'n'){feedback = fileNext(cmd);}
    else if (activeCMD == 'r'){feedback = resumeRead(cmd);}
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
