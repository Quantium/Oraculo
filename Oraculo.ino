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
unsigned int prevState = RETURNING;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int reply;
//These are the posible responses for our Oraculus. Is spanish. I hope you speak spanish. If don't use google translate.
String responses[] = {"SI", "NO", "PROBABLEMENTE", "CIERTAMENTE", "CHIQUITIWOW",
                      "INSEGURO", "NO ENTENDI", "DUDOSO"
                     };
//In the setup initialize the button input pin and the necesary pins to LcdCrystal
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(A0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.noAutoscroll();
  //As you can see the initial state will be WAITING(0)
  state = WAITING;
}

String stateToString(int s){
  switch (s) {
    case WAITING:
      return "WAITING";
      break;
    case ASKING:
      return "ASKING";
      break;
    case ANSWERING:
      return "ANSWERING";
      break;
    case RETURNING:
      return "RETURNING";
      break;
  }
  return "NO STATE";
}

//This is only a debug function that simplify to se the previous and the actual state
void printState() {
  Serial.print(stateToString(prevState));
  Serial.print(" > ");
  Serial.println(stateToString(state));
}

//In the loop function the only thing I do is to switch (Literaly) the functions related to the actual state.
void loop() {
  switch (state) {
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

int switchState = LOW;
int prevSwitchState = LOW;

//The waitingState is planned to run called by the loop function
boolean waitingState() {
	if(state == WAITING && prevState != WAITING){
		printState();
		prevState = WAITING;
		lcd.clear();
		lcd.setCursor(2, 0);
		lcd.print("Pregunta al:");
		lcd.setCursor(4, 1);
		lcd.print("Oraculo!");
		return false;
	}
	if(state == WAITING && prevState == WAITING){
		//On every loop I read if the button is pressed
		switchState = analogRead(A0);
		//If it's pressed I check if wasn't that way before in order to avoid
		//repeating the same actions in a loop when the button was pressed or released
		if (switchState == 1023 && prevSwitchState == 0) {
			switchState = HIGH;
			Serial.println("Button pressed");
			digitalWrite(LED_BUILTIN, HIGH);
			state = ASKING;
			return true;
		}
		prevSwitchState = LOW;
		return false;
	}
	return false;
}

unsigned long askingTime;
boolean ledblink = HIGH;

boolean askingState() {
	if(state == ASKING && prevState != ASKING){
		printState();
		prevState = ASKING;
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("El Oraculo dice");
		askingTime = millis();
	}
	if(state == ASKING && prevState == ASKING){
		//After 3 seconds
		if (millis() > askingTime + 3000) {
			digitalWrite(LED_BUILTIN, LOW);
			//Change the state to ANSWERING(2)
			state = ANSWERING;
			return true;
		}
		return false;
	}
	return false;
}

//When ANSWERING
boolean answeringState() {
	if(state == ANSWERING && prevState != ANSWERING){
		printState();
		prevState = ANSWERING;
		//In the begining of the state I show a random spanish answer
		reply = random(8);
		String response = responses[reply];
		//This messure the center less the half of the size of the response
		//to know in what cursor you need to put your word to center it.
		int c = (16 - response.length()) / 2;
		lcd.setCursor(c, 1);
		lcd.print(response);
	}
	if(state == ANSWERING && prevState == ANSWERING){
		//After 3 seconds
		if (millis() > askingTime + 3000) {
			//Bye
			state = RETURNING;
			return true;
		}
		return false;
	}
	return false;
}

unsigned long returningTime;
int rc;
boolean displayOn = 1;
//Let's start again
boolean returningState() {
	if(state == RETURNING && prevState != RETURNING){
		printState();
		prevState = RETURNING;
		rc = 0;
		returningTime = millis();
	}
	if(state == RETURNING && prevState == RETURNING){
		//This scroll Left the screen every .3 seconds
		if ((millis() - returningTime) % 300 == 0) {
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("El Oraculo dice");
			if (displayOn){
				String response = responses[reply];
				int c = (16 - response.length()) / 2;
				lcd.setCursor(c, 1);
				lcd.print(response);
				lcd.print(" ");
				lcd.print(rc);
				displayOn = 0;
			} else {
				displayOn = 1;
			}
			//And... It's gone
			if (++rc >= 20) {
				//And returning to WAITING State
				state = WAITING;
				lcd.display();
				displayOn = 1;
				return true;
			}
			return false;
		}
	return false;
	}
}
