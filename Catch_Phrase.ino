/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <LiquidCrystal.h>

const int rs = 15, en = 2, d4 = 32, d5 = 33, d6 = 25, d7 = 26;//Screen Pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//team symbols

<<<<<<< HEAD
=======
byte team1[] = {
  B11100,
  B01000,
  B01000,
  B00000,
  B00110,
  B00010,
  B00010,
  B00111
};
>>>>>>> parent of 73d73c4... Added most of the code needed

byte team2[] = {
  B11100,
  B01000,
  B01000,
  B00000,
  B00110,
  B00001,
  B00010,
  B00111
};


<<<<<<< HEAD
//Timer
volatile int secondCounter;

int gameClock;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//team symbols

byte team1[] = {
  B11100,
  B01000,
  B01000,
  B00000,
  B00110,
  B00010,
  B00010,
  B00111
};

byte team2[] = {
  B11100,
  B01000,
  B01000,
  B00000,
  B00110,
  B00001,
  B00010,
  B00111
};


=======
>>>>>>> parent of 73d73c4... Added most of the code needed
void printToScreen(char toScreen[], int layer, int type){//This function writes the charactor array to the screen. Type = 0 standard; Type = 1 category(removes .txt and \;


  
  //Cleans up the name
  if(type == 1){
    toScreen = toScreen + 1;
    //toScreen[strlen(toScreen)-4] = '\0';//Ends the string before the extension
  }
  
      
  //Clears line
  for(int i = 1; i <= 16; ++i){
    lcd.setCursor(i,layer);
    lcd.write(" ");
  }

  int j = 0; //word interator

  int startPos = (8-(strlen(toScreen)/2));
  
  for(int i = startPos; i < startPos + strlen(toScreen); ++i){
    
    lcd.setCursor(i,layer);
    
    lcd.write(toScreen[j]);

    j++;
    
    
  }
  
  
}
<<<<<<< HEAD

