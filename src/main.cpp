#include "main.h"
#include <FastLED.h>
#include <RTClib.h>

// WS2812 LED stuff
#define PIN 6
#define NUM_LEDS 144

// Real time clock stuff
RTC_DS3231 rtc;
// The pin that is connected to SQW
#define CLOCK_INTERRUPT_PIN 2

// Test mode pin - connect to GND to activate test mode
#define TEST_MODE_PIN 4

CRGB leds[NUM_LEDS];

void setup()
{
  // Init internal LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Init test mode pin with internal pullup
  pinMode(TEST_MODE_PIN, INPUT_PULLUP);

  // Init the RTC
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC!");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (rtc.lostPower())
  {
    // This will adjust to the date and time at compilation
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // We don't need the 32K Pin, so disable it
  rtc.disable32K();

  // Init the LED strip
  FastLED.addLeds<WS2812, PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  // Init serial monitor
  Serial.begin(115200);
  Serial.println("------------------------");
}

void loop()
{
  // Toggle internal LED to see the loop is running...
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  // Check if test mode is active (pin connected to GND)
  bool testMode = (digitalRead(TEST_MODE_PIN) == LOW);

  // Print current time
  char date[10] = "hh:mm:ss";
  auto now = rtc.now();
  now.toString(date);
  Serial.println(date);
  
  if (testMode)
  {
    Serial.println("TEST MODE ACTIVE");
  }
  
  Serial.println("------------------------");
  delay(500); // To get each second

  // Get the current hour
  int currentHour = now.hour();

  // Check if we are between 8:00 and 18:00 OR in test mode
  if (testMode || (currentHour >= 8 && currentHour < 18))
  {
    // Inside time frame or test mode active
    
    // LEDs 0-49: red
    fill_solid(&leds[0], 50, CRGB::Red);
    
    // LEDs 50-99: blue
    fill_solid(&leds[50], 50, CRGB::Blue);
    
    // LEDs 100-143: alternating red and blue
    for (int i = 100; i < NUM_LEDS; i++)
    {
      if ((i - 100) % 2 == 0)
      {
        leds[i] = CRGB::Red;
      }
      else
      {
        leds[i] = CRGB::Blue;
      }
    }
    
    FastLED.show(); 
    Serial.println("Light up the LEDs...");
  }
  else
  {
    // Outside time frame
    FastLED.clear(); 
    FastLED.show();
    Serial.println("Turn off the LEDs...");
  }
}
