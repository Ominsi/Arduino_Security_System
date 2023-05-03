/*
 * Security System
 * Kienen Mulloy
 * Arduino Board
 */

#include <LiquidCrystal.h> 
#include <Keypad.h> 

// LCD Screen Pins
const int rsPin = 2;
const int enablePin = 3;
const int d0 = 4;
const int d1 = 5;
const int d2 = 6;
const int d3 = 7;
const int d4 = 8;
const int d5 = 9;
const int d6 = 10;
const int d7 = 11;

// LCD Object Created 
LiquidCrystal lcd(rsPin, enablePin, d0, d1, d2, d3, d4, d5, d6, d7);


// Buzzer Pin
const int buzzerPin = 13;

// Motion Sensor Pin
const int motionSensorPin = 22;

// Membrane Keypad Variable Setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
byte rowPins[ROWS] = {23, 25, 27, 29};
byte colPins[COLS] = {31, 33, 35, 37};

// Keypad Object Created
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Buzzer Frequencies
const int GRACEFREQ = 50; // Freq during alarm grace period
const int ALARMFREQ = 440; // Freq after alarm grace period

// Times 
int TIMEOUTTIME = 10000; // Time before input times out
int GRACETIME = 30000; // Time before alarm grace period ends
int MSGLEN = 1000;

// Security Codes
String secCode = "0000";
String masterCode = "BBBB";

// Other Variables
bool isArmed = false;  // Is the system armed
bool motionDetected = false; // Has motion been detected
bool graceEnded = false; // Has the grace period ended
unsigned long alarmTime; // When was motion detected

void setup() {

  // Setup pins and serial
  pinMode(buzzerPin, OUTPUT);
  pinMode(motionSensorPin, INPUT);
  Serial.begin(9600);

  // Set default view of display
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Disarmed");
}

// Main Loop
void loop() {

  if (isArmed) {
    // When the sensor goes off for the first time since being armed set motionDetected to true, set the alarmTime, and start the buzzer at the lower tone.
    if (digitalRead(motionSensorPin) == HIGH && !motionDetected) {
      motionDetected = true;
      alarmTime = millis();
      tone(buzzerPin, GRACEFREQ);
    // When the sensor is not going off for first time check if motion has been detected, the grace period has not ended, and the time since the alarm went off is long than the grace time. 
    // Set graceEnded, write to the Serial so the esp board handles emailing and change the tone to the higher pitch.
    } else if (motionDetected && !graceEnded && millis() - alarmTime >= GRACETIME) {
      graceEnded = true;
      Serial.write(graceEnded);
      tone(buzzerPin, ALARMFREQ);
    }
  }

  // Section handling input during loop
  lcd.setCursor(0,1);
  String code;
  char key = keypad.getKey(); // Get the key input into keypad
  
  // Done once a key is pressed
  if(key) {
    unsigned long startTime = millis(); // Set startTime of input being entered
    code = getCode(key, startTime); // Call getCode with the original key pushed and the startTime
    
    // When getCode does not timeout from reaching time limit
    if(code != ""){ 
      char choice = getChoice(startTime); // Call getChoice with startTime

      // When getChoice does not timeout from reaching time limit.
      if (choice != '\0') {

        // Check that code passed is either the security code or master code and if so call handleChoice
        if (code.equals(secCode) || code.equals(masterCode)) {
          handleChoice(choice);
        
        // If code do not match send message saying so for a length of time then call resetDisplay
        } else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Incorrect Code");
          delay(MSGLEN);
          resetDisplay();
        }
      
      // When getChoice does timeout call resetDisplay
      } else {
        resetDisplay();
      }

    // When getCode does timeout call resetDisplay
    } else {
      resetDisplay();
    }
  }
}

/*
 * Function to get code from input. Code is a string of four characters. The function is passed a key and a startTime. 
 * When the time getting the code is longer than the constant TIMEOUTTIME it returns "" otherwise it returns the code inputted.
 */
