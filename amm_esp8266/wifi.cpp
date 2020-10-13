#include "config.h"


extern WiFiClient wifiClient;

const char google[] = "www.google.com";

void internetInit()
{ // We start by connecting to a WiFi network
    // Serial.println();
    // Serial.print("Connecting to ");
    // Serial.println(ssid);

    // WiFi.config(ip, dns, gateway, subnet);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        // Serial.print(".");
    }

    // Serial.println("");
    // Serial.println("WiFi connected");
    // Serial.println("IP address: ");
    // Serial.println(WiFi.localIP());
};

char isInternetConnected()
{

    if (wifiClient.connect(google, 80))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}