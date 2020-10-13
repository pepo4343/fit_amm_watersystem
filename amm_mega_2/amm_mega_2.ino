#include "uart.h"
#include "rtc.h"
#include "function.h"
#include "schedule.h"
#include "config.h"
#include "moisture.h"
#include "ultrasonic.h"

#include <avr/wdt.h>

#define EEPROM_SCHEDULE_ADDRESS_1 1
#define EEPROM_SCHEDULE_SIZE_ADDRESS_1 0

#define EEPROM_SCHEDULE_ADDRESS_2 101
#define EEPROM_SCHEDULE_SIZE_ADDRESS_2 100

#define EEPROM_FERTILIZERUSED 200
#define EEPROM_WATERRUSED 300

#define EEPROM_IS_FIRST_TIME_ADDRESS 2000

#define TANK_HEIGHT 64
#define SENSOR_TOP_DISTANCE 10
#define SENSOR_BOTTOM_DISTANCE 30

unsigned char isTankEmpty = 0;

unsigned char isAuto = 1;

unsigned char pump_status = OFF;

unsigned char value1_status = OFF;
unsigned char value2_status = OFF;

unsigned char value_fertilizer_status = OFF;

float moisture1 = 0;
float moisture2 = 0;

unsigned long flowCounter1 = 0;

unsigned long waterUsed = 0;

unsigned char value1_control = 0;
unsigned char value2_control = 0;

unsigned char timer1sFlag = 0;

long prevDistance = 10000;

