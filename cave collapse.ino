#include <LiquidCrystal.h>
#include "LedControl.h"
#include <EEPROM.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 11;
const byte d6 = 2;
const byte d7 = 4;
const byte LCDContrastPin = 3;
const byte LCDBrightnessPin = 5;

const byte buzzerPin = 13;

const byte dinPin = 12;



const byte clockPin = 6;
const byte loadPin = 10;
const byte matrixSize = 8;

// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte joyXPin = A1;
const byte joyYPin = A2;
const byte joySWPin = A0;

char githubLink[] = "https://github.com/AndreiConstantinDina/IntroductionToRobotics";
int githubLinkLen = 62;
char aboutInfo[] = "Cave collapse by Dina Andrei-Constantin";
int aboutInfoLen = 39;
char toPrint[17];

int aboutInfoPosition = 0;
bool aboutInfoChanged = true;

const byte MAX_DIFFICULTY = 3;
const byte MIN_DIFFICULTY = 1;

const byte MAX_LCD_BRIGHTNESS_LVL = 5;
const byte MIN_LCD_BRIGHTNESS_LVL = 1;

const byte MAX_MATRIX_BRIGHTNESS_LVL = 5;
const byte MIN_MATRIX_BRIGHTNESS_LVL = 1;

const byte MAX_LCD_CONTRAST_LVL = 5;
const byte MIN_LCD_CONTRAST_LVL = 1;

const byte highscoresStartingByte = 100;

const byte ON = 1;
const byte OFF = 0;

const int numberOfSettings = 8;

bool soundOn = true;
byte soundState = ON;
byte lastSoundState = OFF;

int LCDBrightnessLvl = 2;
int lastLCDBrightnessLvl = 0;

int LCDContrastLvl = 2;
int lastLCDContrastLvl = 0;


int MatrixBrightnessLvl = 3;
int MatrixBrightnessValue;
int lastMatrixBrightnessLvl = 0;

int difficultyLvl = 2;
int lastDifficultyLvl = 0;

int LCDContrastValue = 120;
int LCDBrightnessValue = 100;


const byte START = 0;
const byte DIFFICULTY = 1;
const byte LCD_BRIGHTNESS = 2;
const byte MATRIX_BRIGHTNESS = 3;
const byte SOUNDS = 4;
const byte ABOUT = 5;

const int LEFT = -1;
const int RIGHT = 1;
const int UP = 1;
const int DOWN = -1;
const int NEUTRAL = 0;
const int blinkingInterval = 60;
const uint64_t xShape =   0x8142241818244281;
const uint64_t emptyMatrix = 0;
const uint64_t fullMatrix = 0xffffffffffffffff;
const uint64_t trophyShape = 0x66bd18183c7e6642;
const uint64_t threeShape = 0x1824201818202418;
const uint64_t twoShape = 0x007c44040810243c;
const uint64_t oneShape = 0x007c101014181010;

struct scoreRecord {
  char name[4] = "ZZZ";
  int score;
} highScores[6];

int mapChangeInterval = 1000;


int gameMap[matrixSize][matrixSize] = {
 {0, 0, 0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0, 0, 0}, 
 {0, 0, 0, 0, 0, 0, 0, 0}
};

int lastObstacle[matrixSize] = {1, 1, 1, 0, 0, 1, 1, 1};

int playerRow = 4;
int playerCol = 4;
int startTime = 0;
int score = 0;

long long lastBlinkingChange = 0;
bool ledBlinkOff = true;
long long lastMapChange = 0;
long long lastScoreIncrease = 0;
long long scoreIncreaseInterval = 1000;

int valueOX, valueOY, valueSW;
int lastValueSW;
int lastValue;
int lastValueOY = NEUTRAL;
int lastValueOX = NEUTRAL;

const byte MENU = 0;
const byte GAME = 1;

const byte SETTINGS = 0;
const byte MODIFY_SETTINGS = 1;

byte currentMenuState = SETTINGS;

bool SWjustPressed = false;

bool programState = MENU;

int settingsState = START;

int scoreboardPosition = 0;
int scoreboardChanged = false;

