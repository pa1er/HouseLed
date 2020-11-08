
#include <Adafruit_NeoPixel.h>



//////// Config of wifi

//const char* ssid     = "WIFI_SSID";
//const char* password = "WIFI_PASSWORD";
const char* ssid     = "Roggekamp24";
const char* password = "22868209223378290592";


//////// Config of webserver



///////// Config of the neopixel (ws2812b / ws 2811b)

#define LED_PIN    14    // GPIO14 = D5
#define LED_COUNT 160
unsigned char movement = D2; // Movement input

//////// Info on domoticz

char domoIP[20] = "192.168.112.12";
int movementIDX = 56;
int lightIDX = 99;     // Domoticz IDX 48 licht intentiteit buiten, Licht hal = XX (device idx in domotics



//////// Config other
#define  Bh1750addr   0x23    // ook 0x5c is een optie 0x23
