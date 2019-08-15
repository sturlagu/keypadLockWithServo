#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

// Maskinvareoppsett: 5V-Micro-servo, 5V-LCD skjerm, Keypad 4x4, og en Arduino UNO mikrokontroller
// Programvaren gir et brukergrensesnitt med muligheten til å: logge inn med passord, endre passord,
// tekstredigering som å slette siste tall og slette alle tall, åpne servo ved innlogging, og lukke servo ved utlogging

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(13, 12, 3, 2, A4, A5);

// shift keystroke to right
int shift = 0;
// size of keypad 4x4
const byte rows = 4;
const byte cols = 4;

// Keypad layout
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Pins on arduino board for keypad
byte rowPins[rows] = {11, 10, 9, 8};
byte colPins[cols] = {7, 6, 5, 4};

// Password storage
char password[15] = {"1234"};

// temporary keystroke storage
char tempKeys[15] = {};

boolean login = false;

// Create keypad class
Keypad pad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// Create servo class
Servo myServo;
// Servo variabler
int pos = 0;
int servoPin = A0;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready!");
  // attach servo to board pin
  myServo.attach(servoPin);
  // first time reset position
  myServo.write(pos);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD
  lcd.print("Passord:");
}

void loop() {
  // Get char input
  char keyPressed = pad.getKey();
  if(keyPressed){
    if(keyPressed == 'A'){
      if(login == false){
        checkPassword();      
      }else{
        logOut();
        login = false; 
      }
    }else if(keyPressed == 'B'){
      clearLast();
    }else if(keyPressed == 'C'){
      clearScreen(1);
    }else if(keyPressed == 'D'){
      changePassword();
    }else if(shift < 16){
      lcd.setCursor(shift, 1);
      lcd.write(keyPressed);
      tempKeys[shift] = keyPressed;
      shift++;
    }
  }
}
// Validerer passord ved innlogging
void checkPassword(){
  if(strcmp(tempKeys, password) == 0){
    lcd.setCursor(0,1);
    lcd.write("Riktig passord!");
    delay(1500 );
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Steng med tast A");
    login = true;
    controllServo(false);
  }else{
    lcd.setCursor(0,1);
    lcd.write("Feil passord");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Passord:");
    shift = 0;
    memset(tempKeys, 0, sizeof tempKeys); 
  }  
}
// Logg ut
void logOut(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Passord:");
  shift = 0;
  memset(tempKeys, 0, sizeof tempKeys);
  controllServo(true);
}
// Bytte passord hvis innlogget
void changePassword(){
  if(login == true){
    memset(tempKeys, 0, sizeof tempKeys);
    shift = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    boolean changing = true;
    lcd.print("Endre Passord:");
    while(changing == true){
      char keyPressed = pad.getKey();
      if(shift < 16){
        if(keyPressed == 'A'){
          strcpy(password, tempKeys);
          lcd.clear();
          lcd.write("Passord endret!");
          delay(3000);
          lcd.clear();
          lcd.write("Steng med tast A");
          changing = false;    
        }else if(keyPressed == 'B'){
          clearLast();
        }else if(keyPressed == 'C'){
          clearScreen(1);
        }else if(keyPressed){
          lcd.setCursor(shift, 1);
          lcd.write(keyPressed);
          tempKeys[shift] = keyPressed;
          shift++;
        }
      }else if(shift == 16){
        clearScreen(0);
        lcd.setCursor(0,0);
        lcd.write("Maks Lengde!");
        delay(3000);
        clearScreen(0);
        lcd.setCursor(0,0);
        lcd.write("Endre passord:");
        shift--;
      }
    }   
  }
}
// Kontrollerer servo. Enten lukke eller åpne
void controllServo(boolean lock){
  if(lock == false){
    for(pos = 0; pos <= 180; pos++){
      myServo.write(pos);
      delay(15);
    }
  }else if(lock == true){
    for(pos = 180; pos >= 0; pos--){
      myServo.write(pos);
      delay(15);
    }
  }
}
// Sletter øverste eller nederste rad på skjermen
void clearScreen(int row){
    lcd.setCursor(0,row);
    for(int i = 0; i < 16; i++){
      lcd.write(" ");
    }
    if(row == 1){
      shift = 0;
      memset(tempKeys, 0, sizeof tempKeys);
    }
}
// Sletter siste tall skrevet inn
void clearLast(){
  if(shift > 0 && shift < 16){
    shift--;
    lcd.setCursor(shift, 1);
    lcd.write(' ');
    tempKeys[shift] = ' ';
  }
}