void serialCallback(char *type, char *message)
{
    digitalWrite(LED_INTERNET_PIN, HIGH);
    delay(50);
    digitalWrite(LED_INTERNET_PIN, LOW);
    //   char buffer[BUFFER_SIZE];
    //   message.toCharArray(buffer,BUFFER_SIZE);

    if (strcmpPointer(type, "CNM"))
    {
        isAuto = (char)*message - 48;
        value1_control = 0;
        value2_control = 0;
    }

    if (strcmpPointer(type, "CN1"))
    {
        value1_control = (char)*message - 48;
    }

    if (strcmpPointer(type, "CN2"))
    {
        value2_control = (char)*message - 48;
    }

    if (strcmpPointer(type, "CNF"))
    {
        value_fertilizer_status = (char)*message - 48;
        value_fertilizer_status = !value_fertilizer_status;
    }

    if (strcmpPointer(type, "DT"))
    {
        char *token;
        /* get the first token */
        token = strtok(message, ",");
        if (token == NULL)
        {
            return;
        }

        char *xDate = token;

        /* walk through other tokens */
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return;
        }
        char *xTime = token;

        // Serial.println(xDate);
        // Serial.println(xTime);

        setRTC(xDate, xTime);
    }

    if (strcmpPointer(type, "SC1"))
    {
        char *token;

        unsigned int address = EEPROM_SCHEDULE_ADDRESS_1;
        Schedule schedule;
        token = strtok(message, ",");

        if (token == NULL)
        {
            return;
        }
        unsigned char isEnd = 0;
        unsigned int size = 0;
        /* walk through other tokens */
        while (token != NULL)
        {
            unsigned long sod = atol(token);
            if (!isEnd)
            {
                schedule.start.word32 = sod;
            }
            else
            {
                schedule.end.word32 = sod;
                size++;
                saveSchedule(address, schedule);
            }
            isEnd = !isEnd;
            token = strtok(NULL, ",");
        }

        EEPROM.update(EEPROM_SCHEDULE_SIZE_ADDRESS_1, size);
    }

    if (strcmpPointer(type, "SC2"))
    {
        char *token;

        unsigned int address = EEPROM_SCHEDULE_ADDRESS_2;
        Schedule schedule;
        token = strtok(message, ",");
        if (token == NULL)
        {
            return;
        }
        unsigned char isEnd = 0;
        unsigned int size = 0;
        /* walk through other tokens */
        while (token != NULL)
        {
            unsigned long sod = atol(token);
            if (!isEnd)
            {
                schedule.start.word32 = sod;
            }
            else
            {
                schedule.end.word32 = sod;
                size++;
                saveSchedule(address, schedule);
            }
            isEnd = !isEnd;
            token = strtok(NULL, ",");
        }

        EEPROM.update(EEPROM_SCHEDULE_SIZE_ADDRESS_2, size);
    }
}
void setup()
{
    Serial.begin(9600);

    pinMode(PUMP_PIN, OUTPUT);
    pinMode(VALVE_PIN_1, OUTPUT);
    pinMode(VALVE_PIN_2, OUTPUT);
    pinMode(VALVE_FERTILIZER_PIN, OUTPUT);
    pinMode(VALVE_IN_PIN, OUTPUT);

    pinMode(LED_POWER_PIN, OUTPUT);
    pinMode(LED_INTERNET_PIN, OUTPUT);
    pinMode(LED_AUTO_PIN, OUTPUT);

    digitalWrite(PUMP_PIN, OFF);
    digitalWrite(VALVE_PIN_1, OFF);
    digitalWrite(VALVE_PIN_2, OFF);
    digitalWrite(VALVE_FERTILIZER_PIN, OFF);
    digitalWrite(VALVE_IN_PIN, OFF);

    uartInit(serialCallback);
    RTCInit();
    initMoisture();
    timer1InterruptInit();
    ulInit();
    attachInterrupt(digitalPinToInterrupt(FLOW_FERTILIZER_PIN), flow1ISR, FALLING);

    if (isFirstTime())
    {
        Serial.println("FIRST TIME");
        EEPROM.put(EEPROM_FERTILIZERUSED, flowCounter1);
        EEPROM.put(EEPROM_WATERRUSED, waterUsed);
        
    }

    writeFirstTime();

    EEPROM.get(EEPROM_FERTILIZERUSED, flowCounter1);
    EEPROM.get(EEPROM_WATERRUSED, waterUsed);

    wdt_enable(WDTO_8S);
}
void loop()
{
    digitalWrite(LED_POWER_PIN, HIGH);
    static unsigned char timer3sCounter = 0;
    static unsigned char timer10sCounter = 0;

    if (timer1sFlag)
    {
        timer1sFlag = 0;

        if (!isTankEmpty)
        {
            digitalWrite(VALVE_IN_PIN, OFF);
            if (isAuto)
            {
                digitalWrite(LED_AUTO_PIN, HIGH);
                long secOfDay = getSecOfDay();
                Serial.println(secOfDay);
                if (isOnSchedule1(secOfDay))
                {
                    value1_status = ON;
                }
                else
                {
                    value1_status = OFF;
                }

                if (isOnSchedule2(secOfDay))
                {
                    value2_status = ON;
                }
                else
                {
                    value2_status = OFF;
                }
            }
            else
            {
                digitalWrite(LED_AUTO_PIN, LOW);
                // digitalWrite(PUMP_PIN,pump_status);
                if (value1_control)
                {
                    value1_status = ON;
                }
                else
                {
                    value1_status = OFF;
                }

                if (value2_control)
                {
                    value2_status = ON;
                }
                else
                {
                    value2_status = OFF;
                }
            }
        }
        else
        {
            value1_status = OFF;
            value2_status = OFF;
            value_fertilizer_status = OFF;
            digitalWrite(VALVE_IN_PIN, ON);
            Serial.println("tank EMPTY");
            long distance = getDistanceCM();
            if (distance < SENSOR_TOP_DISTANCE)
            {
                isTankEmpty = 0;
            }
        }
        timer3sCounter++;
        timer10sCounter++;
        readMoisture(moisture1, moisture2);

        writeEEPROM(EEPROM_FERTILIZERUSED,flowCounter1);
        // Serial.println(flowCounter1);
        //   Serial.println(moisture1);
        //    Serial.println(moisture2);
    }
    if (timer3sCounter > 3)
    {
        float fertilizerUsed = (float)flowCounter1 / 300 * 1000;

        Serial3.print("FIT DA;");
        Serial3.print(String(isTankEmpty));
        Serial3.print(",");
        Serial3.print(String(isAuto));
        Serial3.print(",");
        Serial3.print(String(!pump_status));
        Serial3.print(",");
        Serial3.print(String(!value1_status));
        Serial3.print(",");
        Serial3.print(String(!value2_status));
        Serial3.print(",");
        Serial3.print(String(!value_fertilizer_status));
        Serial3.print(",");
        Serial3.print(moisture1);
        Serial3.print(",");
        Serial3.print(moisture2);
        Serial3.print(",");
        Serial3.print(fertilizerUsed);
        Serial3.print(",");
        Serial3.print(waterUsed);
        Serial3.print("\n");
        timer3sCounter = 0;
    }

    if (timer10sCounter > 10)
    {
        timer10sCounter = 0;

        long curDistance = getDistanceCM();
        
        if (curDistance > TANK_HEIGHT - SENSOR_BOTTOM_DISTANCE)
        {
            isTankEmpty = 1;
        }
        if (prevDistance <= curDistance)
        {
            if(!isTankEmpty){
                Serial.print("distance : ");
                Serial.println(curDistance - prevDistance);
                if(curDistance - prevDistance<5){
                     waterUsed = waterUsed + getVolume(curDistance - prevDistance);

                        writeEEPROM(EEPROM_WATERRUSED,waterUsed);
                } 
            }
          
        }

        prevDistance = curDistance;
    }
   
    if (value1_status == ON || value2_status == ON)
    {   
        pump_status = ON;
    
    }
    else
    {
        pump_status = OFF;
        digitalWrite(PUMP_PIN, OFF);
    }

    digitalWrite(PUMP_PIN, pump_status);
    digitalWrite(VALVE_PIN_1, value1_status);
    digitalWrite(VALVE_PIN_2, value2_status);
    digitalWrite(VALVE_FERTILIZER_PIN, value_fertilizer_status);
    uartLoop();

    wdt_reset();
}

