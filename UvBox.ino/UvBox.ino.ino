#include  <LiquidCrystal.h>
const int rs = 9, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define NOTE_CS3 139
#define NOTE_F3  175
#define NOTE_GS3 208
#define NOTE_CS4 277

#define buzzerPin 3
#define UvPin   12

#define p1Pin    11
#define p2Pin    10
#define potPin   17

#define minTime  10
#define maxTime 300

#define pressed false
#define epsilon 2
bool isExposing = false;

long tBuffer=0;

void Init(){
  lcd.setCursor(0,0);
  lcd.print("UV EXPUSURE BOX");
  lcd.setCursor(1,5);
  lcd.print("by gAz");
  tone(buzzerPin,NOTE_CS3);
  delay(200);
  tone(buzzerPin,NOTE_F3);
  delay(200);
  tone(buzzerPin,NOTE_GS3);
  delay(200);
  tone(buzzerPin,NOTE_CS4);
  delay(200);
  noTone(buzzerPin);
  lcd.clear();
}

void pinInit(){
  pinMode(p1Pin, INPUT_PULLUP);
  pinMode(p2Pin, INPUT_PULLUP);
  pinMode(UvPin, OUTPUT);
}

bool trigged = false;
int readTime=0;
bool readPlaySw, readStopSw;
void getInputs(){

  readTime = analogRead(potPin);

  readPlaySw = digitalRead(p1Pin);
  readStopSw = digitalRead(p2Pin);
  // Serial.print(readPlaySw);
  // Serial.print("  ");
  // Serial.print(readPlaySw);
  // Serial.println("");
}


bool latch = false;

bool isTimerOn = false;
void logic(){

  if(!isExposing && readPlaySw==pressed && latch == false){

    isExposing =true;
    latch = true;
    Serial.println("exposing");

    startTimer(map(readTime, 0, 1000, minTime, maxTime)); //start time
  }

  if((isExposing && timerOut()==false) || readStopSw==pressed && latch==false){  //stop light
    Serial.println("stop exposing");
    isExposing = false;
    latch = true;
  }
  if(latch && readPlaySw==!pressed &&readPlaySw==!pressed){
    latch = false;
  }

}

bool oldExposingState =false;
void outProcess(){

  if(oldExposingState != isExposing &&isExposing == false){
    printHomeStatic();
    oldExposingState = isExposing;
    digitalWrite(UvPin,0);
    Serial.println("print home");
    for(int i=0; i<3; i++){
      tone(buzzerPin,NOTE_CS4);
      delay(200);
      noTone(buzzerPin);
    }
  }
  if(oldExposingState != isExposing &&isExposing == true){
    printExposingStatic();
    oldExposingState = isExposing;
    digitalWrite(UvPin,1);
    Serial.println("print exp");
    tone(buzzerPin,NOTE_CS4);
    delay(200);
    noTone(buzzerPin);
  }

  if(isExposing == false){
    updateHome();
  }
  else{
    updateExposing();
  }
}

void printHomeStatic(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   SET  TIME");
  lcd.setCursor(10,1);
  lcd.print("s");
  lcd.setCursor(4,1);
  lcd.print("      ");
  lcd.setCursor(4,1);
  lcd.print(map(readTime, 0, 1023, minTime, maxTime));
}
void printExposingStatic(){
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   EXPOSING");
  lcd.setCursor(0,1);
  lcd.print("ETA");
  lcd.setCursor(10,1);
  lcd.print("s");
}
void updateHome(){

  if(isChangedEp(readTime)){

    lcd.setCursor(4,1);
    lcd.print("      ");
    lcd.setCursor(4,1);
    lcd.print(map(readTime, 0, 1023, minTime, maxTime));
    
  }
}
void updateExposing(){

  if(isChanged( (tBuffer - millis())/1000)){

    lcd.setCursor(4,1);
    lcd.print("      ");
    lcd.setCursor(4,1);
    lcd.print( (tBuffer - millis())/1000);
    
  }
}
int oldState =0;
bool isChangedEp(int in){

  if(abs(oldState - in)>epsilon){
    oldState = in;
    return true;
  }
  return false;
}
bool isChanged(int in){

  if(oldState != in){
    oldState = in;
    return true;
  }
  return false;
}


void startTimer(long timeMs){
  tBuffer = timeMs*1000 + millis(); 
}


bool timerOut(){
  if(millis() <= tBuffer){
    return true;
  }
  else 
    return false;
}

void setup(){
  Serial.begin(115200);
  lcd.begin(16, 2);
  pinInit();
  Init();
  printHomeStatic();
  
}

void loop(){

  getInputs();
  logic();
  outProcess();
}    
