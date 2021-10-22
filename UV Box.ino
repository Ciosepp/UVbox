#include <LiquidCrystal.h>
#include <Timer.h>
#define EXPOSING true
#define PRESSED false
#define CLOSED false

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup(){
	lcd.begin(16,2);
	lcd.setCursor(0,0);
	lcd.print("UV exposure box");
	lcd.setCursor(2,1);
	lcd.print("Version 1.0");
	//setup output pins
	//setup lcd
	//setup HMI 
}

double delayTime = 0.1;		//[ms]
double remainingTime = 0;	//[ms]

bool IS_EXPOSING = false;

int intensity = 255;		//0 -> 255

int selection = 0;			//0=time 1= intensity

bool IS_PAUSE = FALSE;

void loop(){

	if(IS_EXPOSING == !EXPOSING){					//parameters' setting
		delayTime = updateTime(delayTime, selection);
		intensity = updateIntensity(intensity, selection);
	}

	if(digitalRead(lidSwitchPin)==CLOSED && digitalRead(startButtonPin) && IS_EXPOSING == !EXPOSING){	//start exposing

		timer.start(delayTime);	//start timer
		setUV(intensity);		//enable UV lights at intensity level
		IS_EXPOSING = EXPOSING;	//change state value to exposing
	}

	if(digitalRead(lidSwitchPin) == !CLOSED && IS_EXPOSING == !EXPOSING && !IS_PAUSE){		//lid opened while exposure, exp pauses

		timer.pause();
		setUV(0);		//disble UV lights
		IS_PAUSE =true;
	}

	if(digitalRead(lidSwitchPin) == CLOSED && IS_EXPOSING == !EXPOSING && IS_PAUSE){		//lid closed, exp resumeds

		timer.resume();
		setUV(intensity);		//disble UV lights
		IS_PAUSE =false;
	}

	if(timer.done()){

		setUV(0);		//disble UV lights	
		IS_EXPOSING = !EXPOSING;	//change state value to not exposing	
	}


	diplay(IS_EXPOSING);

}

void setUV(int pwr){
	analogWrite(uvPin, pwr);
}

void diplay(bool state);

double updateTime(double delayTime);

int updateIntensity(int intensity);