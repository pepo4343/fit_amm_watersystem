#include "uart.h"
#include "function.h"
#include "config.h"
SoftwareSerial megaUART(10, 11);
void (*uartCallback)(char *type, char *message);

char buffer[200];

char startWord[] = "STA;";
char *type;
char *message;
void uartInit(void (*callback)(char *, char *))
{
    megaUART.begin(19200);
    uartCallback = callback;
}

void uartLoop()
{

    while (megaUART.available() > 0)
    {

        static unsigned char startIndex = 0;
        static unsigned char index = 0;
        char inByte = (char)megaUART.read();

        static unsigned char isWord = 0;
        if (!isWord)
        {
            if (inByte == startWord[index])
            {
                index++;
            }
            else
            {
                index = 0;
            }

            if (index > 3)
            {
                index = 0;
                isWord = 1;
            }
        }

        else
        {
            if (inByte == '\n')
            {
                Serial.println(buffer);
                char *token;
                /* get the first token */
                token = strtok(buffer, ";");

                type = token;

                /* walk through other tokens */
                token = strtok(NULL, ";");

                message = token;

                uartCallback(type, message);
                memset(buffer, 0, 200);

                index = 0;
                isWord = 0;

                return;
            }
            if (index >= 200)
            {
                index = 0;
            }
            buffer[index] = inByte;
            index++;
        }

        // if (index > 3)
        // {
        //     Serial.print(inByte);
        // }
    }
}



void writeSerial(char *message)
{
    while (*message != '\0')
    {
        megaUART.write(*message);
        message++;
        /* code */
    }
}
// void createStr(char * results,char *str )
// {
//     size_t len = strlen(str);
//     char *str2 = malloc(len + 1 + 1); /* one for extra char, one for trailing zero */
//     strcpy(str2, str);
//     str2[len] = c;
//     str2[len + 1] = '\0';
//     results = str2;
//     free( str2 );
// }