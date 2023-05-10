//www.elegoo.com
//2016.12.9

/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

//include the library code for the RTC module
#include <Wire.h>
#include <DS3231.h>

//include the library for the DHT module
#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

//initialises the clock
DS3231 clock;
RTCDateTime dt;

// initialise the temp/humidity sensor
static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

//initialise the push button input to switch between the 2 functions
int buttonApin = 4;
//use a bool type for telling if we are in clcok or temp mode
bool isClock = false;
/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}

void setup() {
  //setup the button pins
  pinMode(buttonApin, INPUT_PULLUP); 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD. -removed
  //lcd.print("Hello, World!");

  //Now to setup the clock module
  clock.begin();
  
  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2016, 12, 9, 11, 46, 00);
  
  // Send sketch compiling time to Arduino
  clock.setDateTime(__DATE__, __TIME__);

  /* Diagnostic seril output */
  //Serial.begin( 9600);
}

void loop() {
  // init our temp/humid values
  float temperature;
  float humidity;
  //Check the push button input, and if detected flip between modes
  if (digitalRead(buttonApin) == LOW)
  {
    isClock = !isClock;
    lcd.clear();
  }
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);
  //Print the current data in short date format
  //TODO: only output this once?  per day?
  if(isClock)
  {
    dt = clock.getDateTime();
  
    lcd.print(dt.day); lcd.print("/");
    lcd.print(dt.month); lcd.print("/");
    lcd.print(dt.year); 
    //now shift the cursor to the next row and print the time
    lcd.setCursor(0, 1);
    lcd.print(dt.hour);   lcd.print(":");
    lcd.print(dt.minute); lcd.print(":");
    lcd.print(dt.second);

    //Delay for 1 second
    delay(1000);
  }
  else
  {
    if( measure_environment( &temperature, &humidity ) == true )
    {
      lcd.print( "T = " );
      lcd.print( temperature, 1 );
      lcd.print( " deg. C");
      lcd.setCursor(0, 1);
      lcd.print( "H = " );
      lcd.print( humidity, 1 );
      /*
      Serial.print( "T = " );
      Serial.print( temperature, 1 );
      Serial.print( " deg. C, H = " );
      Serial.print( humidity, 1 );
      Serial.println( "%" );
      */
    }
  }
  //Delay breaks the humidity sensor!
}
