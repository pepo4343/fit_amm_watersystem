
#include "config.h"
#include "volume.h"
#include <EEPROM.h>

unsigned long height;

long microsecondsToCentimeters(long microseconds);
long getDistance();

unsigned char reading;
long time = 0;
unsigned char previous = HIGH;


void initUS()
{
    //initialize ultrasonic sensor
    pinMode(US_SET_HEIGHT_PIN, INPUT_PULLUP);

    pinMode(US_ECHO, INPUT);
    pinMode(US_TRIG, OUTPUT);

    EEPROM.get(HEIGHT_ADDRESS, height);
    Serial.println("----------------------");
    Serial.println(height);
    Serial.println("----------------------");
}

void getPercent( unsigned int &currentPercent)
{

    long distance = getDistance();

    if (distance == -1)
    {
    
        Serial.println("GG");
        currentPercent = 0;
        return;
    }

    long waterHeight = height - distance;

    waterHeight = constrain(waterHeight, 0, height);
    float percent = (float)waterHeight / (float)height * 100;

    currentPercent = percent;
}

long getDistance()
{
    digitalWrite(US_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(US_TRIG, HIGH);
    delayMicroseconds(5);
    digitalWrite(US_TRIG, LOW);
    unsigned long duration = pulseIn(US_ECHO, HIGH,20000);

    long cm = microsecondsToCentimeters(duration);

    // Serial.println("+++++++++++++++++++++++++++++++");

    // Serial.println(cm);

    // Serial.println("+++++++++++++++++++++++++++++++");
    if(cm == 0){
        return height;
    }

    if( cm - HEIGHT_PADDING < 0 ){
        return 0;
    }
    return  cm - HEIGHT_PADDING ;
}

long microsecondsToCentimeters(long microseconds)
{
    // The speed of sound is 340 m/s or 29 microseconds per centimeter.
    // The ping travels out and back, so to find the distance of the
    // object we take half of the distance travelled.
    return microseconds / 29 / 2;
}

void setHeight()
{
    reading = digitalRead(US_SET_HEIGHT_PIN);

    if (reading == HIGH && previous == LOW && millis() - time > BUTTON_DEBOUNCE)
    {
        Serial.println("Srtting");
        height = getDistance();

        EEPROM.put(HEIGHT_ADDRESS, height);
        time = millis();
    }
    previous = reading;
}