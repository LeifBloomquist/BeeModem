/*
C64 Wifi Terminal - WiFiModem ESP32
Copyright 2015-2020 Leif Bloomquist and Alex Burger

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.
*/

/* ESP32 port by Leif Bloomquist */
/* ESP8266 port by Alex Burger */
/* Written with assistance and code from Greg Alekel and Payton Byrd */

#include <WiFi.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <EEPROM.h>
#include "Telnet.h"
#include "ADDR_EEPROM.h"

#define VERSION "ESP32 0.15"

// Board type
//#define WEMOS_LOLIN
#define TTGO  

// For Wemos Lolin32 ESP32 with built-in SSD1306 OLED
#ifdef WEMOS_LOLIN
#define PIN_OLED_SDA 5 
#define PIN_OLED_SCL 4
#endif

//For ESP32 OLED V2.0 TTGO & for Arduino
#ifdef TTGO
#define PIN_OLED_SDA 4
#define PIN_OLED_SCL 15
#define PIN_OLED_RST 16 
#define PIN_C64_RX   23   // Receive from C64
#define PIN_C64_TX   19   // Transmit to C64
#endif

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define Debug Serial
//#define C64 Serial1
HardwareSerial C64(2);  // Serial1 for ESP32 

#define DEFAULT_BAUD_RATE 2400
unsigned int BAUD_RATE = DEFAULT_BAUD_RATE;

// WiFi network name and password:
const char * networkName = "L0ndonGreen2";
const char * networkPswd = "establ1shment";

//Are we currently connected to Wi-Fi?
boolean wifi_connected = false;

String lastHost = "";
int lastPort = TELNET_DEFAULT_PORT;

int mode_Hayes = 0;    // 0 = Menu, 1 = Hayes
boolean mode_petscii = 0;    // 0 = Menu, 1 = Hayes   Always use ASCII for Hayes.  To set SSID, user must use ASCII mode.

void setup() 
{
	// 1. Debug Serial
	Debug.begin(115200);
	Debug.println(F("\nDEBUG: Wi-Fi Modem Initializing"));

	// 2. OLED
	OLED_Init();

	// 3. EEPROM
	restoreEEPROMSettings();

	// 4. C64 Serial
	C64.begin(BAUD_RATE, SERIAL_8N1, PIN_C64_RX, PIN_C64_TX);
	C64.println(F("\nWi-Fi Modem Initializing"));
	
	// 5. Wi-Fi
	ConnectToWiFi(networkName, networkPswd);

	int timeout = 0;
	while (!wifi_connected)  // TODO, timeout
	{
		delay(1);
		if (timeout++ > 5000)
		{
			C64.println(F("\n\n*** Failed to connect to Wi-Fi! ***\n"));
			break;
		}
	}
	delay(100);

	ShowInfo();
	ShowConfiguration();

	Debug.println(F("DEBUG: Initialization Complete"));
}

void loop() 
{
	if (mode_Hayes < 0 || mode_Hayes > 1)
	{
		mode_Hayes = 0;
	}

	// DEBUG !!!! Always start in Menu mode for testing.
	mode_Hayes = 0;

	if (mode_Hayes)
	{
		HayesEmulationMode();
	}
	else
	{
		ShowMenu();
	}
}

void ShowMenu()
{
	C64.print(F("\r\nCommodore Wi-Fi Modem\r\n\r\n"
		"1. Telnet to Host\r\n"
		"2. Phone Book\r\n"
		"3. Wait for Incoming Connection\r\n"
	    "4. Configuration\r\n"
		"5. Hayes Emulation Mode \r\n"
		"\r\n"
		"Select: "));

	DisplayString("READY.");

	int option = ReadByte(C64);
	C64.println((char)option);   // Echo back

	switch (option)
	{
		case '1':
			DoTelnet();
			break;

		case '2':
			PhoneBook();
			break;

		case '3':
			Incoming();
			break;

		case '4':
			Configuration();
			break;

		case '5':
			EnterHayesMode();
			break;

		case '\n':
		case '\r':
		case ' ':
			break;

		default:
			C64.println(F("Unknown option, try again"));
			break;
	}
}

