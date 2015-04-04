//******Senior Capstone Design Diwheel Program 3.0********//
/*
Author: Christopher Parisi
Organization: California Baptist University College of Engineering

Baud Rate: 9600 (Arduino Standard)
Use either HyperTerminal or Arduino Serial Monitor for Control 

CAUTION! When controlling Diwheel, be sure not to change motor direction instantaneously. 
Back EMF can harm the motor drivers and permanently break them.

Descrete Transfer Function For Sensor Feedback:
If the gryo and acceleromter IMU sensor was functional, this transfer function would have been used for controlling the angle of the chassis.
The transfer function takes in a theta value and provides a distance that the linear weight needs to be moved.
This number would be used along with the linear motor encoder to control how far the weight is moved at what time.
Z represents the pitch angle given from the IMU sensor and then give a distance.
This movement will be tied to Arduino's Interrupt and will constantly be moving to compensate for various accelerations.

( 5.519*z^2 - 10.95*z + 5.43 ) / ( z^3 - 2.573*z^2 + 2.204*z - 0.6278 ) = distance

Controls for Diwheel
w: forward
s: backward
a: left
d: right
x: stop(safety stop-slows down before stopping)
v: emergency stop
g: start system
r: reset system
1: 10% Speed
2: 20% Speed
3: 30% Speed
4: 40% Speed
5: 50% Speed
6: 60% Speed
7: 70% Speed
8: 80% Speed
9: 90% Speed
0: 100% Speed
*/

//Arduino Pin Assignments
const int LM_DIR = 12;        //Direction control for the linear motor      
const int LM_PWM = 3;        //PWM Signal for speed control of linear motor --CAN I ACTUALL USE THIS PIN?
const int LM_EncA = 2;       //Linear motor encoder A
const int LM_EncB = 5;       //Linear motor encoder A
const int M1_DIR = 6;        //Direction control for Motor 1
const int M2_DIR = 7;        //Direction control for Motor 2
const int M1_RESET = 8;      //Reset signal for Motor Driver 1
const int M2_RESET = 9;      //Reset signal for Motor Driver 2
const int M1_PWMH = 10;       //PWM signal for speed control of Motor 1
const int M2_PWMH = 11;       //PWM signal for speed control of Motor 2