void printTeamSymbols(){
  lcd.setCursor(0,0);
  lcd.write(byte(0));
  lcd.setCursor(15,0);
  lcd.write(byte(1));
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){//lists the directories (not really needed for our purposes)
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){//creates a directory (not really needed for our purposes)
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){//we don't really need this
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

char* readFile(fs::FS &fs, const char * path){
    //Serial.printf("Reading file: %s\n", path);

    char oneLine[16];
    Serial.println("Opening File");
    File file = fs.open(path);
    Serial.println("File Opened");
    
    int count = 0;
    while(file.available()){
        Serial.println("Reading File");
        oneLine[count] = file.read();
        Serial.println(oneLine[count]);
        count++;
    }
    //Serial.println(oneLine);
    file.close();
    Serial.println("File Closed");
    return oneLine;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

/*
  ╔╗─╔╗╔═══╗╔╗───╔═══╗╔═══╗╔═══╗     ╔═══╗╔╗─╔╗╔═╗─╔╗╔═══╗╔════╗╔══╗╔═══╗╔═╗─╔╗╔═══╗
  ║║─║║║╔══╝║║───║╔═╗║║╔══╝║╔═╗║     ║╔══╝║║─║║║║╚╗║║║╔═╗║║╔╗╔╗║╚╣─╝║╔═╗║║║╚╗║║║╔═╗║
  ║╚═╝║║╚══╗║║───║╚═╝║║╚══╗║╚═╝║     ║╚══╗║║─║║║╔╗╚╝║║║─╚╝╚╝║║╚╝─║║─║║─║║║╔╗╚╝║║╚══╗
  ║╔═╗║║╔══╝║║─╔╗║╔══╝║╔══╝║╔╗╔╝     ║╔══╝║║─║║║║╚╗║║║║─╔╗──║║───║║─║║─║║║║╚╗║║╚══╗║
  ║║─║║║╚══╗║╚═╝║║║───║╚══╗║║║╚╗     ║║───║╚═╝║║║─║║║║╚═╝║──║║──╔╣─╗║╚═╝║║║─║║║║╚═╝║
  ╚╝─╚╝╚═══╝╚═══╝╚╝───╚═══╝╚╝╚═╝     ╚╝───╚═══╝╚╝─╚═╝╚═══╝──╚╝──╚══╝╚═══╝╚╝─╚═╝╚═══╝
*/
int totalSecondCounter;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
=======
>>>>>>> parent of 73d73c4... Added most of the code needed

void printTeamSymbols(){
  lcd.setCursor(0,0);
  lcd.write(byte(0));
  lcd.setCursor(15,0);
  lcd.write(byte(1));
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){//lists the directories (not really needed for our purposes)
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){//creates a directory (not really needed for our purposes)
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){//we don't really need this
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

char* readFile(fs::FS &fs, const char * path){
    //Serial.printf("Reading file: %s\n", path);

    char oneLine[16];
    Serial.println("Opening File");
    File file = fs.open(path);
    Serial.println("File Opened");
    
    int count = 0;
    while(file.available()){
        Serial.println("Reading File");
        oneLine[count] = file.read();
        Serial.println(oneLine[count]);
        count++;
    }
    //Serial.println(oneLine);
    file.close();
    Serial.println("File Closed");
    return oneLine;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

<<<<<<< HEAD
/*
╔═══╗╔═══╗╔════╗╔╗─╔╗╔═══╗     ╔═══╗╔╗─╔╗╔═╗─╔╗╔═══╗╔════╗╔══╗╔═══╗╔═╗─╔╗
║╔═╗║║╔══╝║╔╗╔╗║║║─║║║╔═╗║     ║╔══╝║║─║║║║╚╗║║║╔═╗║║╔╗╔╗║╚╣─╝║╔═╗║║║╚╗║║
║╚══╗║╚══╗╚╝║║╚╝║║─║║║╚═╝║     ║╚══╗║║─║║║╔╗╚╝║║║─╚╝╚╝║║╚╝─║║─║║─║║║╔╗╚╝║
╚══╗║║╔══╝──║║──║║─║║║╔══╝     ║╔══╝║║─║║║║╚╗║║║║─╔╗──║║───║║─║║─║║║║╚╗║║
║╚═╝║║╚══╗──║║──║╚═╝║║║───     ║║───║╚═╝║║║─║║║║╚═╝║──║║──╔╣─╗║╚═╝║║║─║║║
╚═══╝╚═══╝──╚╝──╚═══╝╚╝───     ╚╝───╚═══╝╚╝─╚═╝╚═══╝──╚╝──╚══╝╚═══╝╚╝─╚═╝                        
*/
void setup()
{
    //Disable interrupts
    cli();

    lcd.begin(16, 2);
    lcd.createChar(0, team1);
    lcd.createChar(1, team2);

    //Setup pins for the buttons
    pinMode(TEAM_1_BUTTON.pin, INPUT);
    pinMode(TEAM_2_BUTTON.pin, INPUT);
    pinMode(START_BUTTON.pin, INPUT);
    pinMode(CATEGORY_BUTTON.pin, INPUT);
    pinMode(NEXT_BUTTON.pin, INPUT);

    //Set up Timer
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000000, true);

    Serial.begin(115200);

    initSDCard();

    attachInterrupt(TEAM_1_BUTTON.pin, team1ButtonFunction, RISING);
    attachInterrupt(TEAM_2_BUTTON.pin, team2ButtonFunction, RISING);
    attachInterrupt(START_BUTTON.pin, startButtonFunction, RISING);
    attachInterrupt(CATEGORY_BUTTON.pin, categoryButtonFunction, RISING);
    attachInterrupt(NEXT_BUTTON.pin, nextButtonFunction, RISING);

    //Enable interrupts
    sei();
}

/*
╔╗───╔═══╗╔═══╗╔═══╗     ╔═══╗╔╗─╔╗╔═╗─╔╗╔═══╗╔════╗╔══╗╔═══╗╔═╗─╔╗
║║───║╔═╗║║╔═╗║║╔═╗║     ║╔══╝║║─║║║║╚╗║║║╔═╗║║╔╗╔╗║╚╣─╝║╔═╗║║║╚╗║║
║║───║║─║║║║─║║║╚═╝║     ║╚══╗║║─║║║╔╗╚╝║║║─╚╝╚╝║║╚╝─║║─║║─║║║╔╗╚╝║
║║─╔╗║║─║║║║─║║║╔══╝     ║╔══╝║║─║║║║╚╗║║║║─╔╗──║║───║║─║║─║║║║╚╗║║
║╚═╝║║╚═╝║║╚═╝║║║───     ║║───║╚═╝║║║─║║║║╚═╝║──║║──╔╣─╗║╚═╝║║║─║║║
╚═══╝╚═══╝╚═══╝╚╝───     ╚╝───╚═══╝╚╝─╚═╝╚═══╝──╚╝──╚══╝╚═══╝╚╝─╚═╝*/

void loop()
{
    //Category Selection
    displayNextCategory();
    while (START_BUTTON.pressed == false)
    {
        if (CATEGORY_BUTTON.pressed)
        {
            displayNextCategory();
            CATEGORY_BUTTON.pressed = false;
        }
        delay(10);
=======
void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
>>>>>>> parent of 73d73c4... Added most of the code needed
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

<<<<<<< HEAD
/*
╔╗─╔╗╔═══╗╔╗───╔═══╗╔═══╗╔═══╗     ╔═══╗╔╗─╔╗╔═╗─╔╗╔═══╗╔════╗╔══╗╔═══╗╔═╗─╔╗╔═══╗
║║─║║║╔══╝║║───║╔═╗║║╔══╝║╔═╗║     ║╔══╝║║─║║║║╚╗║║║╔═╗║║╔╗╔╗║╚╣─╝║╔═╗║║║╚╗║║║╔═╗║
║╚═╝║║╚══╗║║───║╚═╝║║╚══╗║╚═╝║     ║╚══╗║║─║║║╔╗╚╝║║║─╚╝╚╝║║╚╝─║║─║║─║║║╔╗╚╝║║╚══╗
║╔═╗║║╔══╝║║─╔╗║╔══╝║╔══╝║╔╗╔╝     ║╔══╝║║─║║║║╚╗║║║║─╔╗──║║───║║─║║─║║║║╚╗║║╚══╗║
║║─║║║╚══╗║╚═╝║║║───║╚══╗║║║╚╗     ║║───║╚═╝║║║─║║║║╚═╝║──║║──╔╣─╗║╚═╝║║║─║║║║╚═╝║
╚╝─╚╝╚═══╝╚═══╝╚╝───╚═══╝╚╝╚═╝     ╚╝───╚═══╝╚╝─╚═╝╚═══╝──╚╝──╚══╝╚═══╝╚╝─╚═╝╚═══╝
*/

void initSDCard()
{
  //Finds the SD Card
  if (!SD.begin())
  {
    Serial.println("Card Mount Failed");
    printToScreen("Card Mount Failed", 1, 0);

    return;
  }
  uint8_t cardType = SD.cardType();
=======
=======
>>>>>>> parent of 73d73c4... Added most of the code needed
void setup(){
    
    
    
  lcd.begin(16, 2);
  lcd.createChar(0,team1);
  lcd.createChar(1,team2);
    
    
    pinMode(35, INPUT); //Setup pins for the buttons
    pinMode(16, INPUT);
    pinMode(17, INPUT);
    pinMode(14, INPUT);
    pinMode(27, INPUT);
      
    Serial.begin(115200);//Finds the SD Card
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();
>>>>>>> parent of 25351b3... Random game length

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    //listDir(SD, "/", 0);
    //createDir(SD, "/mydir");
    //listDir(SD, "/", 0);
    //removeDir(SD, "/mydir");
    //listDir(SD, "/", 2);
    //writeFile(SD, "/hello.txt", "Hello ");
    //appendFile(SD, "/hello.txt", "World!\n");
    //readFile(SD, "/hello.txt");
    //deleteFile(SD, "/foo.txt");
    //renameFile(SD, "/hello.txt", "/foo.txt");
    //readFile(SD, "/foo.txt");
    //testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

    char wordDir[] = "/"; //Directory of the word lists
<<<<<<< HEAD

  
    SD.begin();
    File root = SD.open(wordDir);
    File category = root.openNextFile();

    if(! category){//end of the list
      Serial.println("No lists"); 
    }else{
    
    printToScreen(strdup(category.name()),1,1);

    Serial.println(strdup(category.name()));
    }
    root.close();
    
    

    
}

void loop(){

    //State of the left button
    bool currStateLeft;
    bool lastStateLeft;

   //State of the Right button
    bool currStateRight;
    bool lastStateRight;

    //State of the category button
    bool currStateCategory;
    bool lastStateCategory;

    //State of the button for next
    bool currStateNext;
    bool lastStateNext;

    //State if the button for Go
    bool currStateGo;
    bool lastStateGo;

    

    //Directory of the word lists
    const char wordDir[] = "/"; 

    SD.begin();
    File root = SD.open(wordDir);
    File category = root.openNextFile();

    if(! category){//end of the list
      Serial.println("No lists");
    }
  
    

  //char clearDisplay[16] = "                ";

  char displayCategories[] = "Category:";
  char phrase[17];

  

  
  printToScreen(displayCategories, 0, 0);
  int selCategory = 0;
  
  while(true){

    currStateLeft     = digitalRead(16);//buttons for controlling selections
    currStateRight    = digitalRead(17);
    currStateCategory = digitalRead(14);
    currStateGo       = digitalRead(35);
    


    //This detects if the left or right buttons have been pressed to change the catagory
    if(currStateLeft != lastStateLeft || currStateRight != lastStateRight){//I
      if(currStateLeft == 1 || currStateRight == 1){
        if(currStateRight == 1){//Only gooes in one direction
          //TODO: Use Doubly linked lists to travel the list in both directions
          category = root.openNextFile();


          char systemVolume[] = "/System Volume";
          boolean sysVolSame = true;
          for(int i = 0; i < strlen(systemVolume); ++i){
            if(systemVolume[i] != category.name()[i]){
              sysVolSame = false;
              
              break;
            }
          }
          if(sysVolSame){//skip system volume information
            Serial.println("System Volume Skipped");
            category = root.openNextFile();
          }
          
          if(! category){//end of the list
            root.rewindDirectory();//go to list start
            category = root.openNextFile();
          }

          
          
          printToScreen(strdup(category.name()),1,1);
          Serial.println(category.name());
          
          
        }
      }
    }
    //This detects if the catagory has been chosen and starts the game
    if(currStateGo != lastStateGo){
      if(currStateGo== true){
        if(!category.available()){
          Serial.println("No words in category");
          return;
        }

        unsigned long startTime = millis();
        while(millis() < startTime + 60000){
          currStateNext = digitalRead(27);
          if(currStateNext != lastStateNext){
            if(currStateNext == true){
              Serial.println("Next Word");
        
            
          
              int i = 1;
              while(category.peek() != '\n' && category.peek() != -1 && i < 14){
                phrase[i] = category.read();
                i++;
                Serial.println(i);
              }
              //printToScreen(phrase,0,1);
            }
        
          }
          delay(10);
          lastStateNext = currStateNext;
        }
      }   
    }
=======

  
    SD.begin();
    File root = SD.open(wordDir);
    File category = root.openNextFile();

    if(! category){//end of the list
      Serial.println("No lists"); 
    }else{
    
    printToScreen(strdup(category.name()),1,1);

    Serial.println(strdup(category.name()));
    }
    root.close();
    
    

    
}

void loop(){

    //State of the left button
    bool currStateLeft;
    bool lastStateLeft;

   //State of the Right button
    bool currStateRight;
    bool lastStateRight;

    //State of the category button
    bool currStateCategory;
    bool lastStateCategory;

    //State of the button for next
    bool currStateNext;
    bool lastStateNext;

    //State if the button for Go
    bool currStateGo;
    bool lastStateGo;

    

    //Directory of the word lists
    const char wordDir[] = "/"; 

    SD.begin();
    File root = SD.open(wordDir);
    File category = root.openNextFile();

    if(! category){//end of the list
      Serial.println("No lists");
    }
  
    

  //char clearDisplay[16] = "                ";

  char displayCategories[] = "Category:";
  char phrase[17];

  

  
  printToScreen(displayCategories, 0, 0);
  int selCategory = 0;
  
  while(true){

    currStateLeft     = digitalRead(16);//buttons for controlling selections
    currStateRight    = digitalRead(17);
    currStateCategory = digitalRead(14);
    currStateGo       = digitalRead(35);
    


    //This detects if the left or right buttons have been pressed to change the catagory
    if(currStateLeft != lastStateLeft || currStateRight != lastStateRight){//I
      if(currStateLeft == 1 || currStateRight == 1){
        if(currStateRight == 1){//Only gooes in one direction
          //TODO: Use Doubly linked lists to travel the list in both directions
          category = root.openNextFile();


          char systemVolume[] = "/System Volume";
          boolean sysVolSame = true;
          for(int i = 0; i < strlen(systemVolume); ++i){
            if(systemVolume[i] != category.name()[i]){
              sysVolSame = false;
              
              break;
            }
          }
          if(sysVolSame){//skip system volume information
            Serial.println("System Volume Skipped");
            category = root.openNextFile();
          }
          
          if(! category){//end of the list
            root.rewindDirectory();//go to list start
            category = root.openNextFile();
          }

          
          
          printToScreen(strdup(category.name()),1,1);
          Serial.println(category.name());
          
          
        }
      }
    }
    //This detects if the catagory has been chosen and starts the game
    if(currStateGo != lastStateGo){
      if(currStateGo== true){
        if(!category.available()){
          Serial.println("No words in category");
          return;
        }

        unsigned long startTime = millis();
        while(millis() < startTime + 60000){
          currStateNext = digitalRead(27);
          if(currStateNext != lastStateNext){
            if(currStateNext == true){
              Serial.println("Next Word");
        
            
          
              int i = 1;
              while(category.peek() != '\n' && category.peek() != -1 && i < 14){
                phrase[i] = category.read();
                i++;
                Serial.println(i);
              }
              //printToScreen(phrase,0,1);
            }
        
          }
          delay(10);
          lastStateNext = currStateNext;
        }
      }   
    }
>>>>>>> parent of 73d73c4... Added most of the code needed
  
    delay(10);
    lastStateCategory = currStateCategory;
    lastStateLeft     = currStateLeft;
    lastStateRight    = currStateRight;
    lastStateGo       = currStateGo;
  }
  Serial.println("Go!!!!");
}
