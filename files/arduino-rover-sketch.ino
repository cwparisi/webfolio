/*

  XBeeCableReplacement.pde
   
  Control the color of an RGB LED wirelessly via XBee.
   
   
  Required (in addition to your Arduino):
  
    (These items are found in the SparkFun XBee Wireless Kit Retail product
     or can be obtained individually.)
  
       * 1 x SparkFun XBee Explorer USB
    
       * 1 x SparkFun XBee Wireless Shield
    
       * 2 x "Series 1" Digi XBee communication modules
   
   
  Instructions:
  
    * Ensure both XBee communication modules are configured to use
      their default configuration and communicate at 9600 baud.
  
    * Put the XBee Wireless Shield onto your Arduino but do not insert
      the XBee communication module.
  
    * Wire an RGB LED to your Arduino as shown in CIRC-12 of the
      SparkFun Inventor's Guide.
      
    * Connect your Arduino to your computer using a USB cable.
    
    * Upload this sketch as normal to your Arduino.
      
    * Test the functionality of the sketch works when connected via
      the USB cable. Open the Serial Monitor window and ensure it is
      set to communicate at 9600 baud. Type a sequence of one or more
      of the characters 'r', 'g' and 'b' and then press the "Send"
      button. The RGB LED should change color in response. A space
      character in the sequence will cause the LED to turn off.
      
    * Disconnect your Arduino from your computer.
    
    * Insert one XBee module into the XBee Explorer board. Ensure the
      orientation of the XBee module is correct with the beveled end
      of the module pointing away from the USB connector so the module
      outline matches the diagonal white lines silkscreened on the
      board.
      
    * Insert the other XBee module into the XBee Wireless Shield.
      Again, ensure the orientation of the XBee module is correct with
      the beveled end pointing over the edge of the shield away from
      the USB connector on the Arduino board. The outline of the
      module should match the the diagonal white lines silkscreened on
      the shield.
    
    * The small switch located near the TX/RX pins on the shield
      should be set to the "UART" position so the XBee module is
      connected to the hardware serial port on the Arduino.

    * Connect the XBee Explorer USB to your computer with a mini-USB
      cable.
  
    * Select the correct serial port for your XBee Explorer USB from
      the Tools > Serial Port menu in the Arduino IDE.
  
    * Connect your Arduino with the XBee Wireless Shield on it to a
      power source--either a battery, a "wall wart" power adapter or
      another USB port.
      
    * Open the Serial Monitor window of the Arduino IDE and make sure
      the correct communication speed of "9600 baud" is selected from
      the pop-up menu in the lower right corner of the Serial Monitor
      window.
      
    * You may need to press the reset button on the shield to
      redisplay the instructions.
      
    * Control your LED as before--except you've now replaced the cable
      with air!

    
   For more details and help read the XBee Shield and Explorer USB
   Quickstart Guide:
   
      <http://sparkfun.com/tutorials/192>

 */

// LED leads connected to PWM pins
const int RED_LED_PIN = 9;
const int GREEN_LED_PIN = 10;
const int BLUE_LED_PIN = 11;

// Change this value if you want fast color changes
const int WAIT_TIME_MS = 40;


void setLedColour(int redIntensity, int greenIntensity, int blueIntensity) {
   /*

      This routine sets the PWM value for each color of the RGB LED.

    */
   analogWrite(RED_LED_PIN, redIntensity);
   analogWrite(GREEN_LED_PIN, greenIntensity);
   analogWrite(BLUE_LED_PIN, blueIntensity);
}


int E1 = 6; //M1 Speed Control
int E2 = 5; //M2 Speed Control
int M1 = 8; //M1 Direction Control
int M2 = 7; //M2 Direction Control


#include "pitches2.h"

// notes in the melody:
/*int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
*/

int melody_zelda[] = {
  NOTE_C5, NOTE_CS5 ,NOTE_D5, NOTE_DS5};

/*
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };
*/

int noteDurations_zelda[] = {
  4, 4, 4, 1 };

void setup() {
  int i;
  for(i=5;i<=8;i++)
  pinMode(i, OUTPUT);
  // Configure the serial port and display instructions.
  Serial.begin(9600);
  //Serial.println("Send the characters 'r', 'g' or 'b' to change LED colour:");

//Startup Song
  for (int thisNote = 0; thisNote < 4; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations_zelda[thisNote];
    tone(8, melody_zelda[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.1;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }

}


void loop() {

  // When specific characters are sent we change the current color of the LED.
  /*
  while (Serial.available() < 1) {} // Wait until a character is received
  char characterRead = Serial.read();
  int leftspeed = 255; //255 is maximum speed
  int rightspeed = 255;
 */
  if (Serial.available()) {
    int characterRead = Serial.read();
    
    int leftspeed = 255; //255 is maximum speed
    int rightspeed = 255;    
    
    // If the character matches change the state of the LED,
    // otherwise ignore the character.
  
    switch(characterRead) {
       case 't':
         setLedColour(255, 0, 0);
         break;
       
       case 'g':
         setLedColour(0, 255, 0);       
         break;
        
       case 'b':
         setLedColour(0, 0, 255);       
         break;
         
       case ' ':
         setLedColour(0, 0, 0);       
         break;
case 'w'://Move Forward
forward (leftspeed,rightspeed);
break;
case 's'://Move Backwards
reverse (leftspeed,rightspeed);
break;
case 'a'://Turn Left
left (leftspeed,rightspeed);
break;
case 'd'://Turn Right
right (leftspeed,rightspeed);
break;
case 'x'://Stop
stop();
break;
case 'r'://Autonomous Mode
forward (leftspeed, rightspeed);
delay(2000);
stop();
delay(2000);
right (leftspeed, rightspeed);
delay(2000);
stop();
delay(2000);
left (leftspeed, rightspeed);
delay(2000);
stop();
delay(2000);
reverse (leftspeed, rightspeed);
delay(2000);
stop();
delay(2000);
break; 

default:
         // Ignore all other characters and leave the LED
         // in its previous state.
//stop();
break;

}
delay(WAIT_TIME_MS);
}
}

void stop(void) //Stop
{
digitalWrite(E1,LOW);
digitalWrite(E2,LOW);
}
void forward(char a,char b)
{
   //tone(8, NOTE_C4, 1000/1);
    
analogWrite (E1,a);
digitalWrite(M1,LOW);
analogWrite (E2,b);
digitalWrite(M2,LOW);
}
void reverse (char a,char b)
{
  
  //tone(8, NOTE_C4, 1000/1);
  
analogWrite (E1,a);
digitalWrite(M1,HIGH);
analogWrite (E2,b);
digitalWrite(M2,HIGH);
}
void left (char a,char b)
{
  
  //tone(8, NOTE_C4, 1000/1);
  
analogWrite (E1,a);
digitalWrite(M1,HIGH);
analogWrite (E2,b);
digitalWrite(M2,LOW);
}
void right (char a,char b)
{
  
  //tone(8, NOTE_C4, 1000/1); 
  
analogWrite (E1,a);
digitalWrite(M1,LOW);
analogWrite (E2,b);
digitalWrite(M2,HIGH);
}



