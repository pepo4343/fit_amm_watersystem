#include "ultrasonic.h"
#include "config.h"

long microsecondsToCentimeters(long microseconds);
void ulInit(){
    pinMode(UL_TRIG,OUTPUT);
    pinMode(UL_ECHO,INPUT);
}

 long getDistanceCM(){
      long duration, cm;
     

      digitalWrite(UL_TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(UL_TRIG, HIGH);
      delayMicroseconds(5);
      digitalWrite(UL_TRIG, LOW);

      duration = pulseIn(UL_ECHO, HIGH,40000);

      cm = microsecondsToCentimeters(duration);


      return cm;
 }

  long microsecondsToCentimeters(long microseconds)
    {
      // The speed of sound is 340 m/s or 29 microseconds per centimeter.
      // The ping travels out and back, so to find the distance of the
      // object we take half of the distance travelled.
      return microseconds / 29 / 2;
    }

long getVolume(long distance){
    float volume = 3.14*TANK_DIAMETER*TANK_DIAMETER/4*distance*0.001;
 
    return (long)volume;
}