char nameLetter1 = 'A';
char nameLetter2 = 'A';
char nameLetter3 = 'A';


void setup() {
  // set up the LCD's number of columns and rows:
  Serial.begin(9600);
  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);
  pinMode(joySWPin, INPUT_PULLUP);
  
  pinMode(LCDContrastPin, OUTPUT);
  pinMode(LCDBrightnessPin, OUTPUT);
  
  pinMode(buzzerPin, OUTPUT);

  difficultyLvl = EEPROM.read(0);
  LCDBrightnessLvl = EEPROM.read(4);
  LCDContrastLvl = EEPROM.read(8);
  MatrixBrightnessLvl = EEPROM.read(12);
  soundOn = EEPROM.read(20);
  for (int i=0; i<5; i++)
    {
      for (int j=0; j<4; j++)
        highScores[i].name[j] = EEPROM.read(highscoresStartingByte + i*5 + j);
      highScores[i].score = EEPROM.read(highscoresStartingByte + i*5 + 4);
    }


  LCDBrightnessValue = map (LCDBrightnessLvl, MIN_LCD_BRIGHTNESS_LVL, MAX_LCD_BRIGHTNESS_LVL, 100, 255);
  analogWrite(LCDBrightnessPin, LCDBrightnessValue);

  LCDContrastValue = map (LCDContrastLvl, MIN_LCD_CONTRAST_LVL, MAX_LCD_CONTRAST_LVL, 50, 150);
  analogWrite(LCDContrastPin, LCDContrastValue);

  MatrixBrightnessValue = map (MatrixBrightnessLvl, MIN_MATRIX_BRIGHTNESS_LVL, MAX_MATRIX_BRIGHTNESS_LVL, 0, 15);
  lc.setIntensity(0, MatrixBrightnessValue);

  lc.shutdown(0, false); // turn off power saving, enables display
  lc.clearDisplay(0);// clear screen

  analogWrite(LCDBrightnessPin, LCDBrightnessValue);

  analogWrite(LCDContrastPin, LCDContrastValue);
  lcd.begin(16, 2);
  // initialize the serial communications:
  lcd.write("Welcome, player");
   for (int row = 0; row < matrixSize; row++) {
      for (int col = 0; col < matrixSize; col++) {
        lc.setLed(0, row, col, true); // turns on LED at col, row
        delay(25);
      }
    }
  lcd.clear();
  
  writeMenuOnLCD();

}



void loop() {
// when characters arrive over the serial port...
  if (programState == MENU)
    menuState();
  else gameState();

  
}

void menuState(){
  
  valueSW = digitalRead(joySWPin);
  if (valueSW == LOW && lastValueSW == HIGH)
    {
      if (currentMenuState == SETTINGS)
            {
              SWjustPressed = true;
              currentMenuState = MODIFY_SETTINGS;
            }
      else {
        writeMenuOnLCD();
        currentMenuState = SETTINGS;
        
        }
    }
  lastValueSW = valueSW;

  if (currentMenuState == SETTINGS){
    valueOY = analogRead(joyYPin);
     

    if (valueOY < 200)
      valueOY = DOWN;
    else if (valueOY > 1000)
      valueOY = UP;
    else valueOY = NEUTRAL;

    if (lastValue != valueOY){
      
      if (soundOn)
        playSound();
      lastValue = valueOY;
      settingsState += valueOY;
      if (settingsState > numberOfSettings)
        settingsState = 0;
      if (settingsState < 0)
        settingsState = numberOfSettings;
      writeMenuOnLCD();
      lastDifficultyLvl = -1;

    }
    
  }

  else if (currentMenuState == MODIFY_SETTINGS){
      modifySettingsMenu();
  }
 
  

}

