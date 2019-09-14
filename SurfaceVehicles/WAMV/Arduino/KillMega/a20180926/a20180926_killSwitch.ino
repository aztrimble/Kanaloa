/*
 * This code is used to run the killswitch system. The Arduino is powered by the main power supply in the high current box going through a 12 V regulator. 
 * 5 V goes out to the two physical kill switches which loops back into two pins on the Arduino. When the switches are KILLED, the circuit breaks and the Arduino 
 * reads the pin as LOW. If the switches are UNKILLED, the Arduino reads the pin as HIGH. For the remote killswitch, the Arduino interprets the PWM signal coming 
 * from the receiver to infer the remote killswitch state. If any of these switches are KILLED, a software switch is KILLED, and the system is KILLED. 
 * 
 * Created by: Kai Jones
 * Revisions by: Jordan Romanelli and Kai Jones
 * Date: 2018.09.26
 * 
 * Update Notes:
 * - Implemented code to control all safety lights (green, yellow, and red safety lights) 
 * 
 */

//-----------------
// Pin Definitions
//-----------------
const int physicalKill1Pin = 2;     // Physical kill switch number 1 that reads whether switch is KILLED (OPEN/LOW) or UNKILLED (CLOSED/HIGH) 
const int physicalKill2Pin = 3;     // Physical kill switch number 2 that reads whether switch is KILLED (OPEN/LOW) or UNKILLED (CLOSED/HIGH)  
const int safetyRelayPin = 6;       // Pin which Arduino sends a HIGH/LOW signal to relay
const int stateInPin = 9;           // digital in from mainMega for manual mode state (HIGH for autonomous, LOW for manual)
const int remoteKillInPin = 8;      // digital in from mainMega for kill state (LOW for kill, HIGH for UNKILL)
const int greenLightPin = 12;       // Pin which Arduino sends a HIGH/LOW signal to relay for green safety light
const int redLightPin = 13;         // Red LED light on safety pole (HIGH to turn light ON, LOW to turn light OFF)

//------------------
// System Variables
//------------------
bool physicalKill1Status = true;    // physical kill 1 status variable. true means KILL is detected, false means UNKILL is detected.
bool physicalKill2Status = true;    // physical kill 2 status variable. true means KILL is detected, false means UNKILL is detected.
bool remoteKillStatus = true;       // digital in from mainMega for kill state (LOW for kill, HIGH for UNKILL)
bool stateStatus = true;            // digital in from mainMega for manual mode state (HIGH for autonomous, LOW for manual)
bool killStatus = true;             // overall kill status based on kill switches (HIGH for kill, LOW for UNKILL)
int ch8PulseLength = 0;             // Pulse length of the PWM signal coming from channel 8 of the receiver.


//---------------
// SETUP FUNCTION
//---------------
void setup() {
  pinMode(physicalKill1Pin, INPUT);         // Reads whether physical killswitch 1 is KILLED (LOW) or UNKILLED (HIGH)
  pinMode(physicalKill2Pin, INPUT);         // Reads whether physical killswitch 2 is KILLED (LOW) or UNKILLED (HIGH)
  pinMode(safetyRelayPin, OUTPUT);          // Arduino sends a HIGH/LOW signal to relay depending on killswitches's states
  pinMode(stateInPin, INPUT_PULLUP);        // digital in from mainMega for manual mode state (HIGH for autonomous, LOW for manual)
  pinMode(remoteKillInPin, INPUT_PULLUP);   // digital in from mainMega for kill state (LOW for kill, HIGH for UNKILL)
  pinMode(redLightPin, OUTPUT);             // Arduino sends signal out to red safety light
  pinMode(greenLightPin, OUTPUT);           // Arduino sends HIGH/LOW signal to green safety light
}

//--------------
// LOOP FUNCTION
//--------------
void loop() {

  // Read states
  physicalKill1Status = digitalRead(physicalKill1Pin);
  physicalKill2Status = digitalRead(physicalKill2Pin);
  remoteKillStatus = digitalRead(remoteKillInPin);
  stateStatus = digitalRead(stateInPin);
  
  // If statement to kill the system
  if (physicalKill1Status == HIGH && physicalKill2Status == HIGH && remoteKillStatus == HIGH) {   // All switches are unkilled
    killStatus = false;    
    digitalWrite(safetyRelayPin, LOW);
  }
  else {
    killStatus = true;
    digitalWrite(safetyRelayPin, HIGH);
  }
    
  // Control LED lights
  ledController(killStatus, stateStatus);
  
  delay(50);  // Delay is for data to settle (Wonky without delay)
  
}


//------------
//  FUNCTIONS
//------------
bool ledController(bool killStatus, bool manualControlSignal){
  if(killStatus){
    digitalWrite(redLightPin, LOW);
    digitalWrite(greenLightPin, HIGH);
  }
  else if(!killStatus && !manualControlSignal){
    digitalWrite(redLightPin, LOW);
    digitalWrite(greenLightPin, LOW);
  }
  else if(!killStatus && manualControlSignal){
    digitalWrite(redLightPin, HIGH);
    digitalWrite(greenLightPin, LOW);
  }
  else{
    digitalWrite(redLightPin, HIGH);
    digitalWrite(greenLightPin, HIGH);
  }
}
