
/*
Yet Another ATTiny-based Jar of Fireflies
Joe FitzPatrick, @securelyfitz, joefitz@securinghardware.com

Software, hardware, and build notes at https://git.io/fireflies

Arduino sketch for ATTiny45+ based jar of fireflies. 6 charlieplexed LEDs plus a capacitive button
Based very heavily on Jason Webb's jar of fireflies:

todos:
--be more consistent with globals vs. passed values
--clean up state change code
--fix 1 vs 2 led option
--switch power modes when in state 0 to prolong standby battery life https://learn.sparkfun.com/tutorials/h2ohno/low-power-attiny
--turn off ADC to save power

Jason Webb - zen.webb@gmail.com
Project wiki: http://jason-webb.info/wiki/index.php?title=Jar_of_Fireflies
Github repo: https://github.com/jasonwebb/Jar-of-Fireflies
*/

#define F_CPU 1000000

#include <CapacitiveSensor.h>

// Pin assignments
const byte pinA = 0;
const byte pinB = 1;
const byte pinC = 2;

//i'm lazy so i use a ton of global variables to track state
int currled=0;
//int numleds=1;
int count=0;
int currstate=0;
int startmillis=0;
int currmillis=0;
int flashmillis=0;
int delaymillis=0;
bool pressed=false;

CapacitiveSensor   cs_3_4 = CapacitiveSensor(3,4);        // 10M resistor between pins 3 & 4, pin 4 is sensor pin, add a wire and or foil if desired

void setup() {
  // Flush any stray current from the ports
  turnOffAll();
  // flash each LED to show they work and so we know we just put in a good battery
  for(int j=0; j<6; j++) {
    turnOn(j);
    delay(250);
  }
/*  numleds=2;
  for(int j=0; j<6; j++) {
    turnOn(j);
    delay(250);
  }
  numleds=1;
*/
  turnOffAll();
}

void loop() {    
  //debounce capacitive sensor
  if(pressed){
    if((cs_3_4.capacitiveSensor(1)<10)){
      pressed=false;
    }
  }else{
    if(cs_3_4.capacitiveSensor(1)>50){
      pressed=true;
      state(currstate+1);
    }
  }

  //LED state is depentend on a combination of all those global variables, plus the elapsed time since entering that state:
  currmillis=millis()-startmillis;

  //do all the work. all this to just set an LED based on the elapsed milliseconds and current state.
  switch(currstate) {
    case 0:
      turnOffAll();
    break;
    case 1:  //one long flash
      multiflash(2,4096,4096,0,0);
    break;
    case 2: //flash quickly for a minute
      multiflash(120,250,750,0,0);
    break;
    case 3: //flash slow for 5 minutes with pause in between
      multiflash(60,2500,7500,500,2500);
    break;
    case 4: //flash random quick forever
      multiflash(0,250,750,0,0);
    break;
    case 5: //flash slow forever with pause inbetween
      multiflash(0,2500,7500,500,2500);
    break;
  }
}

//all this to maintain state between loops
void multiflash(int numflashes,int minflash,int maxflash,int mindelay,int maxdelay){
      //if our counter decrements, turn off (state 0)
      if ((count>=numflashes)and(numflashes!=0)){
        count=0;
        state(0);
      //if we've exceeded the lenth of time for the current flash, prepare for the next flash
      }else if(currmillis>flashmillis+delaymillis){
        startmillis=millis();  
        currled=random(0,5);
//        numleds=random(1,2);
        flashmillis=random(minflash,maxflash);
        delaymillis=random(mindelay,maxdelay);
        count++;
      //otherwise. flash a LED
      } else {
        timedflash(currled,flashmillis,currmillis);
      }
}

//resets a ton of global variables to enter a different state
void state(int nextstate){ 
        turnOffAll(); 
        currstate=nextstate;  
        currstate%=6;
        startmillis=millis();  
        flashmillis=0;
        delaymillis=0;
        currled=random(0,5);
//        numleds=random(1,2);
}


//this sets the right PWM value for the LED based on how long the tootal flash should be and how much time has elapsed
void timedflash(int led, int totalmillis, int elapsedmillis){
  //if we're past the end of the flash, then off
  if (elapsedmillis>totalmillis){
    turnOff(led);
  //if we're the first second, then fade in proportionally
  }else if (elapsedmillis<1024){
    analogOn(led,byte(elapsedmillis/4));
  //if we're in the last second, then fade out proportionally
  }else if (totalmillis-elapsedmillis<1024){
    analogOn(led,(totalmillis-elapsedmillis)/4);
  //otherwise, just stay lit
  }else{
    turnOn(led);
  }
}

//useful test function
void flash(int count){
  for(int i=0;i<=count;i++){
    turnOn(1);
    delay(500);
    turnOff(1);
    delay(500);
  }
}

// Turn on a single LED to full brightness
void turnOn(byte led) {
  analogOn(led, 255);
}

// Turn off an LED completely
void turnOff(byte led) {
  analogOn(led, 0);
}

//turns off all LEDs. Should be fixed to use the pin assignments instead of pin numbers directly
void turnOffAll() {
  for(int i=0; i<=2; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

//this chooses between illuminating 1 or 2 charlieplexed LEDs
//not working properly right now
/*
void analogOn12(byte led, byte value) {
  if (numleds==1){
    analogOn(led,value);
  }else{
    analogOn2(led,value);
  }
}


// Write an analog value to 2 LEDs. Thanks to charlieplexing, we can some pairs of LEDs on at once.
//not working properly yet
void analogOn2(byte led, byte value) {
  switch(led) {
    case 0:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinA, LOW);
      digitalWrite(pinC, LOW);
      analogWrite(pinB, value);
      break;
    case 1:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinB, LOW);
      digitalWrite(pinC, LOW);
      analogWrite(pinA, value);
      break;
    case 2:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinC, HIGH);
      digitalWrite(pinA, HIGH);
      analogWrite(pinB, 255-value);
      break;
    case 3:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinC, LOW);
      digitalWrite(pinA, LOW);
      analogWrite(pinB, value);
      break;
    case 4:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinC, HIGH);
      digitalWrite(pinB, HIGH);
      analogWrite(pinA, 255-value);
      break;
    case 5:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinC, LOW);
      digitalWrite(pinB, LOW);
      analogWrite(pinA, value);
      break;
  }
}
*/
// Write an analog value to an LED - just one LED
void analogOn(byte led, byte value) {
  switch(led) {
    case 0:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, INPUT);
      
      digitalWrite(pinA, LOW);
      analogWrite(pinB, value);
      break;
    case 1:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, INPUT);
      
      digitalWrite(pinB, LOW);
      analogWrite(pinA, value);
      break;
    case 2:
      pinMode(pinA, INPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinC, HIGH);
      analogWrite(pinB, 255-value);
      break;
    case 3:
      pinMode(pinA, INPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinC, LOW);
      analogWrite(pinB, value);
      break;
    case 4:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, INPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinC, HIGH);
      analogWrite(pinA, 255-value);
      break;
    case 5:
      pinMode(pinA, OUTPUT);
      pinMode(pinB, INPUT);
      pinMode(pinC, OUTPUT);
      
      digitalWrite(pinC, LOW);
      analogWrite(pinA, value);
      break;
  }
}