void gameState(){
  if (millis() - lastScoreIncrease > scoreIncreaseInterval){
     score+=difficultyLvl;
    lastScoreIncrease = millis();
  }
  if (gameMap[playerRow][playerCol] == 1)
    {
      programState = MENU;
      for (int row = 0; row < matrixSize; row++) {
          for (int col = 0; col < matrixSize; col++) {
          gameMap[row][col] = 0;
          lc.setLed(0, row, col, false); // turns off LED at col, row
          delay(25);
      }
      lastObstacle[row] = 1;
      }     
    lastObstacle[3] = lastObstacle[4] = 0;      
    playerRow = playerCol = 4;
    lcd.setCursor(0, 0);
    lcd.write("GAME OVER. Score: ");
    lcd.print(score);
    lcd.setCursor(0, 1);
    lcd.write("Press J3 to continue");
    for(int PositionCount=0; PositionCount<7; PositionCount++)
      {
        if (soundOn)
          digitalWrite(buzzerPin, HIGH);
        lcd.scrollDisplayLeft(); //builtin command to scroll right the text
        displayImage(xShape);
        digitalWrite(buzzerPin, LOW);
        delay(300);
        displayImage(emptyMatrix);
        delay(300);

        //game over

        
      }
      displayImage(fullMatrix);

      for (int i=0; i<5; i++)
          if (score > highScores[i].score){

            // new highscore

            // display message on matrix + lcd for 2 seconds
            displayImage(trophyShape);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.write("New highscore!");
            lcd.setCursor(0, 1);
            lcd.write("You're in top ");
            lcd.print(i+1);
            delay(2500);

            int valueSW = digitalRead(joySWPin);
            lcd.clear();
            newHighscoreMenuDisplay();
            int letterPosition = 0;
            while (valueSW == HIGH){
              // edit name for the player until J3 is pressed
              valueSW = digitalRead(joySWPin);
              valueOX = analogRead(joyXPin);
              valueOY = analogRead(joyYPin);
              bool updateName = false;

              if (valueOY < 200)
                valueOY = DOWN;
              else if (valueOY > 1000)
                valueOY = UP;
              else valueOY = NEUTRAL;

  
              if (lastValueOY != valueOY){
                if (valueOY == UP){
                  if (letterPosition == 0)
                      nameLetter1--;
                  else  if (letterPosition == 1)
                      nameLetter2--;

                  else  if (letterPosition == 2)
                      nameLetter3--;
                  updateName = true;
                  }
                  else if (valueOY == DOWN){
                    if (letterPosition == 0)
                      nameLetter1++;
                  else  if (letterPosition == 1)
                      nameLetter2++;

                  else  if (letterPosition == 2)
                      nameLetter3++;
                  updateName = true;
                }

                  lastValueOY = valueOY;

                  if (nameLetter1 < 'A')
                    nameLetter1 = 'A';
                  if (nameLetter1 > 'Z')
                    nameLetter1 = 'Z';

                  if (nameLetter2< 'A')
                    nameLetter2 = 'A';
                  if (nameLetter2 > 'Z')
                    nameLetter2 = 'Z';
                  
                  if (nameLetter3 < 'A')
                    nameLetter3 = 'A';
                  if (nameLetter3> 'Z')
                    nameLetter3 = 'Z';
  

                  if (updateName)
                    newHighscoreMenuDisplay();
                  
                }

              if (valueOX < 200)
                valueOX = RIGHT;
              else if (valueOX > 1000)
                valueOX = LEFT;
              else valueOX = NEUTRAL;

              if (lastValueOX != valueOX){
                if (valueOX == LEFT)
                  letterPosition--;
                else if (valueOX == RIGHT)
                  letterPosition++;
                if (letterPosition < 0)
                  letterPosition = 0;
                if (letterPosition > 2)
                  letterPosition = 2;
              lastValueOX = valueOX;
              
              }
                

            }
            // save new player
            char newName[4];
            newName[0] = nameLetter1;
            newName[1] = nameLetter2;
            newName[2] = nameLetter3;
            newName[3] = '\0';
            strcpy(highScores[5].name, newName);
            highScores[5].score = score;

            // rewrite the top 5
            sortHighscores();


            // save top 5 to EEPROM
             for (int i=0; i<5; i++)
                {
                  for (int j=0; j<4; j++)
                    EEPROM.update(highscoresStartingByte + i*5 + j, highScores[i].name[j]);
                  EEPROM.update(highscoresStartingByte + i*5 + 4, highScores[i].score);
                }
        
          displayImage(fullMatrix);
          writeMenuOnLCD();
          break;
          }

    }


  else{
    //map updating randomly once in a while
    if (millis() - lastMapChange > mapChangeInterval/(difficultyLvl * 0.8)){
      if (soundOn)
            playSound();
      for (int row = 0; row < matrixSize; row++) 
        for (int col = 0; col < matrixSize - 1; col++) 
            gameMap[row][col] = gameMap[row][col+1];

        for (int row = 0; row < matrixSize; row++)
          gameMap[row][matrixSize - 1] = lastObstacle[row];
        
        int numberOfShifts = 1;
        int shiftDirection = random(0, 2);

        if (lastObstacle[0] == 0) shiftDirection = 1;
        if (lastObstacle[matrixSize - 1] == 0) shiftDirection = 0;

        if (shiftDirection == 0){
          for (int i=1; i<=numberOfShifts; i++)
            {
              for (int row = 0; row < matrixSize - 1; row++)
                lastObstacle[row] = lastObstacle[row+1];
              lastObstacle[matrixSize - 1] = 1;
            }
        }

        else  for (int i=1; i<=numberOfShifts; i++)
            {
              for (int row = matrixSize - 1; row > 0; row--)
                lastObstacle[row] = lastObstacle[row-1];
              lastObstacle[0] = 1;
            }

            
        for (int row = 0; row < matrixSize; row++) {
          for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, gameMap[row][col]); // turns on LED at col, row
          }
        }

        lastMapChange = millis();

    }
    

    valueOX = analogRead(joyXPin);
    valueOY = analogRead(joyYPin);

    // Serial.print(valueOX);
    // Serial.print(" ");
    // Serial.print(valueOY);
    // Serial.println();

    if (valueOY < 200)
      valueOY = DOWN;
    else if (valueOY > 1000)
      valueOY = UP;
    else valueOY = NEUTRAL;

  
    if (lastValueOY != valueOY){
      lc.setLed(0, playerRow, playerCol, 0);
      if (valueOY == DOWN)
        playerRow--;
      if (valueOY == UP)
        playerRow++;

      lastValueOY = valueOY;
    }

    if (valueOX < 200)
      valueOX = RIGHT;
    else if (valueOX > 1000)
      valueOX = LEFT;
    else valueOX = NEUTRAL;

    if (lastValueOX != valueOX){
      lc.setLed(0, playerRow, playerCol, 0);
      if (valueOX == LEFT)
        playerCol--;
      if (valueOX == RIGHT)
        playerCol++;

      lastValueOX = valueOX;
    }


    if (playerCol > 5)
      playerCol = 5;
      if (playerCol < 0)
      playerCol = 0;
    if (playerRow > matrixSize - 1)
      playerRow = matrixSize - 1;
    if (playerRow < 0)
      playerRow = 0;


    // player blinking dot
    if (millis() - lastBlinkingChange > blinkingInterval)
      {
      if (ledBlinkOff)
        lc.setLed(0, playerRow, playerCol, 1);
      else
        lc.setLed(0, playerRow, playerCol, 0);
      ledBlinkOff = !ledBlinkOff;
      lastBlinkingChange = millis();
      }
    
      
  }

}

