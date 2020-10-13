

#include <Ticker.h>
#include <SoftwareSerial.h>
// SoftwareSerial NodeSerial(D2, D3); // RX | TX

#include "config.h"
#include "mqtt.h"
#include "wifi.h"
#include "ntp.h"
#include "function.h"
#include "uart.h"

#define BUFFER_SIZE 200

WiFiClient wifiClient;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Ticker isr;

//status
unsigned char internetStatus = 0;

volatile unsigned char timer1sFlag = 0;
volatile unsigned char timer5sFlag = 0;
volatile unsigned char timer10sFlag = 0;
void inline timerInterrupt(void);

unsigned long lastMsg = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(READY_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // put your setup code here, to run once:

  isr.attach(1, timerInterrupt);
  internetInit();
  mqttInit();
  ntpInit();
  uartInit(serialCallback);
  digitalWrite(READY_PIN, LOW);
  Serial.println("start");
}


void serialCallback(char* type, char* message)
{
 
  // Serial.println(message);
  // Serial.println(type);

   if (strcmpPointer(type, "DA")){
     Serial.println(message);
       char buffer[BUFFER_SIZE];
       char index = 0;
       while (*message != '\0')
       {
        buffer[index] =  *message;
        message++;
        index++;
       }
       buffer[index] = '\0';
       Serial.println(buffer);
       mqttClient.publish("fit/amm/" ALIAS "/watersystem/data", buffer);
   }


}

void loop()
{

  // put your main code here, to run repeatedly:
  // client.loop();
  if (timer1sFlag)
  {
    timer1sFlag = 0;
    // Serial.println("runing");
    
  }

  if (timer5sFlag)
  {
    timer5sFlag = 0;

    internetStatus = isInternetConnected();

    if (internetStatus)
    {
      //Have internet connection
      mqttConnect();
    }
    else
    {
      // Serial.println("No Internet Connection");
      internetInit();
    }

    long now = millis();
    // Serial.print("LastMsg:  ");
    // Serial.println(now - lastMsg);
    if (now - lastMsg > 600000)
    {
      ESP.restart();
    }
  }
  if (timer10sFlag)
  {
    timer10sFlag = 0;
    if (internetStatus)
    {
      sendNTPpacket();
    }
  }
  mqttLoop();
  uartLoop();
  ntpLoop();
}

void inline timerInterrupt(void)
{
  static char timer5sCounter = 0;
  static char timer10sCounter = 0;

  timer1sFlag = 1;
  timer10sCounter++;
  timer5sCounter++;
  if (timer10sCounter > 10)
  {
    timer10sCounter = 0;
    timer10sFlag = 1;
  }
  if (timer5sCounter > 5)
  {
    timer5sCounter = 0;
    timer5sFlag = 1;
  }
}