void timer1InterruptInit()
{
    cli(); //stop interrupts

    TCCR1A = 0; // set entire TCCR3A register to 0
    TCCR1B = 0; // same for TCCR3B
    TCNT1 = 0;  //initialize counter value to 0

    TCCR1B |= (1 << CS10) | (1 << CS12) | (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = 15625;

    sei();
}

ISR(TIMER1_COMPA_vect)
{

    timer1sFlag = 1;
}

unsigned char isOnSchedule1(long secOfDay)
{
    unsigned int address = EEPROM_SCHEDULE_SIZE_ADDRESS_1;
    int scheduleSize = EEPROM.read(address);
    // Serial.println(scheduleSize);
    address++;
    for (int i = 0; i < scheduleSize; i++)
    {
        Schedule schedule = readSchedule(address);
        // Serial.print(schedule.start.word32);
        // Serial.print("  ");
        // Serial.println(schedule.end.word32);
        if (secOfDay >= schedule.start.word32 && secOfDay <= schedule.end.word32)
        {
            return 1;
        }
    }
    return 0;
}

unsigned char isOnSchedule2(long secOfDay)
{
    unsigned int address = EEPROM_SCHEDULE_SIZE_ADDRESS_2;
    int scheduleSize = EEPROM.read(address);
    // Serial.println(scheduleSize);
    address++;
    for (int i = 0; i < scheduleSize; i++)
    {
        Schedule schedule = readSchedule(address);
        // Serial.print(schedule.start.word32);
        // Serial.print("  ");
        // Serial.println(schedule.end.word32);
        if (secOfDay >= schedule.start.word32 && secOfDay <= schedule.end.word32)
        {
            return 1;
        }
    }
    return 0;
}

void flow1ISR()
{
    flowCounter1++;
}

void writeEEPROM(unsigned int address,unsigned long data)
{
    long temp;
    EEPROM.get(address, temp);
    if (temp != data)
    {
        Serial.println("WRITING EEPROM");
        EEPROM.put(address, data);
    }
}

char isFirstTime()
{
    for (int i = 0; i < 20; i++)
    {
        if (EEPROM.read(EEPROM_IS_FIRST_TIME_ADDRESS + i) != 64)
        {
            return 1;
        }
    }
    return 0;
}

void writeFirstTime()
{
    for (int i = 0; i < 20; i++)
    {
        EEPROM.write(EEPROM_IS_FIRST_TIME_ADDRESS + i, 64);
    }
}