void modifySettingsMenu(){
  switch(settingsState){
    case(0):
     if (SWjustPressed){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("Dodge for your life");
        lcd.setCursor(0, 1);
        lcd.write("Move using joystick");
        lcd.scrollDisplayLeft();
        displayImage(threeShape);
        delay(1000);
        lcd.scrollDisplayLeft();
        displayImage(twoShape);
        delay(1000);
        lcd.scrollDisplayLeft();
        displayImage(oneShape);
        delay(1000);
        
        score = 0;
        programState = GAME;
        SWjustPressed = !SWjustPressed;
      }

      break;
    
    case(1):
      if (SWjustPressed){
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.write("DIFFICULTY");
        lcd.setCursor(7, 1);
        lcd.print(difficultyLvl);
        SWjustPressed = !SWjustPressed;
      }
      if (lastDifficultyLvl != difficultyLvl){
          lcd.clear();
          lcd.setCursor(3, 0);
          lcd.write("DIFFICULTY");
          lcd.setCursor(7, 1);
          lcd.print(difficultyLvl);
          lastDifficultyLvl = difficultyLvl;
      }
      valueOY = analogRead(joyYPin);

      if (valueOY < 200)
        valueOY = DOWN;
      else if (valueOY > 1000)
        valueOY = UP;
      else valueOY = NEUTRAL;

      if (lastValue != valueOY){
        lastValue = valueOY;
        difficultyLvl -= valueOY;
        if (difficultyLvl > MAX_DIFFICULTY)
          difficultyLvl = MAX_DIFFICULTY;
        if (difficultyLvl < MIN_DIFFICULTY)
          difficultyLvl = MIN_DIFFICULTY;

        EEPROM.update(0, difficultyLvl);
        if (soundOn)
          playSound();
      }
      break;
    
    case(2):
      if (SWjustPressed){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("BRIGHTNESS LVL");
        lcd.setCursor(7, 1);
        lcd.print(LCDBrightnessLvl);
        SWjustPressed = !SWjustPressed;
      }
      if (lastLCDBrightnessLvl != LCDBrightnessLvl){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("BRIGHTNESS LVL");
        lcd.setCursor(7, 1);
        lcd.print(LCDBrightnessLvl);
        lastLCDBrightnessLvl = LCDBrightnessLvl;
        LCDBrightnessValue = map (LCDBrightnessLvl, MIN_LCD_BRIGHTNESS_LVL, MAX_LCD_BRIGHTNESS_LVL, 100, 255);
        analogWrite(LCDBrightnessPin, LCDBrightnessValue);

      }
      valueOY = analogRead(joyYPin);

      if (valueOY < 200)
        valueOY = DOWN;
      else if (valueOY > 1000)
        valueOY = UP;
      else valueOY = NEUTRAL;

      if (lastValue != valueOY){
          
        lastValue = valueOY;
        LCDBrightnessLvl -= valueOY;
        if (LCDBrightnessLvl > MAX_LCD_BRIGHTNESS_LVL)
          LCDBrightnessLvl = MAX_LCD_BRIGHTNESS_LVL;
        if (LCDBrightnessLvl < MIN_LCD_BRIGHTNESS_LVL)
          LCDBrightnessLvl = MIN_LCD_BRIGHTNESS_LVL;
          if (soundOn)
            playSound();
      }
      EEPROM.update(4, LCDBrightnessLvl);

      break;
    
    case(3):
     if (SWjustPressed){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("CONTRAST LVL");
        lcd.setCursor(7, 1);
        lcd.print(LCDContrastLvl);
        SWjustPressed = !SWjustPressed;
      }
      if (lastLCDContrastLvl != LCDContrastLvl){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("CONTRAST LVL");
        lcd.setCursor(7, 1);
        lcd.print(LCDContrastLvl);
        lastLCDContrastLvl = LCDContrastLvl;
        LCDContrastValue = map (LCDContrastLvl, MIN_LCD_CONTRAST_LVL, MAX_LCD_CONTRAST_LVL, 50, 150);
        analogWrite(LCDContrastPin, LCDContrastValue);

      }
      valueOY = analogRead(joyYPin);

      if (valueOY < 200)
        valueOY = DOWN;
      else if (valueOY > 1000)
        valueOY = UP;
      else valueOY = NEUTRAL;

      if (lastValue != valueOY){
          
        lastValue = valueOY;
        LCDContrastLvl -= valueOY;
        if (LCDContrastLvl > MAX_LCD_CONTRAST_LVL)
          LCDContrastLvl = MAX_LCD_CONTRAST_LVL;
        if (LCDContrastLvl < MIN_LCD_CONTRAST_LVL)
          LCDContrastLvl = MIN_LCD_CONTRAST_LVL;
          if (soundOn)
            playSound();
      }
      EEPROM.update(8, LCDContrastLvl);

      break;

    case(4):
      if (SWjustPressed){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("BRIGHTNESS LVL");
        lcd.setCursor(7, 1);
        lcd.print(MatrixBrightnessLvl);
        SWjustPressed = !SWjustPressed;
      }
      if (lastMatrixBrightnessLvl != MatrixBrightnessLvl){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("BRIGHTNESS LVL");
        lcd.setCursor(7, 1);
        lcd.print(MatrixBrightnessLvl);
        lastMatrixBrightnessLvl = MatrixBrightnessLvl;
        MatrixBrightnessValue = map (MatrixBrightnessLvl, MIN_MATRIX_BRIGHTNESS_LVL, MAX_MATRIX_BRIGHTNESS_LVL, 0, 15);
        lc.setIntensity(0, MatrixBrightnessValue);

      }
      valueOY = analogRead(joyYPin);

      if (valueOY < 200)
        valueOY = DOWN;
      else if (valueOY > 1000)
        valueOY = UP;
      else valueOY = NEUTRAL;

      if (lastValue != valueOY){
          
        lastValue = valueOY;
        MatrixBrightnessLvl -= valueOY;
        if (MatrixBrightnessLvl > MAX_MATRIX_BRIGHTNESS_LVL)
          MatrixBrightnessLvl = MAX_MATRIX_BRIGHTNESS_LVL;
        if (MatrixBrightnessLvl < MIN_MATRIX_BRIGHTNESS_LVL)
          MatrixBrightnessLvl = MIN_MATRIX_BRIGHTNESS_LVL;
          if (soundOn)
            playSound();
      }
      EEPROM.update(12, MatrixBrightnessLvl);

      break;
    
    case(5):
      if (SWjustPressed){
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.write("SOUND");
        lcd.setCursor(7, 1);
        if (soundOn)
          lcd.write("ON");
        else lcd.write("OFF");
        SWjustPressed = !SWjustPressed;
      }
      if (lastSoundState != soundState){
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.write("SOUND");
        lcd.setCursor(7, 1);
        if (soundOn)
          lcd.write("ON");
        else lcd.write("OFF");
        lastSoundState = soundState;
      }
      valueOY = analogRead(joyYPin);

      if (valueOY < 200)
        valueOY = DOWN;
      else if (valueOY > 1000)
        valueOY = UP;
      else valueOY = NEUTRAL;

      if (lastValue != valueOY)
        {
          if (valueOY != NEUTRAL){
          lastValue = valueOY;
          soundOn = !soundOn;
          if (soundOn)
            soundState = ON;
          else soundState = OFF;
          if (soundOn)
            playSound();
          }

        }
      EEPROM.update(20, soundOn);

      break;
    
    case(6):
      // About settings

      valueOX = analogRead(joyXPin);
      if (valueOX < 200)
        valueOX = LEFT;
      else if (valueOX > 1000)
        valueOX = RIGHT;
      else valueOX = NEUTRAL;

      if (lastValue != valueOX){
          
        lastValue = valueOX;
        if (valueOX == RIGHT)
          aboutInfoPosition--;
        else if (valueOX == LEFT)
          aboutInfoPosition++;
        aboutInfoChanged = true;

        if (aboutInfoPosition > githubLinkLen)
          aboutInfoPosition = 0;
        if (aboutInfoPosition < 0)
          aboutInfoPosition = githubLinkLen; 
      }
      if (aboutInfoChanged){
        lcd.clear();
        if (aboutInfoPosition < aboutInfoLen){
            lcd.setCursor(0, 0);
            strncpy(toPrint, aboutInfo + aboutInfoPosition, 16);
            lcd.print(toPrint);
        }
        lcd.setCursor(0, 1);
        strncpy(toPrint, githubLink + aboutInfoPosition, 16);
        lcd.print(toPrint);
      }

      
      aboutInfoChanged = false;


      break;

    case (7):
      // Highscores


      valueOY = analogRead(joyYPin);
      scoreboardChanged = false;
      if (valueOY < 200)
        valueOY = DOWN;
      else if (valueOY > 1000)
        valueOY = UP;
      else valueOY = NEUTRAL;

      if (lastValue != valueOY){
         if (soundOn)
        playSound();
        lastValue = valueOY;
        scoreboardPosition += valueOY;
        if (scoreboardPosition > 4)
          scoreboardPosition = 0;
        if (scoreboardPosition < 0)
          scoreboardPosition = 4;
        scoreboardChanged = true;

        }

        if (scoreboardChanged || SWjustPressed){
          // display player
          lcd.clear();
          lcd.setCursor(0, 0);
          for (int i=0; i<3; i++)
            lcd.print(highScores[scoreboardPosition].name[i]);
          lcd.print(" ");
          lcd.print(highScores[scoreboardPosition].score);

          int nextPlayer = scoreboardPosition + 1;
          if (nextPlayer > 4)
            nextPlayer = 0;
          if (nextPlayer < 0)
           nextPlayer = 4;

          lcd.setCursor(0, 1);
          for (int i=0; i<3; i++)
            lcd.print(highScores[nextPlayer].name[i]);
          lcd.print(" ");
          lcd.print(highScores[nextPlayer].score);

          if (SWjustPressed)
            SWjustPressed = false;

        }

      break;
    case(8):
      if (SWjustPressed){
        SWjustPressed = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write("Dodge obstacles");
        lcd.setCursor(0, 1);
        lcd.write("Use joy to move");
        }
      break;
    default:
      lcd.clear();
  }
}

