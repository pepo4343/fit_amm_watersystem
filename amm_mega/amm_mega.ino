#include "volume.h"
#include "oled.h"
#include "config.h"
#include "rtc.h"
#include "uart.h"
#include "schedule.h"
#include "function.h"

unsigned int currentPercent;

char timer500msFlag = 0;
char timer1sFlag = 0;
char timer5sCounter = 0;
unsigned char isOnSchedule(long secOfDay);

long button1Time = 0;
unsigned char button1Previous = HIGH;

unsigned char pump = OFF;
unsigned char valve = 0;

unsigned int moisure1 = 0;
unsigned int moisure2 = 0;

unsigned long flowCounter1 = 0;
unsigned long flowCounter2 = 0;

unsigned char isAuto = 0;

float waterUsed = 0;
float fertilizerUsed = 0;

float fertilizeNeed = 0;

void setup()
{

  Serial.begin(9600);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);

  pinMode(BUTTON_PUMP, INPUT_PULLUP);
  pinMode(BUTTON_VALVE, INPUT_PULLUP);
  pinMode(SWITCH_MODE, INPUT_PULLUP);

  digitalWrite(PUMP_PIN, OFF);
  digitalWrite(VALVE_PIN, OFF);

  pinMode(MOISTURE_1_PIN, INPUT);
  pinMode(MOISTURE_2_PIN, INPUT);

  pinMode(FLOW_1_PIN, INPUT_PULLUP);
  pinMode(FLOW_2_PIN, INPUT_PULLUP);

  // EEPROM.put(EEPROM_WATERUSED, 0);
  // EEPROM.put(EEPROM_FERTILIZERUSED, 0);
  EEPROM.get(EEPROM_WATERUSED, flowCounter1);
  EEPROM.get(EEPROM_FERTILIZERUSED, flowCounter2);

  if (flowCounter1 == 0xFFFFFFFF && flowCounter2 == 0xFFFFFFFF)
  {
    Serial.println("first time");
    EEPROM.put(EEPROM_WATERUSED, 0L);
    EEPROM.put(EEPROM_FERTILIZERUSED, 0L);

    EEPROM.get(EEPROM_WATERUSED, flowCounter1);
    EEPROM.get(EEPROM_FERTILIZERUSED, flowCounter2);
  }

  Serial.println(flowCounter1);
  Serial.println(flowCounter2);
  attachInterrupt(digitalPinToInterrupt(FLOW_1_PIN), flow1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(FLOW_2_PIN), flow2ISR, FALLING);
  digitalWrite(PUMP_PIN, OFF);

  // Serial.println(waterUsed == 12.14);
  // put your setup code here, to run once:
  initUS();
  oledInit();
  timer1InterruptInit();
  RTCInit();
  uartInit(serialCallback);
}

void loop()
{
  isAuto = digitalRead(SWITCH_MODE);

  // // put your main code here, to run repeatedly:
  if (timer500msFlag)
  {

    waterUsed = (float)flowCounter1 / 300;
    fertilizerUsed = (float)flowCounter2 / 300;
    //run every 500ms
    timer500msFlag = 0;
    moisure1 = analogRead(MOISTURE_1_PIN);
    moisure2 = analogRead(MOISTURE_2_PIN);
    getPercent(currentPercent);
    oledWrite();

    // Serial.println(currentPercent);
  }
  if (timer1sFlag)
  {
    timer5sCounter++;
    timer1sFlag = 0;
    long secOfDay = getSecOfDay();
    Serial.println(secOfDay);
    writeEEPROM();

    if (isAuto)
    {
      if (isOnSchedule(secOfDay))
      {
        pump = ON;
      }
      else
      {
        pump = OFF;
      }
    }
  }

  if (timer5sCounter >= 5)
  {
    Serial.println("SEndding");
    timer5sCounter = 0;
    writeSerial("STA;DA;");
    char mqttMessage[25];
    sprintf(mqttMessage, "%d,%d,%d,%d,%d,%d.%02d,%d.%02d",
            isAuto,
            pump,
            valve,
            moisure1,
            moisure2,
            (int)waterUsed,
            (int)(waterUsed * 100) % 100,
            (int)fertilizerUsed,
            (int)(fertilizerUsed * 100) % 100);
    writeSerial(mqttMessage);
    writeSerial("\n");
    memset(mqttMessage, 0, 40);
  }
  if (isAuto)
  {
    if (flowCounter2 < fertilizeNeed)
    {
      valve = ON;
    }
    else
    {
      valve = OFF;
    }
  }
  if (!isAuto)
  {

    char button1status = digitalRead(BUTTON_PUMP);
    Serial.println((int)button1status);
    if (button1status == HIGH && button1Previous == LOW && millis() - button1Time > BUTTON_DEBOUNCE)
    {
      Serial.println("Clicked");
      pump = !pump;
      button1Time = millis();
    }
    button1Previous = button1status;

    if (!digitalRead(BUTTON_VALVE))
    {
      valve = ON;
    }
    else
    {
      valve = OFF;
    }
  }

  if (currentPercent < 5)
  {

    pump = OFF;
  }
  digitalWrite(PUMP_PIN, pump);
  digitalWrite(VALVE_PIN, valve);
  setHeight();
  uartLoop();
}

