#include "ADDR_EEPROM.h"

void SetDefaults()
{
	C64.println();

	C64.print(F("Factory Defaults, are you sure (y/n)?"));
	int option = ReadByte(C64);

	switch (option)
	{
		case 'y':
		case 'Y':
			DoSetDefaults();
			break;

		default:
			return;
	}
}

void DoSetDefaults()
{
	// Graphics Mode
	updateEEPROMByte(ADDR_PETSCII, 0);  // ASCII Mode

	// Menu Mode
	updateEEPROMByte(ADDR_HAYES_MENU, 0);  // Menu Mode

	// Baud Rate
	updateEEPROMInteger(ADDR_BAUD_LO, DEFAULT_BAUD_RATE);

	// Last Port


	// SSID
	networkName = "None";
	networkPswd = "None";

	// Phone Book
	ClearPhoneBook();

	// Hayes Settings - TODO

	// Mark as initialized
	updateEEPROMByte(ADDR_INITIALIZED, EEPROM_INITIALIZED);

	DisplayBoth("Restarting with Factory Defaults");
	C64.println();
	delay(1000);
	ESP.restart();
	while (1);
}