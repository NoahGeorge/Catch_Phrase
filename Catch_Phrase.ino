#include "SD.h"
#include <LiquidCrystal.h>

//LCD Screen Pins
const int rs = 15, en = 2, d4 = 32, d5 = 33, d6 = 25, d7 = 26;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Scoring
int team1Score = 0;
int team2Score = 0;
const int WINNING_SCORE = 7;

const int ROUND_TIME_IN_SECONDS = 30; //TODO make round time random within a range

File root;
File category;

int currentWord = 0;

//Timer
volatile int secondCounter;
int totalSecondCounter;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

//Buttons
struct Button
{
    const uint8_t pin;
    volatile bool pressed;
};
Button TEAM_1_BUTTON = {16, false}, TEAM_2_BUTTON = {17, false}, START_BUTTON = {22, false}, CATEGORY_BUTTON = {14, false}, NEXT_BUTTON = {27, false};

//team symbols
byte team1[] = {B11100, B01000, B01000, B00000, B00110, B00010, B00010, B00111};
byte team2[] = {B11100, B01000, B01000, B00000, B00110, B00001, B00010, B00111};

//Interrupt Service Routines which become activated when a button is pressed
void IRAM_ATTR team1ButtonFunction()
{
    TEAM_1_BUTTON.pressed = true;
}

void IRAM_ATTR team2ButtonFunction()
{
    TEAM_2_BUTTON.pressed = true;
}

void IRAM_ATTR startButtonFunction()
{
    START_BUTTON.pressed = true;
}

void IRAM_ATTR categoryButtonFunction()
{
    CATEGORY_BUTTON.pressed = true;
}

void IRAM_ATTR nextButtonFunction()
{
    NEXT_BUTTON.pressed = true;
}

//Timer Interrupt Service Routine
void IRAM_ATTR onTimer()
{
    portENTER_CRITICAL_ISR(&timerMux);
    secondCounter++;
    portEXIT_CRITICAL_ISR(&timerMux);
}

/*
   _____      _               __  __      _   _               _ 
  / ____|    | |             |  \/  |    | | | |             | |
 | (___   ___| |_ _   _ _ __ | \  / | ___| |_| |__   ___   __| |
  \___ \ / _ \ __| | | | '_ \| |\/| |/ _ \ __| '_ \ / _ \ / _` |
  ____) |  __/ |_| |_| | |_) | |  | |  __/ |_| | | | (_) | (_| |
 |_____/ \___|\__|\__,_| .__/|_|  |_|\___|\__|_| |_|\___/ \__,_|
                       | |                                      
                       |_|                                      
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
  _                         __  __      _   _               _ 
 | |                       |  \/  |    | | | |             | |
 | |     ___   ___  _ __   | \  / | ___| |_| |__   ___   __| |
 | |    / _ \ / _ \| '_ \  | |\/| |/ _ \ __| '_ \ / _ \ / _` |
 | |___| (_) | (_) | |_) | | |  | |  __/ |_| | | | (_) | (_| |
 |______\___/ \___/| .__/  |_|  |_|\___|\__|_| |_|\___/ \__,_|
                   | |                                        
                   |_|                                        */

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
    }
    START_BUTTON.pressed = false;
    Serial.printf("Starting with category: %s\n", category.name());

    //Shuffle word list
    int numWords = getNumWordsInFile(category);
    Serial.printf("Number of words in file: %d\n", numWords);
    String wordList[numWords];
    fillRandomizedWordArrayFromFile(wordList, category, numWords);

    //Gameplay loop
    while (team1Score < WINNING_SCORE && team2Score < WINNING_SCORE)
    {
        timerAlarmEnable(timer);

        displayNextWord(wordList, numWords);

        //Timer is running, word to guess is displayed on screen
        while (totalSecondCounter < ROUND_TIME_IN_SECONDS)
        {
            //Get the next word when next button is pressed
            if (NEXT_BUTTON.pressed)
            {
                displayNextWord(wordList, numWords);
                NEXT_BUTTON.pressed = false;
            }

            //Update the clock if a second has passed
            if (secondCounter > 0)
            {
                updateClock();
            }
        }

        //Stop the timer and reset the timer values
        Serial.println("Timer Expired");
        timerAlarmDisable(timer);
        TEAM_1_BUTTON.pressed = false;
        TEAM_2_BUTTON.pressed = false;
        secondCounter = 0;
        totalSecondCounter = 0;

        //Between rounds, waiting for score update or the start of the next round
        while (START_BUTTON.pressed == false)
        {
            if (TEAM_1_BUTTON.pressed)
            {
                team1Score++;
                Serial.printf("Team 1 earned a point. The score is now %d-%d\n", team1Score, team2Score);
                printTeamScores();
                TEAM_1_BUTTON.pressed = false;
            }

            if (TEAM_2_BUTTON.pressed)
            {
                team2Score++;
                Serial.printf("Team 2 earned a point. The score is now %d-%d\n", team1Score, team2Score);
                printTeamScores();
                TEAM_2_BUTTON.pressed = false;
            }

            if (team1Score >= WINNING_SCORE)
            {
                Serial.println("Team 1 WINS.");
                break;
            }

            if (team2Score >= WINNING_SCORE)
            {
                Serial.println("Team 2 WINS.");
                break;
            }

            delay(10);
        }
        START_BUTTON.pressed = false;
    }
    //Game over
    delay(4000);
    lcd.clear();
    team1Score = 0;
    team2Score = 0;
    currentWord = 0;
    Serial.println("Starting new game.");
}