////////////////////////////
///// Serial Callback///////
////////////////////////////

void serialCallback(char *type, char *message)
{
  if (strcmpPointer(type, "DT"))
  {
    char *token;
    /* get the first token */
    token = strtok(message, ",");

    char *xDate = token;

    /* walk through other tokens */
    token = strtok(NULL, ",");

    char *xTime = token;

    // Serial.println(xDate);
    // Serial.println(xTime);

    setRTC(xDate, xTime);
  }

  if (strcmpPointer(type, "SC"))
  {
    char *token;

    unsigned int sizeAddress = 0;

    unsigned int address = sizeAddress + 1;

    Schedule schedule;
    token = strtok(message, ",");
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

    EEPROM.update(sizeAddress, size);
  }

  if (strcmpPointer(type, "FT"))
  {
    long fertilizerCounterNeed = atol(message);
    fertilizerCounterNeed = fertilizerCounterNeed/1000*300;
    Serial.println(fertilizerCounterNeed);
    fertilizeNeed = flowCounter2 + fertilizerCounterNeed;
  }
}

////////////////////
/////// ISR  ///////
////////////////////

void flow1ISR()
{
  flowCounter1++;
}

void flow2ISR()
{
  flowCounter2++;
}

ISR(TIMER1_COMPA_vect)
{
  static char timer1sCounter = 0;

  timer1sCounter++;
  if (timer1sCounter > 2)
  {
    timer1sCounter = 0;
    timer1sFlag = 1;
  }
  timer500msFlag = 1;
}

void timer1InterruptInit()
{
  cli(); //stop interrupts

  TCCR1A = 0; // set entire TCCR3A register to 0
  TCCR1B = 0; // same for TCCR3B
  TCNT1 = 0;  //initialize counter value to 0

  TCCR1B |= (1 << CS10) | (1 << CS12) | (1 << WGM12);
  TIMSK1 |= (1 << OCIE1A);
  OCR1A = 7812;

  sei();
}

////////////////////////////////
/////// Utils Function /////////
////////////////////////////////

unsigned char isOnSchedule(long secOfDay)
{
  unsigned int address = 0;
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

void writeEEPROM()
{
  long temp;
  EEPROM.get(EEPROM_WATERUSED, temp);
  if (temp != flowCounter1)
  {
    Serial.println("WRITING EEPROM");
    EEPROM.put(EEPROM_WATERUSED, flowCounter1);
  }

  EEPROM.get(EEPROM_FERTILIZERUSED, temp);
  if (temp != flowCounter2)
  {
    Serial.println("WRITING EEPROM");
    EEPROM.put(EEPROM_FERTILIZERUSED, flowCounter2);
  }
}

// void btnPress(unsigned int pin)
// {
//   static long time = 0;
//   static char previous = HIGH;
//   long debounce = 100; // the debounce time, increase if the output flickers
//   char reading = digitalRead(pin);

//   if (reading == HIGH && previous == LOW && millis() - time > debounce)
//   {
//     Serial.println("Pin");
//     Serial.println(pin);
//     time = millis();
//   }
//   previous = reading;
// }