/*
 Name:		SlapHat.ino
 Created:	5/5/2021 10:00:58 AM
 Author:	admin
*/

#pragma region OLED libraries
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <splash.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GrayOLED.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#pragma endregion

#include <Servo.h>

// SCL connects to A5
// SDA connects to A4

#pragma region OLED Setup
#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);


#define XPOS 0

#define YPOS 1

#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16

#define LOGO16_GLCD_WIDTH 16

static const unsigned char PROGMEM logo16_glcd_bmp[] = { B00000000, B11000000, B00000001, B11000000, B00000001, B11000000, B00000011, B11100000, B11110011, B11100000, B11111110, B11111000, B01111110, B11111111, B00110011, B10011111, B00011111, B11111100, B00001101, B01110000, B00011011, B10100000, B00111111, B11100000, B00111111, B11110000, B01111100, B11110000, B01110000, B01110000, B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)

#error("Height incorrect, please fix Adafruit_SSD1306.h!");

#endif

#pragma endregion

#pragma region Servo Setup
Servo myservo; //this is the servo oject
int pos = 0;    // variable to store the servo position
#pragma endregion

#define Switch 1					// Attach switch to pin 1
#define Hour_Button 2				// Attach Hour Button to pin 2
#define Minute_Button 3				// Attach Minute Button to pin 3
#define Buzzer 4					// Attach buzer to pin 4
#define Servo_Pin 5					// Attach the servo to pin 5

long Current_Time = 0;
long Current_Time_millis = 0;

#pragma region protoypes
void OLED_Setup();
long SetTime();
void DisplayTime(long Current_Time, long Alarm_Time = 0);
String TimeString(long Time, bool Current = true);
#pragma endregion

void setup() {
	OLED_Setup();
	
	pinMode(Switch, INPUT);
	
	pinMode(Hour_Button, INPUT_PULLUP);

	pinMode(Minute_Button, INPUT_PULLUP);

	pinMode(Buzzer, OUTPUT);

	myservo.attach(Servo_Pin);  // attaches the servo on pin 9 to the servo object
	myservo.write(0);
	
	display.setCursor(0, 0);
	display.clearDisplay();
	display.setTextSize(1);
	display.println("Set Current Time");
	display.display();
	delay(3000);

	Current_Time = SetTime();
	Current_Time_millis = millis();
	
	display.setCursor(0, 0);
	display.clearDisplay();
	display.setTextSize(1);
	display.println("Current Time Set");
	display.display();
	delay(100);

}


void loop() {
	Current_Time++;
	DisplayTime(Current_Time);
	delay(1000);
}
 
/// <summary>
///  Begins the serial output and sets up the display.  Only call during setup()
/// </summary>
void OLED_Setup() {
	Serial.begin(9600);

	display.begin(SSD1306_SWITCHCAPVCC, 0x3C); ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

	display.display();

	display.setTextSize(2);

	display.setTextColor(WHITE);

	display.setCursor(0, 0);

	display.clearDisplay();
	display.display();
}

/// <summary>
/// Uses the Minute and Hours buttons to set time.  When both buttons are pressed, the time is set.
/// </summary>
/// <returns>Time in military format</returns>
long SetTime() {

	long Time = 0;
	long Hours = 0;
	long Minutes = 0;

	while (true) {
		Serial.write("Time Loop\n");
		
		// Only change time once per loop
		int Minute_Button_State = digitalRead(Minute_Button);
		int Hour_Button_State = digitalRead(Hour_Button);
		
		// Add time for forward
		if (Minute_Button_State == LOW)
		{
			Serial.write("Minute LOW\n");
			Minutes++;
			if (Minutes >= 60) { Minutes = 0; }
		}

		// Subtract time for backwards
		if (Hour_Button_State == LOW)
		{
			Serial.write("Hour LOW\n");
			Hours++;
			if (Hours >= 24) { Hours = 0; }
		}

		// Pressing both buttons sets the time
		if (Minute_Button_State == LOW && Hour_Button_State == LOW)
		{
			Serial.write("Both LOW\n");
			break;
		}
		
	


		Time = Hours * 10000 + Minutes*100;
		Serial.println("Time = "+String(Time));
		DisplayTime(Time);

	}
	return Time;
}

/// <summary>
/// Displays the time in the proper format on the OLED
/// </summary>
/// <param name="Time">Military time int, form HHMMSS</param>
void DisplayTime(long Current_Time, long Alarm_Time = 0) {
	display.setCursor(0, 0);
	display.clearDisplay();
	
	display.setTextSize(1);
	display.println("Current Time:");

	display.setTextSize(2);
	display.println(TimeString(Current_Time));

	display.setTextSize(1);
	display.println("Alarm Time:");

	display.setTextSize(2);
	display.println(TimeString(Alarm_Time,false));
	display.display();
} 

/// <summary>
/// 
/// </summary>
/// <param name="Time">Military time int, form HHMMSS</param>
/// <returns>Properly formatted string for display</returns>
String TimeString(long Time,bool Current = true) {
	long Hours = floor(Time / 10000);
	long Minutes = floor((Time - Hours * 10000) / 100);
	long Seconds = Time - Hours * 10000 - Minutes * 100;

	if (Seconds == 60) {
		Seconds = 0;
		Minutes++;
	}

	if (Minutes == 60) {
		Minutes = 0;
		Hours++;
	}

	if (Hours == 24) {
		Hours = 0;
	}

	if (Current) { Current_Time = Hours * 10000 + Minutes * 100 + Seconds; }

	String Hours_Str = String(Hours);
	String Minutes_Str = String(Minutes);
	String Seconds_Str = String(Seconds);

	// Add a zero if not 2 chars
	if (Hours_Str.length() < 2) {
		Hours_Str = "0" + Hours_Str;
	}

	// Add a zero if not 2 chars
	if (Minutes_Str.length() < 2) {
		Minutes_Str = "0" + Minutes_Str;
	}

	// Add a zero if not 2 chars
	if (Seconds_Str.length() < 2) {
		Seconds_Str = "0" + Seconds_Str;
	}


	String time_display = Hours_Str + ":" + Minutes_Str + ":" + Seconds_Str;

	return time_display;
}