#include "rtc.h"



#define SECOND_TICK 67L
#define countof(a) (sizeof(a) / sizeof(a[0]))

// ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST); // IO, SCLK, CE
// RtcDS1302<ThreeWire> Rtc(myWire);





const char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server

RtcDS3231<TwoWire> Rtc(Wire);

static const char mstring[][4] = {
    "   ", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

EthernetUDP Udp;

unsigned int localPort = 8888;

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

void printDateTime(char* date )
{

  RtcDateTime now = Rtc.GetDateTime();

  snprintf_P(date,
              20,
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             now.Month(),
             now.Day(),
             now.Year(),
             now.Hour(),
             now.Minute(),
             now.Second());
  Serial.print(date);
}

void setRTC(char *Date, char *Time)
{
  // Rtc.Begin();
  RtcDateTime compile = RtcDateTime(Date, Time);
  Rtc.SetDateTime(compile);
};

void getRTC(int *currentHour, int *currentMinute, int *currentSecond)
{

  RtcDateTime dt = Rtc.GetDateTime();
  *currentHour = dt.Hour();
  *currentMinute = dt.Minute();
  *currentSecond = dt.Second();
};

void RTCInit()
{
  //Init RTC
  Rtc.Begin();

//  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
//  Serial.println(compiled);
//  Rtc.SetDateTime(compiled);
};

char RTCisValid()
{
  RtcDateTime now = Rtc.GetDateTime();
  if (!now.IsValid())
  {
    // Common Causes:
    //    1) the battery on the device is low or even missing and the power line was disconnected
    Serial.println("RTC lost confidence in the DateTime!");
    return '1';
  }
  return '0';
}

void udpInit()
{

  Udp.begin(localPort);
};

void sendNTPpacket()
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(timeServer, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
  int counter = 0;
  while (1)
  {
    Serial.println("waiting........");
    if (Udp.parsePacket())
    {
      setRTCFromNTP();
      break;
    }
    if (counter > 60)
    { //timeout after 1 minute if doesn't recieve udp packet back

      break;
    }
    counter++;
    vTaskDelay(SECOND_TICK);
  }
};

void setRTCFromNTP()
{
  // We've received a packet, read the data from it
  Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  // the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, extract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  // now convert NTP time into everyday time:

  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  time_t epoch = secsSince1900 - seventyYears + 25200; //+7 GMT

  //Set RTC with NTP
  char date[20];
  snprintf_P(date,
             countof(date),
             PSTR("%03s %02u %04u"),
             mstring[month(epoch)],
             day(epoch),
             year(epoch));
  Serial.println(date);
  char xtime[20];
  snprintf_P(xtime,
             countof(xtime),
             PSTR("%02u:%02u:%02u"),
             hour(epoch),
             minute(epoch),
             second(epoch));
  Serial.println(xtime);
  setRTC(date, xtime);
};


long getSecOfDay(){
  RtcDateTime now = Rtc.GetDateTime();
  long secOfDay = ( (long) now.Hour())*3600;
  secOfDay += ( (long) now.Minute())*60;
  secOfDay += ( (long) now.Second());

  return secOfDay;
};
