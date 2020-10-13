#include "moisture.h"


void initMoisture()
{
    pinMode(MOISURE_1_PIN, INPUT);
    pinMode(MOISURE_2_PIN, INPUT);
}

void readMoisture(float &moisture1, float &moisture2)
{
    int moisture1Analog = analogRead(MOISURE_1_PIN);
    int moisture2Analog = analogRead(MOISURE_2_PIN);

    moisture1 = -0.0155 * (float)moisture1Analog + 13.587;
    moisture2 = -0.0155 * (float)moisture2Analog + 13.587;
}

