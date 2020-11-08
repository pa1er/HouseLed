#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>   // Voor de i2c bus
#include <BH1750.h>
#include <i2cdetect.h>
#include <math.h>
#include <Stdio.h>
//#include <Bridge.h>
#include <ESP8266HTTPClient.h>

#include "config.h"

// used example: https://www.electronics-lab.com/project/nodemcu-esp8266-webserver-tutorial/
// NodeMCU: https://www.instructables.com/id/Getting-Started-With-ESP8266LiLon-NodeMCU-V3Flashi/
// Neopixel: https://github.com/adafruit/Adafruit_NeoPixel
// BH1750: https://create.arduino.cc/projecthub/infoelectorials/project-017-arduino-bh1750-light-sensor-project-640075
// HTTP Request: http://domoticx.com/esp8266-wifi-http-get-request-maken-arduinoide/



//Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Also done in config file
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

WiFiServer server(80);// Set port to 80
String header; // This storees the HTTP request
byte buff[2];

String greenstate = "off";// state of green LED
String redstate = "off";// state of red LED
String bluestate = "off";
String selectLeds;
int typeCommend;
int red = 0;
int green = 0;
int blue = 0;
int ind1; // as index for splitting the url
int ind2;
int ind3;
int ind4;
int ind5;
int ind6;
int cmCounter = 0;
int arrayPref[20];
//int selectedLeds[200];
int movementState = 2;
//int movementIDX = 56;
int cmDelay = 0;
bool luxMeter;
bool redUP = false; // Updown for the fade
bool greenUP = false;
bool blueUP = false;
bool fade = false;
bool christmas = false;
uint32_t stripcolor;
BH1750 lightMeter;
int lightCount=0;     // Counter for interval in sending light intensity to Domoticz
//int lightIDX = 99;     // Domoticz IDX 48 licht intentiteit beuiten, Licht hal = XX (device idx in domotics
char domoURL[100];
HTTPClient http;

void setup() {
  // Setup common:
  Serial.begin(115200);  // using LoLin NodeMCU at 9600
  Serial.println("HouseLed starting up.....");

  // Setup wifi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());// this will display the Ip address of the Pi which should be entered into your browser
  server.begin();

  // Setup neopixel
  Serial.print("Neopixel setup");
  strip.begin();
  strip.show(); // all pixels to off
  Serial.println("...... Done");

  // i2c bus
  Wire.begin(D2,D1);
//  BH1750_Init(Bh1750addr);
//  delay(200);
  i2cdetect();
  luxMeter = lightMeter.begin();
  Serial.print(F("BH1750 Init "));
  Serial.println(luxMeter);
  delay(2000);

// Movement sonsor
  pinMode(movement, INPUT);
//  attachInterrupt(0, flow, RISING); // see http://arduino.cc/en/Reference/attachInterrupt
//  sei();                            // Enable interrupts
}

