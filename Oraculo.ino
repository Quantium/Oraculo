//Simple Finite State Machine Patter applied to a Simple Arduino Project
#include <LiquidCrystal.h>
//Define States
#define WAITING 0
#define ASKING 1
#define ANSWERING 2
#define RETURNING 3
//This variable contains the actual State the machine is running to
unsigned int state;
//This variable contains the previous State that the machine was running exactly one loop iteration before
//I set this to for because 0 status is already used by WAITING State
unsigned int prevState = 4;

LiquidCrystal lcd(12,11,5,4,3,2);

int switchState = 0;
int prevSwitchState = 0;
int reply;
//These are the posible responses for our Oraculus. Is spanish. I hope you speak spanish. If don't use google translate.
String responses[] = {"SI", "NO","PROBABLEMENTE", "CIERTAMENTE", "CHIQUITIWOW",
                    "INSEGURO", "NO ENTENDI", "DUDOSO"};
//In the setup initialize the button input pin and the necesary pins to LcdCrystal
void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(6,INPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  lcd.noAutoscroll();
  //As you can see the initial state will be WAITING(0)
  state = WAITING;
}

//This is only a debug function that simplify to se the previous and the actual state
void printState(){
  Serial.print(state);
  Serial.print(" : ");
  Serial.println(prevState);
} 

//In the loop function the only thing I do is to switch (Literaly) the functions related to the actual state.
void loop(){
  switch(state){
    case WAITING:
      waitingState();
      break;
    case ASKING:
      askingState();
      break;
    case ANSWERING:
      answeringState();
      break;
    case RETURNING:
      returningState();
      break;
  }
}

//The waitingState is planned to run called by the loop function
boolean waitingState(){
  //I first check if the immediately previous state was different to WAITING(0),
  if(prevState != WAITING){ //if it was ...
    //I set prevState to the actual
    prevState = state;
    //clear the LcdCrystal screen
    lcd.clear();
    //Write the welcome message
    lcd.setCursor(2,0);
    lcd.print("Pregunta al:");
    lcd.setCursor(4,1);
    lcd.print("Oraculo!");
    //Debug printing the states
    printState();
  }
  //On every loop I read if the button is pressed
  switchState = digitalRead(6);
  //If it's pressed I check if wasn't that way before in order to avoid
  //repeating the same actions in a loop when the button was pressed or released
  if(switchState != prevSwitchState){
    //If it wasn't I check if the state is LOW
    //This means that the before state of the button was pressed/HIGH
    //This is a kind of onRelease Listener... very primitive I know.
    if(switchState == LOW){
      //When the button release itself I change the state to ASKING(1)
      state = ASKING;
      prevSwitchState = switchState;
      return true;
    }
  }
  //On every iteration I change the prev state of the button to the actual one
  prevSwitchState = switchState;
  //This is useless burocratic peace of shit :p
  return false;
}

unsigned long askingTime;
boolean ledblink = HIGH;

//Mostly the same as the previous
boolean askingState(){
  if(prevState != ASKING){
    prevState = state;
    askingTime = millis();
    //Write a mysterious message
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("El Oraculo dice");
    printState();
  }
  //Every 50 milliseconds
  if((millis() - askingTime) % 50 == 0){
    //Blinking
    ledblink = !ledblink;
    digitalWrite(7,ledblink);
    digitalWrite(9,!ledblink);
  }
  //After 1.5 seconds
  if(millis() > askingTime + 1500){
    printState();
    //Cooling down
    digitalWrite(7,LOW);
    digitalWrite(9,LOW);
    //Change the state to ANSWERING(2)
    state = ANSWERING;
    return true;
  }
  return false;
}

//When ANSWERING
boolean answeringState(){
  if(prevState != ANSWERING){
    digitalWrite(8,HIGH);
    prevState = state;
    //In the begining of the state I show a random spanish answer
    reply = random(8);
    String response = responses[reply];
    //This messure the center less the half of the size of the response
    //to know in what cursor you need to put your word to center it.
    int c = (16 - response.length())/2;
    lcd.setCursor(c,1);
    lcd.print(response);
    printState();
  }
  //After 3 secods
  if(millis() > askingTime + 3000){
    printState();
    digitalWrite(8,LOW);
    //Bye
    state = RETURNING;
    return true;
  }
  return false;
  
}

unsigned long returningTime;
int rc;
//Let's start again
boolean returningState(){
  if(prevState != RETURNING){
    prevState = state;
    rc = 0;
    returningTime = millis();
  }
  //This scroll Left the screen every .3 seconds
  if((millis() - returningTime) % 300 == 0){
    lcd.scrollDisplayLeft();
    //And... It's gone
    if(++rc >= 16){
      //And returning to WAITING State
      state = WAITING;
      return true;
    }
  }
  return false;
}



