#include "schedule.h"
void saveSchedule(unsigned int &address, Schedule schedule)
{
    EEPROM.update(address, schedule.start.lsb);
    address++;
    EEPROM.update(address, schedule.start.midL);
    address++;
    EEPROM.update(address, schedule.start.midH);
    address++;
    EEPROM.update(address, schedule.start.msb);
    address++;

    EEPROM.update(address, schedule.end.lsb);
    address++;
    EEPROM.update(address, schedule.end.midL);
    address++;
    EEPROM.update(address, schedule.end.midH);
    address++;
    EEPROM.update(address, schedule.end.msb);
    address++;
}

void saveOneSchedule(unsigned int address, Schedule schedule)
{
    EEPROM.update(address, schedule.start.lsb);
    address++;
    EEPROM.update(address, schedule.start.midL);
    address++;
    EEPROM.update(address, schedule.start.midH);
    address++;
    EEPROM.update(address, schedule.start.msb);
    address++;

    EEPROM.update(address, schedule.end.lsb);
    address++;
    EEPROM.update(address, schedule.end.midL);
    address++;
    EEPROM.update(address, schedule.end.midH);
    address++;
    EEPROM.update(address, schedule.end.msb);
    address++;
}

Schedule readSchedule(unsigned int &eeAddress)
{

    Schedule schedule;
    schedule.start.lsb = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.start.midL = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.start.midH = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.start.msb = EEPROM.read(eeAddress);

    eeAddress++;
    schedule.end.lsb = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.end.midL = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.end.midH = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.end.msb = EEPROM.read(eeAddress);
    eeAddress++;
    return schedule;
}


Schedule readOneSchedule(unsigned int eeAddress)
{

    Schedule schedule;
    schedule.start.lsb = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.start.midL = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.start.midH = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.start.msb = EEPROM.read(eeAddress);

    eeAddress++;
    schedule.end.lsb = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.end.midL = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.end.midH = EEPROM.read(eeAddress);
    eeAddress++;
    schedule.end.msb = EEPROM.read(eeAddress);
    eeAddress++;
    return schedule;
}