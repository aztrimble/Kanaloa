/*
 * I2C master. This program takes the motor controller outputs within 
 * the range of 0-4096 (with 2048 being neutral), creates a string for 
 * each output, and sends it to the high current Arduino via I2C
 * communication protocol. 
 * 
 * Msg Template: 
 *  Q    : start flag
 *  1-4  : thruster number
 *  XXXX : thruster speed magnituderu
 *  F/R  : direction & end flag
 * 
 * Example Output per line: Q13000FQ2500R Q33000FQ4500R 
 * 
 */

#include <Wire.h>

// -------- CONSTANTS ----------------------
#define SLAVE_ADDR 9    // arbitrary address number to identify slave Arduino
#define ANSWERSIZE 14   // number of bits expected to come from slave Arduino

// --------- MESSAGE VARIABLES -------------
volatile char message[28];    // array used to store motor command message from slave Arduino as characters
String q1Msg;                 // string for parsing Q1 motor command
String q2Msg;                 // string for parsing Q2 motor command
String q3Msg;                 // string for parsing Q3 motor command
String q4Msg;                 // string for parsing Q4 motor command
String voltageMsg = "V";      // string for sending Voltage message to slave Arduino
int q1Motor;                  // Q1 motor command to send to motor controller
int q2Motor;                  // Q2 motor command to send to motor controller
int q3Motor;                  // Q3 motor command to send to motor controller
int q4Motor;                  // Q4 motor command to send to motor controller
char q1Dir;                   // variable to store character of Q1's motor direction
char q2Dir;                   // variable to store character of Q2's motor direction
char q3Dir;                   // variable to store character of Q3's motor direction
char q4Dir;                   // variable to store character of Q4's motor direction
float voltage;                // voltage measurement
bool debug = true;            // set to true to see printouts for messages

void setup() {
  Wire.begin();         // join the I2C bus as a master (master device has no paramter)
  Serial.begin(9600);   // establish serial communication with baud rate as the parameter
}

void loop() {
  requestFromSlave();
  parseMotorCmds();
  sendVoltageMsg();
}