void loop() {
   
  // Webserver is used to get commands from Domoticz

  int i;
  uint16_t val1=0;

  // Check movement input
  int value = digitalRead(movement);
  if (value == HIGH) {
      if (movementState != HIGH) {
        movementState = HIGH;
        Serial.println("Movement to HIGH");

      }
  } else {
     if (movementState != LOW) {
        movementState = LOW;
        Serial.println("Movement to LOW");
        
        sprintf(domoURL,"http://%s/json.htm?type=command&param=switchlight&idx=%i&switchcmd=On", domoIP, movementIDX) ;
        http.begin(domoURL);
        int httpResponseCode = http.GET();  // Fire and forget: No check on good reception of the call
    
        Serial.println(domoURL); 
        Serial.println(httpResponseCode);
     }
  }

  if (fade) {
    doFade();
    delay (500);
  }
  if (christmas) {
    doChristmas();
     delay (500);    
  }

  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

 // We do something with the received command
 
           if (header.indexOf("GET /off") >= 0) {
             Serial.println("all off");
             fade=false;
             christmas = false;
             red = 0;
             green = 0;
             blue = 0;    
             typeCommend = 1; //"normal";        
           } else if (header.indexOf("GET /green/on") >= 0) {
             Serial.println("green on");
            greenstate = "on";
            green=255;
            typeCommend = 1; //normal 

           } else if (header.indexOf("GET /green/off") >= 0) {
             Serial.println("green off");
             greenstate = "off";
             green=0;
             typeCommend = 1; 
           } else if (header.indexOf("GET /red/on") >= 0) {
             Serial.println("red on");
             redstate = "on";
             red=255;
             typeCommend = 1; 
           } else if (header.indexOf("GET /red/off") >= 0) {
             Serial.println("red off");
             redstate = "off";
             red=0;
             typeCommend = 1; 
           } else if (header.indexOf("GET /blue/on") >= 0) {
             Serial.println("blue on");
             bluestate = "on";
             blue=255;
             typeCommend = 1; 
           } else if (header.indexOf("GET /blue/off") >= 0) {
             Serial.println("blue off");
             bluestate = "off";
             blue=0;
             typeCommend = 1; 
           } else if (header.indexOf("GET /fade/on") >= 0) {
             Serial.println("fade on");
             fade=true;
             christmas = false;
             typeCommend = 2; //"fade"; 
           } else if (header.indexOf("GET /fade/off") >= 0) {
             Serial.println("fade off");
             fade=false;
             christmas = false;
             red = 0;
             green = 0;
             blue = 0;
             typeCommend = 1; //"normal";
           } else if (header.indexOf("GET /2512") >= 0) {
             Serial.println("Christmas special");
             fade=false;
             christmas = true;
             typeCommend = 2512; //""; 
           } else if (header.indexOf("GET /color/") >= 0) {  
             ind1 = header.indexOf("GET /color/");
             ind2 = header.indexOf("\n", ind1 + 1);
             String readString = header.substring(ind1+1, ind2);
             
             ind1 = readString.indexOf("/");
             ind2 = readString.indexOf("/", ind1+1);
             ind3 = readString.indexOf("/", ind2+1);
             ind4 = readString.indexOf("/", ind3+1);
             ind5 = readString.indexOf("/", ind4+1);

             red = header.substring(ind2 +2, ind3+1).toInt();
             green = header.substring(ind3 +2, ind4+1).toInt();
             blue = header.substring(ind4+2, ind5+1).toInt();

             Serial.print("color: R ");
             Serial.print(red);
             Serial.print(" G ");
             Serial.print(green);
             Serial.print(" B ");
             Serial.println(blue);
             fade=false;
             christmas = false;
             typeCommend = 1; //normal
           } else if (header.indexOf("GET /pref1/") >= 0) {  // Preference1 = 'some' leds on with color red/green/blue
             ind1 = header.indexOf("GET /pref1/");
             ind2 = header.indexOf("\n", ind1 + 1);
             String readString = header.substring(ind1+1, ind2);
              
             ind1 = readString.indexOf("/");
             ind2 = readString.indexOf("/", ind1+1);
             ind3 = readString.indexOf("/", ind2+1);
             ind4 = readString.indexOf("/", ind3+1);
             ind5 = readString.indexOf("/", ind4+1);
             ind6 = readString.indexOf(" ", ind5+1);

             red = header.substring(ind2 +2, ind3+1).toInt();
             green = header.substring(ind3 +2, ind4+1).toInt();
             blue = header.substring(ind4+2, ind5+1).toInt();
                         
             Serial.print("pref1: R ");
             Serial.print(red);
             Serial.print(" G ");
             Serial.print(green);
             Serial.print(" B ");
             Serial.println(blue);
             fade=false;
             christmas = false;
             typeCommend = 3; //"pref1";
           } else if (header.indexOf("GET /select/") >= 0) {  // give leds additional leds on with color red/green/blue
             ind1 = header.indexOf("GET /select/");
             ind2 = header.indexOf("\n", ind1 + 1);
             String readString = header.substring(ind1+1, ind2);
              
             ind1 = readString.indexOf("/");
             ind2 = readString.indexOf("/", ind1+1);
             ind3 = readString.indexOf("/", ind2+1);
             ind4 = readString.indexOf("/", ind3+1);
             ind5 = readString.indexOf("/", ind4+1);
             ind6 = readString.indexOf(" ", ind5+1); // URL ends with a space in de header

             red = header.substring(ind2 +2, ind3+1).toInt();
             green = header.substring(ind3 +2, ind4+1).toInt();
             blue = header.substring(ind4+2, ind5+1).toInt();
             selectLeds = header.substring(ind5+2, ind6+1);
                         
             Serial.print("select: R ");
             Serial.print(red);
             Serial.print(" G ");
             Serial.print(green);
             Serial.print(" B ");
             Serial.print(blue);
             Serial.print(" Selected: ");
             Serial.println(selectLeds);
             
             fade=false;
             christmas = false;
             typeCommend = 4; //"select";
           }
           

//Serial.println(typeCommend);

    switch (typeCommend){

      case 1: //normal
        strip.clear();
        Serial.print("Rood: ");
        Serial.print( red );
        Serial.print(" - Groen: ");
        Serial.print( green );
        Serial.print( " - Blauw: ");
        Serial.println( blue);
    
        stripcolor = strip.Color(red,green, blue);
        strip.fill(stripcolor, 0, 150);
        strip.show();
        break;
      case 2: //fade:
        doFade();
        break;
      case 3: //pref1:
        {
        strip.clear();
        int arrayPref[] = {1,2,34,35,36,37,90,91,92,93,149,150,0}; //End with a 0
        for ( int i = 0; i<20; i++ ) {  // 20 is size of array
          if (arrayPref[i] == 0) {
            break;
          }
        strip.setPixelColor(arrayPref[i]-1, strip.Color(red,green,blue));  
        }
        strip.show();
        }
        break;
      case 4: // select
         strip.clear();
         ind1 = 0;
         while (ind1 >= 0) {
          ind2 = selectLeds.indexOf(",", ind1+1);
//          Serial.println(selectLeds.substring(ind1, ind2));
          strip.setPixelColor(selectLeds.substring(ind1, ind2).toInt(), strip.Color(red,green,blue));
          
          ind1 = ind2+1;
          if (ind2 < 0 ) { 
            ind1 = -4; 
          }
         }
       strip.show(); 
       typeCommend = 0;
       break;

     case 2512: //Special for christmass
//       int arrayRed[] = {10,40,40,10,0,0,0,0,0,0};
//       int arrayGreen[] = {0,0,0,0,0,10,40,40,10,0};
//       int arrayBlue[] = {0,0,0,0,0,0,0,0,0,0};

//       for (int i = 0; i< LED_COUNT - 10; i=i+10) {
//        for (int j = 0; j<10; j++) {
//          strip.setPixelColor(i+j, strip.Color(arrayRed[j],arrayGreen[j],arrayBlue[j]));
//        }
//       }
//       strip.show();
//       Serial.println("2512 change");
       cmDelay = 0;
       christmas = true;
       break;
    }
        


// Displayingen a dafault page

    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
    client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
    client.println(".button2 {background-color: #77878A;}</style></head>");
    client.println("<body><h1>Roggekamp LED verlichting gang</h1>");
// Display current state, and ON/OFF buttons for GPIO 5  
    client.println("<p>green - State " + greenstate + "</p>");
// If the green LED is off, it displays the ON button       
    if (greenstate == "off") {
      client.println("<p><a href=\"/green/on\"><button class=\"button\">ON</button></a></p>");
    } else {
      client.println("<p><a href=\"/green/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 
   
// Display current state, and ON/OFF buttons for GPIO 5  
    client.println("<p>red - State " + redstate + "</p>");
// If the green LED is off, it displays the ON button       
    if (redstate == "off") {
      client.println("<p><a href=\"/red/on\"><button class=\"button\">ON</button></a></p>");
    } else {
      client.println("<p><a href=\"/red/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 
// Display current state, and ON/OFF buttons for GPIO 5  
    client.println("<p>blue - State " + bluestate + "</p>");
// If the green LED is off, it displays the ON button       
    if (bluestate == "off") {
      client.println("<p><a href=\"/blue/on\"><button class=\"button\">ON</button></a></p>");
    } else {
      client.println("<p><a href=\"/blue/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 
    client.println("<p>fade</p>");
// If the fade is off, it displays the ON button       
    if (fade == false) {
      client.println("<p><a href=\"/fade/on\"><button class=\"button\">ON</button></a></p>");
    } else {
      client.println("<p><a href=\"/fade/off\"><button class=\"button button2\">OFF</button></a></p>");
    } 

    client.println("</body></html>");
// Clear the header variable
    header = "";
// Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
          }
        }
      }
    }

  }

i2cloop();

delay(200);
 

}
void doChristmas() {
     if (cmDelay < 1 or cmDelay > 10) { // delay counter 
      cmDelay = 1;   
      cmCounter++;
      if (cmCounter > 9) {
        cmCounter=0;
      }
      int arrayRed[] = {10,40,40,10,0,0,0,0,0,0,10,40,40,10,0,0,0,0,0,0};
      int arrayGreen[] = {0,0,0,0,0,10,40,40,10,0,0,0,0,0,0,10,40,40,10,0};
      int arrayBlue[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
     
     strip.clear();
     for (int i = 0; i< LED_COUNT - 10; i=i+10) {
     
      for (int j = 0; j<10; j++) {
      strip.setPixelColor(i+j, strip.Color(arrayRed[j+cmCounter],arrayGreen[j+cmCounter],arrayBlue[j+cmCounter]));
      }
     }
    
    strip.show();
    Serial.println("2512 change");
     } else {
      cmDelay++;
     }
}
void doFade() {
        if (redUP){
        red++;
        if (red > 254) {
          redUP = false;
        }
      }else {
        red--;
        if (red < 1) {
          redUP = true;
        }
      }
        if (greenUP){
        green++;
        if (green > 254) {
          greenUP = false;
          green=255;
        }
      }else {
        green--;
        if (green < 1) {
          greenUP = true;
          green = 0;
        }
      }
        if (blueUP){
        blue++;
        if (blue > 254) {
          blueUP = false;
          blue = 255;
        }
      }else {
        blue--;
        if (blue < 1) {
          blueUP = true;
          blue = 0;
        }
      }
      
    Serial.print("Rood: ");
    Serial.print( red );
    Serial.print(" - Groen: ");
    Serial.print( green );
    Serial.print( " - Blauw: ");
    Serial.println( blue);
    
    stripcolor = strip.Color(red,green, blue);
    strip.fill(stripcolor, 0, 150);
    strip.show();
}
void Neopixel_off() {
// All leds off
  
  strip.clear();
  strip.show();

}

void Neopixel_1() {
//  strip.fill(white,0);
//  strip.show();
}

static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+4; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip.show();
      delay(25);
  }
}

