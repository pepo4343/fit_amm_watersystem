

#include "uart.h"
#include "function.h"

#define BUFFER_SIZE 200

void (*uartCallback)(char *type, char *message);

char buffer[BUFFER_SIZE];

char startWord[] = "FIT ";

SoftwareSerial NodeSerial(D2, D3);

char *type;
char *message;
void uartInit(void (*callback)(char *, char *))
{
    NodeSerial.begin(9600);
    uartCallback = callback;
}

void uartLoop()
{
    while (NodeSerial.available() > 0)
    {

        static unsigned char startIndex = 0;
        static unsigned char index = 0;
        char inByte = (char)NodeSerial.read();

         Serial.println(inByte);
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

                if (token == NULL)
                {   
                    memset(buffer, 0, BUFFER_SIZE);
                    index = 0;
                    isWord = 0;

                    return;
                }
                type = token;

                /* walk through other tokens */
                token = strtok(NULL, ";");
                if (token == NULL)
                {
                    memset(buffer, 0, BUFFER_SIZE);
                    index = 0;
                    isWord = 0;

                    return;
                }

                message = token;

                uartCallback(type, message);
                memset(buffer, 0, BUFFER_SIZE);

                index = 0;
                isWord = 0;

                return;
            }
            if (index >= BUFFER_SIZE)
            {
                index = 0;
            }
            buffer[index] = inByte;
            index++;
        }
    }
}

void writeSerial(char *message)
{
    while (*message != '\0')
    {
        NodeSerial.write(*message);
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
