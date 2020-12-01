#define FASTLED_INTERNAL
#include <FastLED.h>

#define BRIGHTNESS  225
#define LED_PIN     5
#define NUM_LEDS    41
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define RIGHT_LED_LENGTH    9
#define TOP_LED_LENGTH      23
#define LEFT_LED_LENGTH     9

#define RIGHT_ZONE_LENGTH    3
#define TOP_ZONE_LENGTH      5
#define LEFT_ZONE_LENGTH     3

#define NUMBER_OF_ZONES     11

const byte numChars = NUMBER_OF_ZONES * 3;
byte receivedChars[numChars];   // an array to store the received data
boolean newData = false;

CRGB leds[NUM_LEDS];
int incomingByte; // for incoming serial data

struct Color {
  byte  R;
  byte  G;
  byte  B;
};

#define NUMBER_OF_AREAS   3

struct Area {
  int leds;
  int zones;
};

struct Area areas[NUMBER_OF_AREAS] = {
  { 9, 3 },
  { 23, 5 },
  { 9, 3 }
};

Color zoneColors[NUMBER_OF_ZONES];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.show();
  
  Serial.begin(115200);
}

void loop() {
  if (Serial) {
    recvWithEndMarker();
    showNewData();
  } else {
    showIdleColor();
  }

//  int i;
//
//  for (i = 0; i < NUM_LEDS; i++)
//  {
//    leds[i].setRGB(255, 255, 255);
//  }
//  
//  FastLED.show();
}

void showIdleColor()
{
    int i;

    for (i = 0; i < NUM_LEDS; i++)
    {
      leds[i].setRGB(0, 255, 255);
    }
    
    FastLED.show();
}

void recvWithEndMarker() {
    static byte ndx = 0;
    byte endMarker = 0xFF;
    int rc;
   
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
        }
        else {
            ndx = 0;
            newData = true;
        }
    }
}

int getCurrentZone(int ledIndex, int areaIndex, Area *areas)
{
  Area area = areas[areaIndex];
  
  float ledPerZone = area.leds / area.zones;

  int currentZone = ledIndex/ledPerZone;

  if (areaIndex == 0)
  {
    return currentZone;
  }

  for (int i = 0; i < areaIndex; i++)
  {
    currentZone += areas[i].zones;
  }

  return currentZone;
}

void showNewData() {
    if (newData != true) {
      return;
    }

    int currentZone = 0;

    for(int i = 0; i < numChars; i++)
    {
      if ((i + 1) % 3 == 1)
      {
        zoneColors[currentZone].R = receivedChars[i];
      }
      else if ((i + 1) % 3 == 2)
      {
        zoneColors[currentZone].G = receivedChars[i];
      }
      else if ((i + 1) % 3 == 0)
      {
        zoneColors[currentZone].B = receivedChars[i];
      }
      

      if ((i + 1) % 3 == 0)
      {
        currentZone++;
      }
    }

    // Serial.print(currentZone);

    /*
    for(int i = 0; i < NUMBER_OF_ZONES; i++)
    {
      Color c = zoneColors[i];

      int base = i * 3;

      // Serial.println(c.R);
      
      leds[base].setRGB(c.R, c.G, c.B);
      leds[base + 1].setRGB(c.R, c.G, c.B);
      leds[base + 2].setRGB(c.R, c.G, c.B);
    }
    */

    int ledIndex = 0;

    for(int i = 0; i < NUMBER_OF_AREAS; i++)
    {
      Area area = areas[i];

      for(int j = 0; j < area.leds; j++)
      {
        int currentZone = getCurrentZone(j, i, areas);
  
        Color c = zoneColors[currentZone];
        
        leds[ledIndex].setRGB(c.R, c.G, c.B);

        /*
        Serial.print(i);
        Serial.print(": ");
        Serial.print(currentZone);
        Serial.print(": ");
        Serial.println(ledIndex);
        */

        ledIndex++;
      }
    }

    /*

    for(int i = 0; i < RIGHT_LED_LENGTH; i++)
    {
      int ledPerZone = RIGHT_LED_LENGTH / RIGHT_ZONE_LENGTH;

      int currentZone = i/3;

      Color c = zoneColors[currentZone];

      int base = i * ledPerZone;

      Serial.println(base);
      
      leds[i].setRGB(c.R, c.G, c.B);
      // leds[base + 1].setRGB(c.R, c.G, c.B);
      // leds[base + 2].setRGB(c.R, c.G, c.B);
    }
    */
    
    // Serial.print("This just in...");


    /*
    int rgb[4];

    int ledCount = 0;

    for(int i = 1; i <= numChars; i++)
    {
      int rgbNum = i % 3;
      
      if (rgbNum == 0)
      {
        leds[ledCount].setRGB(rgb[1], rgb[2], receivedChars[i - 1]);
        ledCount++;
      }
      else
      {
        rgb[rgbNum] = receivedChars[i - 1];
      }
    }

    leds[0].setRGB(receivedChars[0], receivedChars[1], receivedChars[2]);
    */

    /*
    for(int i = 0; i < numChars; i++)
    {
      Serial.println(receivedChars[i]);
    }
    Serial.println();
    */
    newData = false;

    FastLED.show();
}
