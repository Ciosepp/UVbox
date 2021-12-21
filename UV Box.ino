#include  <LiquidCrystal.h>
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define buzzerPin 3
#define UvPin   12

#define p1Pin    11
#define p2Pin    10
#define potPin   17

#define minTime  10
#define maxTime 120

#define pressed false
int readTime;
bool readPlaySw;
bool readModeSw;

bool isTimerModeEnabled = true;
bool isExposing = false;
bool statusMem = isExposing;

void setup(){
  lcd.begin(8, 2);
  pinInit();
}

void loop(){

  getInputs();
  logic();
  outProcess();

}

void pinInit(){
  pinMode(p1Pin, INPUT_PULLUP);
  pinMode(p2Pin, INPUT_PULLUP);
  pinMode(UvPin, OUTPUT);
}

void getInputs(){

  readTime = analogRead(potPin);

  readPlaySw = digitalRead(p1Pin);
  readModeSw = digitalRead(p2Pin);
}
/*
bool readPlaySw;
bool readModeSw;

bool isTimerModeEnabled = true;
bool isExposing = false;
*/
bool latch =false;
bool out= false;
void logic(){

  if(!isExposing && readPlaySw == pressed){
    isExposing =true;
    startTimer(map(readTime, 0, 1023, minTime, maxTime)); //start time
  }
  if((isExposing && !timerOut()) || readPlaySw == pressed){  //stop light
    isExposing = false;
  }

  if(readModeSw == pressed && !latch){  //switching mode if not exposing

    isTimerModeEnabled = !isTimerModeEnabled;
    isExposing = false;
    latch = true;
  }
  if(readModeSw == pressed && latch){   //reset button latch
    latch =false;
  }

}

bool oldExp =     false;
bool oldTimerMode = false;
void outProcess(){

  if((isExposing || isTimerModeEnabled) ^ statusMem){ //isExposing edge detection

    digitalWrite(UvPin, isExposing);
    statusMem = isExposing;
  }

  if(!isTimerModeEnabled){

    if((isExposing ^ oldExp)&& !isExposing){ //transition into NOT-exposing state
      homeScreen();
      oldExp = isExposing;
    }
    if(!isExposing){    //time setting state
      updateHomeScreen();
    }
    if((isExposing ^ oldExp)&& isExposing){ //transition into exposing state
      timerScreen();
      oldExp = isExposing;
    }
    if(isExposing){
      updateTimerScreen();
    }
  }
  else{
    modeScreen();

  }
}


long tBuffer=0;
void startTimer(long timeMs){
  tBuffer = timeMs + millis(); 
}

bool timerOut(){
  if(millis() <= tBuffer){
    return true;
  }
  else 
    return false;
}

int oldTime=0,oldVal=0;
void homeScreen(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SET TIME");
  lcd.setCursor(0,1);
  lcd.print(": ");
  lcd.print(map(readTime, 0, 1023, minTime, maxTime));
  lcd.setCursor(7,1);
  lcd.print("s");

}
void updateHomeScreen(){

  int Read= map(readTime, 0, 1023, minTime, maxTime);
  if(oldVal =! Read){

    lcd.setCursor(2,1);
    lcd.print("   ");
    lcd.setCursor(2,1);
    lcd.print(map(readTime, 0, 1023, minTime, maxTime));
    oldVal = Read;
  }
}

void timerScreen(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ETA:");  
}

void updateTimerScreen(){
  int remainingTime = abs( tBuffer-millis()/1000);

  if(oldTime =! remainingTime){
    lcd.setCursor(0,1);
    lcd.print(remainingTime);
    lcd.setCursor(7,1);
    lcd.print("s");
    oldTime = remainingTime;
  }
}

void modeScreen(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  timer");
  lcd.setCursor(0,1);
  lcd.print("disabled");
}