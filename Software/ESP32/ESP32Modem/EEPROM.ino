#include "ADDR_EEPROM.h"

// TODO, switch this all over to ESP32 Preferences

void restoreEEPROMSettings()
{
	EEPROM.begin(4096);

	int init = EEPROM.read(ADDR_INITIALIZED);

	if (init != EEPROM_INITIALIZED)
	{
		DisplayBoth("*** EEPROM Not Initialized ***");
		delay(5000);
		DoSetDefaults();  // Reboots
	}	

	mode_petscii = EEPROM.read(ADDR_PETSCII);	
	mode_Hayes = EEPROM.read(ADDR_HAYES_MENU);
	BAUD_RATE = readEEPROMInteger(ADDR_BAUD_LO);

	Debug.println(F("\nDEBUG: EPROM Settings Restored"));
}

void updateEEPROMByte(int address, byte value)
{
  if (EEPROM.read(address) != value)
  {
    EEPROM.write(address, value);
    EEPROM.commit();
  }
}

void updateEEPROMInteger(int address, uint16_t value)
{
    byte hi = value / 256;
    byte lo = value % 256;        

    updateEEPROMByte(address+0, lo);
    updateEEPROMByte(address+1, hi);
}

uint16_t readEEPROMInteger(int address)
{
    return (EEPROM.read(address + 0) +
            EEPROM.read(address + 1) * 256);
}


void updateEEPROMPhoneBook(int address, String host)
{
    for (int i = 0; i < ADDR_HOST_SIZE - 2; i++)
	{
	    EEPROM.write(address + i, host.c_str()[i]);
	}
	EEPROM.commit();
}

String readEEPROMPhoneBook(int address)
{
    char host[ADDR_HOST_SIZE - 2];

    for (int i = 0; i < ADDR_HOST_SIZE - 2; i++)
    {
		host[i] = EEPROM.read(address + i);
    }
    return host;
}

void updateEEPROMString(int address, String text)
{
	for (int i = 0; i < 38; i++)
	{
		EEPROM.write(address + i, text.c_str()[i]);
	}
	EEPROM.commit();
}

String readEEPROMString(int address)
{
	char text[STRING_SIZE - 2];

    for (int i = 0; i < STRING_SIZE - 2; i++)
	{
        text[i] = EEPROM.read(address + i);
	}
    return text;
}