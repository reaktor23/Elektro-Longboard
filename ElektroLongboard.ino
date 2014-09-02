/*
 Original example by:
   Barrett Anderies
   March 11, 2013
 Modified by:
   Kristian Lauszus
   For more information visit his blog: http://blog.tkjelectronics.dk/ or 
   send him an e-mail:  kristianl@tkjelectronics.com
 Modified for ElektroLongboard by:
   Sniser, Bouni
 */

#include <PS3BT.h>                                                    //Include the necessary libraries.
#include <Servo.h>

#define BRAKE_VALUE        85
#define DEAD_MAN_VALUE     90
#define MAX_VALUE          180
#define RAMP_SPEED         30
#define RAMP_VALUE         5

USB Usb;
BTD Btd(&Usb);
PS3BT PS3(&Btd); 

Servo servo1;                                                         //Create instances of type Servo. servo1 is the steering servo and servo2 is the ESC.

unsigned int raw;
unsigned int val;

void setup() {
    Serial.begin(115200);                                              
    Serial.print(F("\r\nSerial port up at 115200 Baud"));
    if(Usb.Init() == -1) {                                            
        Serial.print(F("\r\nOSC did not start"));
        while(1); //halt
    }
    Serial.print(F("\r\nPS3 Bluetooth Library Started"));              
    pinMode(5, OUTPUT);
    pinMode(4, INPUT_PULLUP);
    servo1.attach(5);
    Serial.print(F("\r\nPWM output attached to pin 5"));              
}

void loop() 
{
    Usb.Task();
    if(PS3.PS3Connected || PS3.PS3NavigationConnected) {
        if(!digitalRead(4) && !PS3.getButtonPress(L1)) {
          raw = PS3.getAnalogButton(L2);
          val = map(raw, 0, 255, DEAD_MAN_VALUE, MAX_VALUE);
          servo1.write(ramp(val));
          Serial.print(raw);              
          Serial.print(" :: ");
          Serial.println(ramp(val));
        } else if(!digitalRead(4) && PS3.getButtonPress(L1)) { 
          // if dead man switch is ok and L1 is pressed, give negative speed command
          servo1.write(BRAKE_VALUE);
          Serial.println("Brake!");
        } else {
          // if dead man switch is not ok set speed command to 0
          servo1.write(DEAD_MAN_VALUE);
          Serial.println("Stop!");
        } 
          
    } else {
        // set output value to 50% to stop the motor if controller is not connected
        servo1.write(90);
    }
    
     if(PS3.getButtonClick(PS)) {
      PS3.disconnect();
   }
    // delay as in the servo example
    delay(1);
}

int ramp(int val) {
  
  static int last = DEAD_MAN_VALUE;
  static unsigned long time = millis();
  
  if(millis() - time > RAMP_SPEED) {
    time = millis();

    if(val - last > RAMP_VALUE) {
      last += RAMP_VALUE;
      return last;
    } else {
      last = val;
      return val;
    } 
    
  }
  
  return last;  
}