/*
  _    _      _                   __  __      _   _               _     
 | |  | |    | |                 |  \/  |    | | | |             | |    
 | |__| | ___| |_ __   ___ _ __  | \  / | ___| |_| |__   ___   __| |___ 
 |  __  |/ _ \ | '_ \ / _ \ '__| | |\/| |/ _ \ __| '_ \ / _ \ / _` / __|
 | |  | |  __/ | |_) |  __/ |    | |  | |  __/ |_| | | | (_) | (_| \__ \
 |_|  |_|\___|_| .__/ \___|_|    |_|  |_|\___|\__|_| |_|\___/ \__,_|___/
               | |                                                      
               |_|                                                      */

void initSDCard()
{
    //Finds the SD Card
    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

    //Directory of the word lists
    const char wordDir[] = "/";
    SD.begin();
    root = SD.open(wordDir);

    category = root.openNextFile();

    if (!category)
    { //end of the list
        Serial.println("No lists");
    }
}

void printToScreen(char toScreen[], int layer, int type)
{ //This function writes the charactor array to the screen. Type = 0 standard; Type = 1 category(removes .txt and \;

    //Cleans up the name
    if (type == 1)
    {
        toScreen = toScreen + 1;
        //toScreen[strlen(toScreen)-4] = '\0';//Ends the string before the extension
    }

    //Clears line
    for (int i = 1; i <= 16; ++i)
    {
        lcd.setCursor(i, layer);
        lcd.write(" ");
    }

    int j = 0; //word interator

    int startPos = (8 - (strlen(toScreen) / 2));

    for (int i = startPos; i < startPos + strlen(toScreen); ++i)
    {

        lcd.setCursor(i, layer);

        lcd.write(toScreen[j]);

        j++;
    }
}

void printTeamSymbols()
{
    lcd.setCursor(0, 0);
    lcd.write(byte(0));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
}

void printTeamScores()
{
    lcd.setCursor(0, 1);
    lcd.write(team1Score + '0');
    lcd.setCursor(15, 1);
    lcd.write(team2Score + '0');
}

void displayNextCategory()
{
    char displayCategories[] = "Category:";
    char phrase[17];

    printToScreen(displayCategories, 0, 0);
    int selCategory = 0;

    //Only goes in one direction
    category = root.openNextFile();

    if (!category)
    {                           //end of the list
        root.rewindDirectory(); //go to list start
        category = root.openNextFile();
    }

    char systemVolume[] = "/System Volume";
    boolean sysVolSame = true;
    for (int i = 0; i < strlen(systemVolume); ++i)
    {
        if (systemVolume[i] != category.name()[i])
        {
            sysVolSame = false;
            break;
        }
    }
    if (sysVolSame)
    { //skip system volume information
        Serial.println("System Volume Skipped");
        category = root.openNextFile();
    }

    if (!category)
    {                           //end of the list
        root.rewindDirectory(); //go to list start
        category = root.openNextFile();
    }

    printToScreen(strdup(category.name()), 1, 1);
    Serial.println(category.name());
}

int getNumWordsInFile(File file)
{
    int i = 0;
    while (file.available())
    {
        file.readStringUntil('\n');
        i++;
    }
    return i;
}

void fillRandomizedWordArrayFromFile(String randomizedWordArray[], File file, int size)
{
    file.seek(0);
    int i = 0;
    while (file.available())
    {
        randomizedWordArray[i] = file.readStringUntil('\n').c_str();
        i++;
    }
    shuffleArray(randomizedWordArray, size);
}

void shuffleArray(String arr[], int n)
{
    // Use a different seed value so that we don't get same
    // result each time we run this program
    randomSeed(analogRead(0));

    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (int i = 0; i < n; i++)
    {
        long j = random(0, n);
        String temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void displayNextWord(String wordList[], int size)
{
    lcd.clear();
    int index = currentWord % size;
    formmatedLCDDisplay(wordList[index]);
    Serial.println(wordList[index]);
    currentWord++;
    printTeamSymbols();
    printTeamScores();
}

void formmatedLCDDisplay(String word)
{
    if (word.length() <= 11)
    {
        char arr[word.length() + 1];
        word.toCharArray(arr, word.length() + 1);
        printToScreen(arr, 0, 0);
    }
    else
    {
        int indexOfSpace = 0;
        for (int i = 12; i > 0; i--)
        {
            if (word.charAt(i) == ' ')
            {
                indexOfSpace = i;
                break;
            }
        }

        int split1Size = indexOfSpace + 1;
        int split2Size = word.length() - indexOfSpace + 1;
        split2Size = min(split2Size, 12);        

        char arr[indexOfSpace + 1];
        word.substring(0, indexOfSpace).toCharArray(arr, indexOfSpace + 1);
        printToScreen(arr, 0, 0);

        char arr2[word.length() - indexOfSpace + 1];
        word.substring(indexOfSpace).toCharArray(arr, word.length() - indexOfSpace + 1);
        printToScreen(arr, 1, 0);
    }
}

void updateClock()
{
    portENTER_CRITICAL(&timerMux);
    secondCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalSecondCounter++;

    Serial.printf("%d seconds have elapsed.\n", totalSecondCounter);
}
