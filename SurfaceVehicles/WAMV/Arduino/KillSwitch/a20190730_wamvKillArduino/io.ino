// Filter noise from remote control joystick inputs using meadian of multiple measurements
void filterSwitch() {

  // Preallocate arrays
  int ch5PulseArray[nMedian];
  int ch7PulseArray[nMedian];
  int ch8PulseArray[nMedian];

  // Calculate delay for each reading based on refresh rate of receiver
  int delayTime = (round((1/receiverRR)*1000));  // [ms]

  // Loop, taking a measurement each time and saving to array
  for (int i = 0; i <= nMedian - 1; i++) {

    // Take a measurement from the interrupt global variable
    ch5PulseArray[i] = ch5PulseRaw;   // yaw (right stick left-right)
    ch7PulseArray[i] = ch7PulseRaw;   // sway (left stick left-right)
    ch8PulseArray[i] = ch8PulseRaw;   // surge (left stick up-down)
    
    // Wait for receiver to push another value
    delay(delayTime);

  }

  // Save the median of each measurement array to global variable
  ch5Filtered = QuickMedian<int>::GetMedian(ch5PulseArray,nMedian);
  ch7Filtered = QuickMedian<int>::GetMedian(ch7PulseArray,nMedian);
  ch8Filtered = QuickMedian<int>::GetMedian(ch8PulseArray,nMedian);

}

// Calculate status variables based on filtered switch inputs  
void switch2Status() {
  // Grab ch5, ch7, and ch8 from interrupt global variables
  int ch5Pulse = ch5Filtered;   // mode switch
  int ch7Pulse = ch7Filtered;   // reverse contactor kill switch
  int ch8Pulse = ch8Filtered;   // remote kill switch

  // Pulse width variables
  int ch5PulseNeutral = round((ch5PulseMax - ch5PulseMin) / 2) + ch5PulseMin;
  int ch7PulseNeutral = round((ch7PulseMax - ch7PulseMin) / 2) + ch7PulseMin;
  int ch8PulseNeutral = round((ch8PulseMax - ch8PulseMin) / 2) + ch8PulseMin;

  // Determine remote kill switch (ch8) state
  if (ch8Pulse + pulseTolerance > ch8PulseMin && ch8Pulse - pulseTolerance < ch8PulseMin) {

    // Switch on transmitter is in the up position (unkill)
    Serial.println("Remote kill switch is in the up position...");
    remoteKillStatus = 0;

  }
  else if (ch8Pulse + pulseTolerance > ch8PulseNeutral && ch8Pulse - pulseTolerance < ch8PulseNeutral) {

    // Switch on transmitter is in the middle position (kill)
    Serial.println("Remote kill switch is in the middle position...");
    remoteKillStatus = 1;

  }
  else if (ch8Pulse + pulseTolerance > ch8PulseMax && ch8Pulse - pulseTolerance < ch8PulseMax) {

    // Switch on transmitter is in the down position (kill)
    Serial.println("Remote kill switch is in the down position...");
    remoteKillStatus = 1;

  }
  else {

    // Invalid input from ch8
    Serial.println("Remote kill swich (channel 8) reporting invalid input; check transmitter connection. KILLING...");
    remoteKillStatus = 2;

  }

  // Determine reverse contactor kill switch (ch7) state
  if (ch7Pulse + pulseTolerance > ch7PulseMin && ch7Pulse - pulseTolerance < ch7PulseMin) {

    // Switch on transmitter is in the up position (unkill)
    Serial.println("Reversing contactor kill switch is in the up position.");
    revConKillStatus = 0;

  }
  else if (ch7Pulse + pulseTolerance > ch7PulseNeutral && ch7Pulse - pulseTolerance < ch7PulseNeutral) {

    // Switch on transmitter is in the middle position (kill)
    Serial.println("Reversing contactor kill switch is in the middle position.");
    revConKillStatus = 1;

  }
  else if (ch7Pulse + pulseTolerance > ch7PulseMax && ch7Pulse - pulseTolerance < ch7PulseMax) {

    // Switch on transmitter is in the down position (kill)
    Serial.println("Reversing contactor kill switch is in the down position.");
    revConKillStatus = 1;

  }
  else {

    // Invalid input from ch8
    Serial.println("Remote reverse contactor kill swich (channel 7) reporting invalid input; check transmitter connection. KILLING...");
    revConKillStatus = 2;

  }

  // Determine auto/manual mode switch state
  if (ch5Pulse + pulseTolerance > ch5PulseMax && ch5Pulse - pulseTolerance < ch5PulseMax) {

    // Switch on transmitter is in the up position (auto)
    Serial.println("Mode switch is in the up position.");
    modeStatus = 0;
    
  }
  else if (ch5Pulse + pulseTolerance > ch5PulseMin && ch5Pulse - pulseTolerance < ch5PulseMin) {

    // Switch on transmitter is in the down position (manual)
    Serial.println("Mode switch is in the down position.");
    modeStatus = 1;
    
  }
  else {

    // Invalid input from ch5
    Serial.println("Remote mode swich (channel 5) reporting invalid input; check transmitter connection. Setting to MANUAL...");
    modeStatus = 1;

  }


  Serial.println(digitalRead(physicalKillPin));

  // Read state of physical kill switches
  if (digitalRead(physicalKillPin) == HIGH) {

    // Physical kill switch is unpressed (unkill)
    Serial.println("All physical kill switches are unkilled.");
    physicalKillStatus = 0;
    
  }
  else {

    // Physical kill switch is pressed (kill)
    Serial.println("At least one physical kill switch is killed.");
    physicalKillStatus = 1;
    
  }

  // Full statement to kill or unkill system.  THIS IS WHERE THE KILL ACTUALLY OCCURS
  if (physicalKillStatus == 0 && remoteKillStatus == 0 && revConKillStatus == 0) {

    killStatus = 0;
    digitalWrite(killRelayPin,LOW);         // pull LOW to unkill
    digitalWrite(revConKillPin,LOW);        // pull LOW to unkill
    digitalWrite(q1KillPin,LOW);            // pull LOW to unkill
    digitalWrite(q2KillPin,LOW);            // pull LOW to unkill
    digitalWrite(q3KillPin,LOW);            // pull LOW to unkill
    digitalWrite(q4KillPin,LOW);            // pull LOW to unkill
    Serial.println("\nSystem is UNKILLED.");
    
  }
  else {

    killStatus = 1;
    digitalWrite(killRelayPin,HIGH);        // pull HIGH to kill
    digitalWrite(revConKillPin,HIGH);       // pull HIGH to kill
    digitalWrite(q1KillPin,HIGH);           // pull HIGH to kill
    digitalWrite(q2KillPin,HIGH);           // pull HIGH to kill
    digitalWrite(q3KillPin,HIGH);           // pull HIGH to kill
    digitalWrite(q4KillPin,HIGH);           // pull HIGH to kill
    Serial.println("\nSystem is KILLED.");
    
  }
  
}

