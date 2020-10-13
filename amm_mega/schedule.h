#ifndef _SCHEDULE_H
#define _SCHEDULE_H


#include <EEPROM.h>


struct Schedule
{
  union {
    unsigned long int word32;
    struct
    {
      unsigned int lsb16;
      unsigned int msb16;
    };
    struct
    {
      unsigned char lsb;
      unsigned char midL;
      unsigned char midH;
      unsigned char msb;
    };
  } start;
  union {
    unsigned long int word32;
    struct
    {
      unsigned int lsb16;
      unsigned int msb16;
    };
    struct
    {
      unsigned char lsb;
      unsigned char midL;
      unsigned char midH;
      unsigned char msb;
    };
  } end;
};


extern void saveSchedule(unsigned int& address , Schedule schedule) ;
extern void saveOneSchedule(unsigned int address , Schedule schedule) ;

extern Schedule readSchedule(unsigned int &eeAddress);
extern Schedule readOneSchedule(unsigned int eeAddress);



#endif