//Other Variable Assignments
//volatile unsigned int encoderPos = 0;
int command = 0;             //User input from Serial Terminal
int sub_command = 0;         //Command used to update information during ramping function
char dir = 'f';              //keeps track of diwheel direction
int spd = 0;                 //Updates speed which is used to control diwheel speed
int current_spd = 0;         //keeps track of current speed for ramping function
int desired_spd = 0;         //keeps track of desired speed for ramping function
char lm_dir = 'f';           //keeps track of linear motor direction
char lm_pos = 'b';           //keeps track of which side the weight is located

 
 void setup()
 {
   //code section from from http://playground.arduino.cc/Main/RotaryEncoders#Example1
   pinMode(LM_EncA, INPUT); 
   digitalWrite(LM_EncA, HIGH);       // turn on pullup resistor
   pinMode(LM_EncB, INPUT); 
   digitalWrite(LM_EncB, HIGH);       // turn on pullup resistor
   //attachInterrupt(0, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2
   
   Serial.begin(9600);      //9600 Baud Rate
   
   Serial.println("Starting Up...");
   pinMode(LM_DIR, OUTPUT);
   pinMode(LM_PWM, OUTPUT);
   //pinMode(LM_EncA, INPUT);
   //pinMode(LM_EncB, INPUT);
   pinMode(M1_DIR, OUTPUT);
   pinMode(M2_DIR, OUTPUT);
   pinMode(M1_RESET, OUTPUT); //defualt HIGH
   pinMode(M2_RESET, OUTPUT); //default HIGH
   pinMode(M1_PWMH, OUTPUT);
   pinMode(M2_PWMH, OUTPUT);
   
   brakeMotors();
   Serial.println("System Ready.");
   
 }
 
 void loop()
 {
   /* Fault Flags removed due to lack of I/O Pins on Arduino
   if ((FF1Motor1 == LOW && FF2Motor1 == HIGH) || (FF1Motor2 == LOW && FF2Motor2 == HIGH)) //Short Circuit
   {
     stopmotors();
     Serial.println("Short Circuit Detected. Stopping Diwheel");
   }
   else if((FF1Motor1 == HIGH && FF2Motor1 == LOW) || (FF1Motor2 == HIGH && FF2Motor2 == LOW))  //Over Temperature
   {
     stopmotors();
     Serial.println("Over Temperature. Stopping Diwheel");
   }
   else if ((FF1Motor1 == HIGH && FF2Motor1 == HIGH) || (FF1Motor2 == HIGH && FF2Motor2 == HIGH)) //Under Voltage
   {
     stopmotors();
     Serial.println("Under Voltage. Stopping Diwheel");
   }
   */
   
   if (Serial.available() > 0) 
   { 
     command = Serial.read();
     switch(command) {
       
       case 'w': //forward
         if (dir == 'b' || dir == 's')
         {
           slowMotors();             //Stop Motors to avoid back EMF
           delay(1000);
         }
         digitalWrite(M1_DIR, HIGH); //Current flows from OUTA(+) to OUTB(-)
         digitalWrite(M2_DIR, HIGH); //Current flows from OUTA(+) to OUTB(-)
         dir = 'f';
         speedControl(dir, spd);      //adjust direction while moving
         break;
         
       case 'a': //left
         dir = 'l';
         speedControl(dir, spd);      //adjust direction while moving
         break; 
       
       case 's': //backward
         if (dir == 'f' || dir == 's')
         {
           slowMotors();            //Stop Motors to avoid back EMF
           delay(1000);
         }
         digitalWrite(M1_DIR, LOW);  //Current flows from OUTB(-) to OUTA(+)
         digitalWrite(M2_DIR, LOW);  //Current flows from OUTB(-) to OUTA(+)
         dir = 'b';
         speedControl(dir, spd);      //adjust direction while moving
         break;
       
       case 'd': //right
         dir = 'r';
         speedControl(dir, spd);
         break;
         
       case 'e': //spin clockwise
         slowMotors();
         delay(1000);
         digitalWrite(M1_DIR, HIGH);
         digitalWrite(M2_DIR, LOW);
         dir = 's';
         break;
       case 'q': //spin counter-clockwise
         slowMotors();
         delay(1000);
         digitalWrite(M1_DIR, LOW);
         digitalWrite(M2_DIR, HIGH);
         dir = 's';
         break;
       
       case 'r': //Reset Motor Driver Circuits
         digitalWrite(M1_RESET, LOW); //Resets Motor 1 (Clears Fault Flags)
         digitalWrite(M2_RESET, LOW); //Resets Motor 2 (Clears Fault Flags)
         break;
       case 'g': //Realeases hold from RESET signal. Must use for initial start up
         digitalWrite(M1_RESET, HIGH);
         digitalWrite(M2_RESET, HIGH);
         break;
       
       case 'x': //stop(coast)
         slowMotors();
         break;
         
       case 'v': //emergency stop (break)
         brakeMotors();
         break;
       
       case 'k': //Linear motor forward (USE ONLY FOR TESTING!)
         if (lm_dir == 'b')
         {
            brakeMotors();
            delay(1000);
         }
         digitalWrite(LM_DIR, LOW);
         lm_dir = 'f';
         speedControl(dir, spd);
         break;
       
       case 'l': //Linear motor backward (USE ONLY FOR TESTING!)
         if (lm_dir == 'f')
         {
            brakeMotors();
            delay(1000);
         }
         digitalWrite(LM_DIR, HIGH);
         lm_dir = 'b';
         speedControl(dir, spd);
         break;
       
       case '1': 
         spd = 25;           //10% power
         desired_spd = 1;
         speedRamp();
         break;
       case '2':
         spd = 50;           //20% power
         desired_spd = 2;
         speedRamp();
         break;
       case '3':
         spd = 75;           //30% power
         desired_spd = 3;
         speedRamp();
         break;
       case '4':
         spd = 100;          //40% power
         desired_spd = 4;
         speedRamp();
         break;
       case '5':
         spd = 125;          //50% power
         desired_spd = 5;
         speedRamp();
         break;
       case '6':
         spd = 150;          //60% power
         desired_spd = 6;
         speedRamp();
         break;
       case '7':
         spd = 175;          //70% power
         desired_spd = 7;
         speedRamp();
         break;
       case '8':
         spd = 200;          //80% power
         desired_spd = 8;
         speedRamp();
         break;
       case '9':
         spd = 225;          //90% power
         desired_spd = 9;
         speedRamp();
         break;
       case '0':
         spd = 255;          //100% power
         desired_spd = 10;
         speedRamp();
         break;
     } 
   }
 }
 
 void speedRamp()
 {
   if (desired_spd > current_spd)
   {
     while ((current_spd != desired_spd) && (Serial.read() != 'x') && (Serial.read() != 'v'))
     {
       sub_command = Serial.read();    //check if a new command has been updated
       if (sub_command == 'a')
       {
         dir = 'l';
       }
       else if (sub_command == 'd')
       {
         dir = 'r';
       }
       current_spd++;                 //ramp of speed 10% at a time
       spd = current_spd*25;          //Pass PWM number corresponding to speed
       speedControl(dir, spd);        //Update speed and direction
       delay(500);                    //Wait 1/2 a second until ramping up again
     }
     if (Serial.read() == 'v')
     {
       brakeMotors();
     }
     else if (Serial.read() == 'x')
     {
       slowMotors();
     }
   }
   else if (desired_spd < current_spd)
   {
     while ((current_spd != desired_spd) && (Serial.read() != 'x') && (Serial.read() != 'v'))
     {
       sub_command = Serial.read();
       if (sub_command == 'a')
       {
         dir = 'l';
       }
       else if (sub_command == 'd')
       {
         dir = 'r';
       }
       current_spd--;
       spd = current_spd*25;
       speedControl(dir, spd);
       delay(500);
     }
     if (Serial.read() == 'v')
     {
       brakeMotors();
     }
     else if (Serial.read() == 'x')
     {
       slowMotors();
     }
   }
   else
     speedControl(dir, spd);
 }
 
 void speedControl(char dir, int x)
 {
   if (dir == 'f' || dir == 'b' || dir == 's')
   {
           analogWrite(M1_PWMH, (x-0.05*x));                //10% reduced to counter drift
           analogWrite(M2_PWMH, x);
           /*
           if (dir == 'f' && lm_pos != 'b')
           {
             digitalWrite(LM_DIR, HIGH);
             analogWrite(LM_PWM, 255);
             delay(1000);
             analogWrite(LM_PWM, 0);
             lm_pos = 'b';
           }
           else if (dir = 'b' && lm_pos != 'f')  
           {
             digitalWrite(LM_DIR, LOW);
             analogWrite(LM_PWM, 255);
             delay(1600);
             analogWrite(LM_PWM, 0);
             lm_pos = 'f';
           } 
     */      
   }
   else if (dir == 'l')
   {
           if (spd == 50 || spd == 75 || spd == 100)        //30% drop in speed in left wheel
           {
             analogWrite(M1_PWMH, (x-0.4*x));
             analogWrite(M2_PWMH, x);
           }
           else if (spd == 125 || spd == 150 || spd == 175) //20% drop in speed in left wheel
           {
             analogWrite(M1_PWMH, (x-0.3*x));
             analogWrite(M2_PWMH, x);
           }
           else if (spd == 200 || spd == 225 || spd == 255) //10% drop in speed in left wheel
           {
             analogWrite(M1_PWMH, (x-0.2*x));
             analogWrite(M2_PWMH, x);
           }
           else
           {
             analogWrite(M1_PWMH, x/2);
             analogWrite(M2_PWMH, x);
           }  
    }
    else if (dir == 'r')
    {
           if (spd == 50 || spd == 75 || spd == 100)        //30% drop in speed in right wheel
           {
             analogWrite(M1_PWMH, x);
             analogWrite(M2_PWMH, (x-0.4*x));
           }
           else if (spd == 125 || spd == 150 || spd == 175) //20% drop in speed in right wheel
           {
             analogWrite(M1_PWMH, x);
             analogWrite(M2_PWMH, (x-0.3*x));
           }
           else if (spd == 200 || spd == 225 || spd == 255) //10% drop in speed in right wheel
           {
             analogWrite(M1_PWMH, x);
             analogWrite(M2_PWMH, (x-0.2*x));
           }
           else
           {
             analogWrite(M1_PWMH, x);
             analogWrite(M2_PWMH, x/2);
           }
    }

 }
 
 void brakeMotors()
 {
   analogWrite(M1_PWMH, 0);   //Turn the motor off by shorting it to GND
   analogWrite(M2_PWMH, 0);   //Turn the motor off by shorting it to GND
   analogWrite(LM_PWM, 0);    //Turn the motor off by shorting it to GND
   desired_spd = 0;
   current_spd = 0;
   spd = 0;
 }
 
 void slowMotors()
 {
   desired_spd = 0;
   while (current_spd != desired_spd)
   {
     current_spd--;
     spd = current_spd*25;
     speedControl(dir, spd);
     delay(300);
   }
     analogWrite(M1_PWMH, 0);   //Turn the motor off by shorting it to GND
     analogWrite(M2_PWMH, 0);   //Turn the motor off by shorting it to GND
     analogWrite(LM_PWM, 0);    //Turn the motor off by shorting it to GND
     spd = 0;
     current_spd = 0;
 }
 
/*
 void doEncoder() 
 {
  /* If pinA and pinB are both high or both low, it is spinning
   * forward. If they're different, it's going backward.
   *
   * For more information on speeding up this process, see
   * [Reference/PortManipulation], specifically the PIND register.
  */ /*
  if (digitalRead(LM_EncA) == digitalRead(LM_EncB)) {
    encoderPos++;
  } else {
    encoderPos--;
  }

  Serial.println (encoderPos, DEC);
}*/

/* Alternate function to show detail in encoder
void doEncoder_Expanded(){
  if (digitalRead(encoder0PinA) == HIGH) {   // found a low-to-high on channel A
    if (digitalRead(encoder0PinB) == LOW) {  // check channel B to see which way
                                             // encoder is turning
      encoder0Pos = encoder0Pos - 1;         // CCW
    } 
    else {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
  }
  else                                        // found a high-to-low on channel A
  { 
    if (digitalRead(encoder0PinB) == LOW) {   // check channel B to see which way
                                              // encoder is turning  
      encoder0Pos = encoder0Pos + 1;          // CW
    } 
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }

  }
  Serial.println (encoder0Pos, DEC);          // debug - remember to comment out
                                              // before final program run
  // you don't want serial slowing down your program if not needed
}
*/
