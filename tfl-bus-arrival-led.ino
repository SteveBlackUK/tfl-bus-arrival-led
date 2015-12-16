// test
//adapted from @LukeUSMC's great post here: http://community.particle.io/t/execute-function-once-a-day-or-after-reset-reboot/14521
#include "application.h"
//< tried this to see if it would fix the error, it didn't
//This application polls the TFL (Transport for London) Bus Arrivals app for a given Bus Stop
//and lights up a set of LEDs to give a quick visual indication how how much time you have
//full post here:
#define HOOK_RESP   "hook-response/tfl274_hook_3"
#define HOOK_PUB    "tfl274_hook_3"

int led_red = D1;
int led_default = D7;
bool busGood;
int badBusCall;
int updatebusminute;

//This will call the function gotBusArrivalData when the particle.io cloud posts a hook response that matches my HOOK_RESP in my definitions.

//Now lets trigger (Spark.publish) our webhook in our code so the particle.io cloud will
//reach out to the API we designated and post a response. In my instance the call looks like this:
// Spark.publish(HOOK_PUB);

//This actually isn't sufficient though. I have learned that you need to give the cloud some time to respond or
//depending on how you are calling this your loop will post your Spark.publish over and over. Here is my getWeather
//function (which is a slightly modified version of @peekay123 getWeather function in his RGB Pong Clock project Peekay123 RGBPong Clock Github5).

//Updates Bus Arrival Data
void getBusArrival() {
  busGood = false;
  Particle.publish(HOOK_PUB);

  unsigned long wait = millis();
  while (!busGood && (millis() < wait + 30000UL))
    Particle.process();
  if (!busGood) {
    badBusCall++;
    if (badBusCall > 2) {
    }
  }
  else
    badBusCall = 0;
}


// Now we've built a webhook, subscribed to that event and have called for particle.io to give us the response
//which will in turn call our gotBusArrival function. The gotBusArrival function will parse the response from a
//bunch of numbers with ~ between them to the data I actually want. Here is my gotBusArrival function which uses
//the ~ to tokenize the data and maps it to int variables for use elsewhere.
void gotBusArrival(const char *name, const char *data) {
    String str = String(data);
    char strBuffer[125] = "";
    str.toCharArray(strBuffer, 125); // example: "\"21~99~75~0~22~98~77~20~23~97~74~10~24~94~72~10~\""
    //TODO deal with consecutive ~ chars - what happens?
    //TODO deal with up to 10 busses arriving - array perhaps?  Below is ugly.
    int arrivalsec1 = atoi(strtok(strBuffer, "\"~"));
    int arrivalroute1 = atoi(strtok(NULL, "~"));
    //then values for the second bus
    int arrivalsec2 = atoi(strtok(NULL, "~"));
    int arrivalroute2 = atoi(strtok(NULL, "~"));
    //then values for the third bus
    int arrivalsec3 = atoi(strtok(NULL, "~"));
    int arrivalroute3 = atoi(strtok(NULL, "~"));
    //then values for the fourth bus
    int arrivalsec4 = atoi(strtok(NULL, "~"));
    int arrivalroute4 = atoi(strtok(NULL, "~"));
    //then values for the third bus
    int arrivalsec5 = atoi(strtok(NULL, "~"));
    int arrivalroute5 = atoi(strtok(NULL, "~"));

    //Set the LED to blink slowly if bus arriving in less than 5min
    //otherwise off
    if ((arrivalsec1/60 <2) or (arrivalsec2/60 <2) /* or (arrivalsec3/60<2)  or (arrivalsec4/60<2)  or (arrivalsec5/60<2) */ )
    {
        // To blink the LED, first we'll turn it on...
        digitalWrite(led_default, HIGH);
        digitalWrite(led_red, HIGH);
        delay(500);
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
        delay(500);
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
        delay(500);
        digitalWrite(led_red, LOW);
        delay(500);
        digitalWrite(led_red, HIGH);
    }
    //Set the second LED to blink more quickly if less than 2 min
    else if ((arrivalsec1/60 <5 && arrivalroute1 == 274 ) or (arrivalsec2/60 <5 && arrivalroute2 == 274 ) or (arrivalsec3/60<5 && arrivalroute3 == 274 )  or (arrivalsec4/60<5 && arrivalroute4 == 274 )  or (arrivalsec5/60<5 && arrivalroute5 == 274 )  )
    {
        digitalWrite(led_red, LOW);
        digitalWrite(led_default, HIGH);
        delay(500);
        digitalWrite(led_default, LOW);
        delay(500);
        digitalWrite(led_default, HIGH);
        delay(500);
        digitalWrite(led_default, LOW);
        delay(500);
        digitalWrite(led_default, HIGH);
        delay(500);
        digitalWrite(led_default, LOW);
        delay(500);
        digitalWrite(led_default, HIGH);
    }
    else {
         digitalWrite(led_red, LOW);
         digitalWrite(led_default, LOW);
    }
    bool busGood = true;
    updatebusminute = Time.minute();
}

void setup() {
    Particle.subscribe(HOOK_RESP, gotBusArrival, MY_DEVICES);
    pinMode(led_red, OUTPUT);
    pinMode(led_default, OUTPUT);
}

void loop() {
if (Time.minute() != updatebusminute){
  getBusArrival();
}
}