String getCode(char key, unsigned long startTime) {
  bool timeout = false; 
  String code;

  // When a key is passed add it to code and print it on the display
  if (key != '\0') {
    lcd.print(key);
    code += key;    
  }

  // While the code is not four characters and the time since startTime is less than constantTIMEOUTTIME get the key and
  // if it exist add it to the code.
  while (code.length() < 4 && millis() - startTime <= TIMEOUTTIME) {
    char key = keypad.getKey();

    if(key) {
      lcd.print(key);
      code += key;
    }
  }

  // If after exiting the loop the code is of at least length 4 return the code, otherwise return the emtpty string.
  if(code.length() >= 4) {
    return code;
  } else {
    return "";
  }
}

/**
 * Function to get operation choice from input. The choice is a single character. The function is passed a startTime. 
 * while the time since the startTime is less than the constant TIMEOUTTIME it gets the keys until it is either 'A' 'C' or 'D'.
 * It returns the choice of if it timesout returns /0.
 */
char getChoice(unsigned long startTime) {
  while(millis() - startTime <= TIMEOUTTIME) {
    char key = keypad.getKey();

    if(key) {
      if (key == 'A' || key == 'C' || key == 'D') {
        return key;
      }
    }
  }
  return '\0';
}

/** Handles the pass choice. Takes a character choice as an input.
 * It checks to make sure the choice is valid for the current armed state and if it is handles it.
 * Otherwise it shows a message.
 */
void handleChoice(char choice) {
  // If the system is Armed check if the choice is A or C. If it is A then call changeArmState to change to armed.
  // If C then call changeCode to handle changing the code.
  if (!isArmed) {
    if(choice == 'A') {
      changeArmState();
    }
    else if (choice == 'C') {
      changeCode();
    }

    // If it is not A or C then show message saying choice was invalid before calling resetDisplay;
    else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Invalid Choice");
      delay(MSGLEN);
      resetDisplay();
    }
  }  

  // If the system is armed, check if input is D. If so also check if the grace time is over.
  // If it is over than ask for the master code and call getCode with a new startTime. 
  // If the correct master code is put in after grace time or if grace time did not start then disarm with changeArmState
  // Otherwise if the incorrect master code is put in give a message before calling resetDisplay or if it timesout call resetDisplay.
  else {
    if (choice == 'D') {
      if (graceEnded) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Type Master Code");
        long startTime = millis();
        lcd.setCursor(0,1);
        String code = getCode('\0', startTime);
        if (code.equals(masterCode)) {
          changeArmState();
        } else if(code != "") {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Incorrect Master");
          delay(MSGLEN);
          resetDisplay();
        } else {
          resetDisplay();
        }
      } else {
        changeArmState();
      }
    }
  }
}

/**
 * Handles the changing from armed to disarmed.
 */
void changeArmState() {
  isArmed = !isArmed; //Swaps isArmed
  
  // If motion was detected than reset motionDetected and graceEnded as well as stop the buzzer
  if (motionDetected) {
    motionDetected = false;
    graceEnded = false;
    noTone(buzzerPin);
  }
  resetDisplay();
}

/** 
 * Handles creating a new code. Ask for a code and then asks for it again. If they match changes the code.
 */
void changeCode() {
  long startTime = millis(); //Set startTime for inputs
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Type New Code");
  lcd.setCursor(0, 1);
  String newCode = getCode('\0', startTime); //Get first code
  
  // If code is given then get another code
  if (newCode != "") { 
    long startTime = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Retype New Code");
    lcd.setCursor(0, 1);
    String checkCode = getCode('\0', startTime);

    //If codes match then set the secCode to the newCode and set message saying the code was changed.
    if(checkCode.equals(newCode)) {
      secCode = newCode;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Code Changed");
      delay(MSGLEN);
      resetDisplay();

    // If checkCode was received but don't match send incorrect code message then reset display.
    } else if (checkCode != "") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Incorrect Code");
      delay(MSGLEN);
     resetDisplay();
     // If getCode timed out either time then return resetDisplay
    } else {
      resetDisplay(); // For checkCode
    }
  } else {
    resetDisplay(); // For newCode
  }
}

/**
 * Resets the display with Disarmed or Armed on first line depending on current state.
 */
void resetDisplay() {
  lcd.clear();
  lcd.setCursor(0,0);
  if (isArmed) {
    lcd.print("Armed");
  } else {
    lcd.print("Disarmed");
  }
}