void i2cloop() {

  if (lightCount == 100  and luxMeter !=0 ) { // If no sesor found, luxMeter=0
    lightCount = 0;
    float lux = lightMeter.readLightLevel(true);
    if (lux > 0) {
      Serial.print("Light: ");
      Serial.print(lux);
      Serial.println(" lx");
  
      sprintf(domoURL,"http://%s/json.htm?type=command&param=udevice&idx=%i&svalue=%0.1f", domoIP, lightIDX, lux) ;
      http.begin(domoURL);
      int httpResponseCode = http.GET();  // Fire and forget: No check on good reception of the call
    
      Serial.println(domoURL); 
      Serial.println(httpResponseCode);
//     while (client.available()) {
//       char c = client.read();
//       Serial.print(c);
//     }
    }
    delay(500);
  } else {
    lightCount++;
  }

}

int BH1750_Read(int address) //
{
//  int i=0;
//  Wire.beginTransmission(address);
//  Wire.requestFrom(address, 2);
//  while(Wire.available()) //
//  {
//    buff[i] = Wire.read();  // receive one byte
//    i++;
//  }
//  Wire.endTransmission(); 
//  return i;
}


void BH1750_Init(int address)
{
//  Wire.beginTransmission(address);
//  Wire.write(0x10);//1lx reolution 120ms
//  Wire.endTransmission();
}