void Configuration()
{
	while (true)
	{
		char temp[30];
		C64.print(F("\r\n"
			"Configuration Menu\r\n"
			"\r\n"
			"1. Display Current Configuration\r\n"
			"2. Change SSID\r\n"
			"3. Change Baud Rate\r\n"
			"4. Set Factory Defaults\r\n"
			"5. Return to Main Menu\r\n"
			"\r\nSelect: "));

		int option = ReadByte(C64);
		C64.println((char)option);  // Echo back

		switch (option)
		{
			case '1':
				ShowConfiguration();				
				break;

			case '2':
			    ChangeSSID();
				break;

			case '3':
				ChangeBaudRate();
				break;

			case '4':
				SetDefaults();
				break;

			case '5': 
				return;

			case '\n':
			case '\r':
			case ' ':
				continue;

			default: C64.println(F("Unknown option, try again"));
				continue;
		}
	}
}

// ----------------------------------------------------------
// Show Configuration

void ShowConfiguration()
{
	C64.println();

	C64.print(F("IP Address:  "));
	C64.println(WiFi.localIP());

	C64.print(F("IP Subnet:   "));
	C64.println(WiFi.subnetMask());

	C64.print(F("IP Gateway:  "));
	C64.println(WiFi.gatewayIP());

	C64.print(F("Wi-Fi SSID:  "));
	C64.println(WiFi.SSID());

	C64.print(F("MAC Address: "));
	C64.println(WiFi.macAddress());

	C64.print(F("DNS IP:      "));
	C64.println(WiFi.dnsIP());

	C64.print(F("Hostname:    "));
	C64.println(WiFi.getHostname());

	C64.print(F("Firmware:    "));
	C64.println(VERSION);

	C64.print(F("Baud Rate:   "));
	C64.println(BAUD_RATE);
}

void ShowInfo()
{
	char temp[40];
	sprintf(temp,("Firmware: %s"), VERSION);
	DisplayString(temp);
	delay(1000);

	sprintf(temp,("Baud Rate: %u"), BAUD_RATE);
	DisplayString(temp);
	delay(1000);

	sprintf(temp,("IP Address:\r\n\r\n%s"), WiFi.localIP().toString().c_str());
	DisplayString(temp);
	delay(1000);

	sprintf(temp,("SSID:\r\n\r\n%s"), WiFi.SSID().c_str() );
	DisplayString(temp);
	delay(1000);
}

void ChangeSSID()
{
	C64.println();
	String input;

	C64.print(F("New SSID: "));
	input = GetInput();

	if (input.length() == 0) return;

	updateEEPROMString(ADDR_WIFI_SSID, input);

	C64.println();

	C64.print(F("Passphrase: "));
	input = GetInput();
	updateEEPROMString(ADDR_WIFI_PASS, input);

	C64.println();
	DisplayBoth("SSID changed.  Rebooting...");
	delay(1000);
	ESP.restart();
	while (1);
}

void ConnectToWiFi(const char * ssid, const char * pwd) 
{	
	char temp[100];
	sprintf(temp, "Connecting to WiFi... %s", ssid);
	DisplayBoth(temp);

	// delete old config
	WiFi.disconnect(true);
	//register event handler
	WiFi.onEvent(WiFiEvent);

	//Initiate connection
	WiFi.begin(ssid, pwd);
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event) 
{
	switch (event) 
	{
		case SYSTEM_EVENT_STA_GOT_IP:
			//When connected set 
			wifi_connected = true;
			C64.println(F("\n*** Connected to Wi-Fi ***\n"));
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			wifi_connected = false;
			C64.println(F("\n*** Disconnected from Wi-Fi ***\n"));
			break;
		default: break;
	}
}