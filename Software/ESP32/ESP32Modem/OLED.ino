
void OLED_Init() 
{

/*

#ifdef TTGO
	pinMode(PIN_OLED_RST, OUTPUT);
	digitalWrite(PIN_OLED_RST, LOW); // low to reset OLED
	delay(50);
	digitalWrite(PIN_OLED_RST, HIGH); // must be high to turn on OLED
#endif

	// Start I2C Communication
	Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);

	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) 
	{
		C64.println(F("SSD1306 OLED allocation failed"));
		Debug.println(F("DEBUG: SSD1306 OLED allocation failed"));
	}
	else
	{
		Debug.println(F("DEBUG: SSD1306 OLED Initialized OK"));
	}

	// Clear the buffer.
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);

	*/
}

void DisplayString(const char* message)
{
/*	display.clearDisplay();
	display.setCursor(0, 10);
	display.println(message);
	display.display();
*/
}

void DisplayBoth(const char* s)
{
	C64.println(s);
	DisplayString(s);
}