void writeMenuOnLCD(){
  lcd.clear();
  switch(settingsState){
    case(0):
      lcd.setCursor(0, 0);
      lcd.write("START");
      lcd.setCursor(0, 1);
      lcd.write("DIFFICULTY");
      break;
    
    case(1):
      lcd.setCursor(0, 0);
      lcd.write("DIFFICULTY");
      lcd.setCursor(0, 1);
      lcd.write("LCD BRIGHTNESS");
      break;
    
    case(2):
      lcd.setCursor(0, 0);
      lcd.write("LCD BRIGHTNESS");
      lcd.setCursor(0, 1);
      lcd.write("LCD CONTRAST");
      break;
    case(3):
      lcd.setCursor(0, 0);
      lcd.write("LCD CONTRAST");
      lcd.setCursor(0, 1);
      lcd.write("MATR BRIGHTNESS");
      break;
    case(4):
      lcd.setCursor(0, 0);
      lcd.write("MATR BRIGHTNESS");
      lcd.setCursor(0, 1);
      lcd.write("SOUND");
      break;
    
    case(5):
      lcd.setCursor(0, 0);
      lcd.write("SOUND");
      lcd.setCursor(0, 1);
      lcd.write("ABOUT");
      break;
    
    case(6):
      lcd.setCursor(0, 0);
      lcd.write("ABOUT");
      lcd.setCursor(0, 1);
      lcd.write("HIGHSCORES");
      break;
    
    case(7):
      lcd.setCursor(0, 0);
      lcd.write("HIGHSCORES");
      lcd.setCursor(0, 1);
      lcd.write("HOW TO PLAY");
      break;
    case(8):
      lcd.setCursor(0, 0);
      lcd.write("HOW TO PLAY");
      lcd.setCursor(0, 1);
      lcd.write("START");
      break;
    default:
      lcd.clear();
  }


}

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
}

void playSound(){
  digitalWrite(buzzerPin, HIGH);
  delay(3);
  digitalWrite(buzzerPin, LOW);

}


void newHighscoreMenuDisplay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("NAME: ");
  
  lcd.print(nameLetter1);
  lcd.print(nameLetter2);
  lcd.print(nameLetter3);

  lcd.setCursor(0, 1);
  lcd.print(score);
}

void sortHighscores(){
  for (int i=0; i<5; i++)
    for (int j=i+1; j<6; j++)
      if (highScores[i].score < highScores[j].score){
        char aux[3];
        strcpy(aux, highScores[i].name);
        strcpy(highScores[i].name, highScores[j].name);
        strcpy(highScores[j].name, aux);
        int auxScore = highScores[i].score;
        highScores[i].score = highScores[j].score;
        highScores[j].score = auxScore;
      }
}