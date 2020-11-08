***HouseLed***

The idea of this project is:

- Control a led-strip (based on Adafruit-Neopixel) by Domoticz, 
- Send light-intensity information to domoticz,
- Send a trigger on a moving detection sensor to domoticz

The reason for this project was  the re-make of our waitingroom behind our front-door at home. I wanted to make a nice light using a led-strip.



**Hardware used:**

This project is build on a **Lolin - Node-MCU** esp8266. 

Ledstrip, based on WS2812 (3.3v) connected to D5, Good powersuply for the strip.

The light intensity meter is a BH1750 light sensor, connecten via i2c.

The moving detection is entering a high to low contact to D2



**Control from and to domoticz:**

The sonsors a triggering an URL of domoticz, including and IDX number (defined in Domoticz and enterd in config.h)

To controll the ledsstrip by Domoticz, the housled has a webserver. You can enter the url of the houseled in youre browser to do some basic testing. 

Domoticz is using the following commands:

[ip]/off   				   Switching off the leds

[ip]/red/on   			Switch all leds to red

[ip]/red/off			   Switch off all red

[ip]/green/on   		Switch all leds to green

[ip]/green/off		   Switch off all green

[ip]/blue/on   		   Switch all leds to blue

[ip]/blue/off			  Switch off all blue

[ip]/fade/on			  Fading some colors (off to switch off) 

[ip]/color/'*red*'/'*green*'/'*blue*'/  	Set all leds to al color geven in red, green and blue (all 0-255 attetion to last / )

[ip]/pref1/'*red*'/'*green*'/'*blue*'/ 	Set som in esp defined leds in a special color

[ip]/select/'*red*'/'*green*'/'*blue*'/'leds'   Give some leds, seperated by a ','  a color. No '/' at the end this time

[ip]/2512/ 				 A hidden function for Christmas.



**Example of config in Domoticz**

![Schermafbeelding 2020-11-08 om 21.01.11](/Users/erikjan/Desktop/Schermafbeelding 2020-11-08 om 21.01.11.png)