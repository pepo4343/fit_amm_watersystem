#include "function.h"

String splitString(String &str)
{
    String results;
    if (str.indexOf(",") == -1)
    {
        results = str;
        str = "";
        return results;
    }
    results = str.substring(0, str.indexOf(","));

    str.replace(str.substring(0, str.indexOf(",") + 1), "");

    return results;
}

char strcmpPointer(char *strg1, char *strg2)
{
    while( ( *strg1 != '\0' && *strg2 != '\0' ) && *strg1 == *strg2 )
    {
        strg1++;
        strg2++;
    }
 
    if(*strg1 == *strg2)
    {
        return 1; // strings are identical
    }
 
    else
    {
        return 0;
    }
}