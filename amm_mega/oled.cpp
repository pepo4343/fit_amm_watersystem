#include "oled.h"

#define SCREEN_WIDTH 127
#define SCREEN_HEIGHT 31
#define TANK_WIDTH 20
#define TANK_HEIGHT 10

#define LOGO_X 98
#define LOGO_Y 5

Adafruit_SSD1306 display(OLED_RESET);

extern unsigned int currentPercent;

extern unsigned int moisure1;
extern unsigned int moisure2;

extern float waterUsed;
extern float fertilizerUsed;
extern unsigned char isAuto;

inline void drawTextStatus();
inline void drawWaterLevelStatus();
inline void drawModeText();
void writeText(char *text);
void oledInit()
{
    display.begin(SSD1306_SWITCHCAPVCC, 0x3c); //initialize I2C addr 0x3c
}

void oledWrite()
{
    display.clearDisplay(); // clears the screen and buffer
    drawWaterLevelStatus();
    drawTextStatus();
    drawModeText();
    display.display();
}

inline void drawModeText()
{
    display.setCursor(60, 22);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    // if (isAuto)
    // {
    //     writeText("AUTO");
    // }
    // else{
    //     writeText("MANAUL");
    // }
}

inline void drawTextStatus()
{

    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    char buff[20];

    sprintf(buff, "W.USED : %d.%02dL", (int)waterUsed, (int)(waterUsed * 100) % 100);
    writeText(buff);
    memset(buff, 0, 20);

    sprintf(buff, "F.USED : %d.%02dL", (int)fertilizerUsed, (int)(fertilizerUsed * 100) % 100);
    writeText(buff);
    memset(buff, 0, 20);

    sprintf(buff, "MS1 : %d", moisure1);
    writeText(buff);
    memset(buff, 0, 20);

    sprintf(buff, "MS2 : %d", moisure2);
    writeText(buff);
    memset(buff, 0, 20);
}

inline void drawWaterLevelStatus()
{

    Serial.println(currentPercent);
    int waterHeight = map(currentPercent, 0, 100, 0, 30);
    display.drawRect(95, 0, 30, 30, WHITE);

    display.fillRect(95, 30 - waterHeight, 30, waterHeight, WHITE);

    display.drawLine(95, 0, 105, 0, BLACK);
    display.drawLine(115, 0, 125, 0, BLACK);

    display.drawLine(95, 1, 105, 1, BLACK);
    display.drawLine(115, 1, 125, 1, BLACK);

    display.drawLine(95, 2, 97, 2, BLACK);
    display.drawLine(123, 2, 125, 2, BLACK);

    display.drawLine(95, 29, 97, 29, BLACK);
    display.drawLine(123, 29, 125, 29, BLACK);

    display.drawLine(97, 2, 105, 2, WHITE);
    display.drawLine(115, 2, 123, 2, WHITE);

    display.drawPixel(105, 1, WHITE);
    display.drawPixel(115, 1, WHITE);

    //FIT text
    display.setTextColor(BLACK);
    display.setTextSize(1);
    display.setCursor(102, 12);
    writeText("FIT");
}

void writeText(char *text)
{
    while (*text != '\0')
    {
        display.write(*text);
        text++;
        /* code */
    }
    display.write('\n');
}