// Communicate status variables to main Arduino
void status2mainArduino() {
  
  if (killStatus == 0) {
    digitalWrite(killCommPin,LOW);        // pull low to indicate unkill
  }
  else {
    digitalWrite(killCommPin,HIGH);       // pull high to indicate kill
  }
  if (revConKillStatus == 0) {
    digitalWrite(revConKillCommPin,LOW);  // pull low to indicate unkill
  }
  else {
    digitalWrite(revConKillCommPin,HIGH); // pull high to indicate kill
  }
  if (modeStatus == 0) {
    digitalWrite(modeCommPin,LOW);        // pull low to indicate autonomous
  }
  else {
    digitalWrite(modeCommPin,HIGH);       // pull high to indicate manual
  }
  
}

// LED controller
void status2Led() {

  if (killStatus) {
    digitalWrite(redLightPin, LOW);
    digitalWrite(greenLightPin, HIGH);
    digitalWrite(blueLightPin, HIGH);
//    Serial.println("RED");
  }

  else if (!killStatus && modeStatus) {
    digitalWrite(redLightPin, LOW);
    digitalWrite(greenLightPin, LOW);
    digitalWrite(blueLightPin, HIGH);
//    Serial.println("YELLOW");
  }

  else if (!killStatus && !modeStatus) {
    digitalWrite(redLightPin, HIGH);
    digitalWrite(greenLightPin, HIGH);
    digitalWrite(blueLightPin, LOW);
//    Serial.println("BLUE");
  }
  else {
    digitalWrite(redLightPin, LOW);
    digitalWrite(greenLightPin, LOW);
    digitalWrite(blueLightPin, LOW);
//    Serial.println("WHITE, ERROR");
  }
}
