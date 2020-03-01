void PhoneBook()
{
  while (true)
  {
    char address[ADDR_HOST_SIZE];
    char numString[2];

    DisplayPhoneBook();

    C64.print(F("\r\nSelect: #, m to modify, c to clear all\r\na to set auto-start, 0 to go back: "));

    char addressChar = ReadByte(C64);
    C64.println((char)addressChar); // Echo 

    if (addressChar == 'm' || addressChar == 'M')
    {
      C64.print(F("\r\nEntry # to modify? (0 to abort): "));

      char addressChar = ReadByte(C64);

      numString[0] = addressChar;
      numString[1] = '\0';
      int phoneBookNumber = atoi(numString);

      if (phoneBookNumber >= 0 && phoneBookNumber <= ADDR_HOST_ENTRIES)
      {
        C64.print(phoneBookNumber);
        switch (phoneBookNumber) {
        case 0:
          break;

        default:
          C64.print(F("\r\nEnter address: "));
          String hostName = GetInput();
          if (hostName.length() > 0)
          {
            updateEEPROMPhoneBook(ADDR_HOSTS + ((phoneBookNumber - 1) * ADDR_HOST_SIZE), hostName);
          }
          else
            updateEEPROMPhoneBook(ADDR_HOSTS + ((phoneBookNumber - 1) * ADDR_HOST_SIZE), "");

        }
      }
    }
    else if (addressChar == 'c' || addressChar == 'C')
    {
      C64.print(F("\r\nAre you sure (y/n)? "));

      char addressChar = ReadByte(C64);

      if (addressChar == 'y' || addressChar == 'Y')
      {
        for (int i = 0; i < ADDR_HOST_ENTRIES; i++)
        {
          updateEEPROMPhoneBook(ADDR_HOSTS + (i * ADDR_HOST_SIZE), "\0");
        }
      }
    }
    else if (addressChar == 'a' || addressChar == 'A')
    {
      C64.print(F("\r\nEntry # to set to auto-start?\r\n""(0 to disable): "));

      char addressChar = ReadByte(C64);

      numString[0] = addressChar;
      numString[1] = '\0';
      int phoneBookNumber = atoi(numString);
      if (phoneBookNumber >= 0 && phoneBookNumber <= ADDR_HOST_ENTRIES)
      {
        C64.print(phoneBookNumber);
        updateEEPROMByte(ADDR_HOST_AUTO, phoneBookNumber);
      }

    }
    else
    {
      numString[0] = addressChar;
      numString[1] = '\0';
      int phoneBookNumber = atoi(numString);

      if (phoneBookNumber >= 0 && phoneBookNumber <= ADDR_HOST_ENTRIES)
      {
        switch (phoneBookNumber) 
        {
          case 0:
            return;

          default:
            strncpy(address, readEEPROMPhoneBook(ADDR_HOSTS + ((phoneBookNumber - 1) * ADDR_HOST_SIZE)).c_str(), ADDR_HOST_SIZE);
            removeSpaces(address);
            Dialout(address);
            break;
          }

      }
    }
  }
}

void DisplayPhoneBook() 
{
  C64.println();
  C64.println(F("Phone Book"));
  C64.println();

  for (int i = 0; i < ADDR_HOST_ENTRIES; i++)
  {
    C64.print(i + 1);
    C64.print(F(":"));
    C64.println(readEEPROMPhoneBook(ADDR_HOSTS + (i * ADDR_HOST_SIZE)));

	yield();  // For 300 baud
  }
  C64.println();
  C64.print(F("Autostart: "));
  C64.print(EEPROM.read(ADDR_HOST_AUTO));
  C64.println();
}

void removeSpaces(char *temp)
{
  char *p1 = temp;
  char *p2 = temp;

  while (*p2 != 0)
  {
    *p1 = *p2++;
    if (*p1 != ' ')
      p1++;
  }
  *p1 = 0;
}

// Connect to an address in the form host:port 
void Dialout(char* host)
{
  char* index;
  uint16_t port = TELNET_DEFAULT_PORT;
  String hostname = String(host);

  if (strlen(host) == 0)
  {
    if (mode_Hayes)
    {
      Modem_PrintERROR();
    }
    return;
  }

  if ((index = strstr(host, ":")) != NULL)
  {
    index[0] = '\0';
    hostname = String(host);
    port = atoi(index + 1);
  }

  lastHost = hostname;
  lastPort = port;

  Connect(hostname, port);
}