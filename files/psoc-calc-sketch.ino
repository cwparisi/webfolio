#include <Keypad.h>
#include <SPI.h>

 const byte ROWS = 4;  // 4 rows
 const byte COLS = 3;  // 3 columns
 //  Define the Keymap
 char keys [ROWS][COLS] = {
   {'1','2','3'},
   {'4','5','6'},
   {'7','8','9'},
   {'*','0','#'}
 };

byte rowPins[ROWS] = {8,7,6,5};
byte colPins[COLS] = {4,3,2};

//  Create the keypad

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const int slaveSelectPin = 10;

void setup()
{
  //initialize pins as outputs
  pinMode(slaveSelectPin, OUTPUT);
  SPI.begin();
  
  Serial.begin(9600);
}

void loop()
{
  char key = kpd.getKey(); 
 
if(key)      //Check for a valid key
 {
   switch (key)
   {
     case '*':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('*');
       digitalWrite(slaveSelectPin,HIGH); 
       break;
     case '#':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('#');
       digitalWrite(slaveSelectPin,HIGH); 
       break;
     case '0':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('0');
       digitalWrite(slaveSelectPin,HIGH); 
       break;
     case '1':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('1');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     case '2':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('2');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     case '3':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('3');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     case '4':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('4');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     case '5':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('5');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     case '6':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('6');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     case '7':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('7');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     case '8':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('8');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     case '9':
       digitalWrite(slaveSelectPin,LOW);
       SPI.transfer('9');
       digitalWrite(slaveSelectPin,HIGH);
       break;
     default:
       Serial.println(key);
   }
 }  
  
}

