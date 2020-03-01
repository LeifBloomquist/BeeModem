
void TerminalMode(WiFiClient client, WiFiServer &server)
{
	bool isFirstChar = true;
	bool isTelnet = false;
	bool telnetBinaryMode = false;

	while (client.connected())
	{
		// 1. Get data from the telnet client and push it to the serial port
		if (client.available() > 0)
		{
			char data = client.read();

			// If first character back from remote side is NVT_IAC, we have a telnet connection.
			if (isFirstChar)
			{
				if (data == NVT_IAC)
				{
					isTelnet = true;
					CheckTelnet(isFirstChar, telnetBinaryMode, client);
				}
				else
				{
					isTelnet = false;
				}
				isFirstChar = false;
			}
			else  // Connection already established, but may be more telnet control characters
			{
				if ((data == NVT_IAC) && isTelnet)
				{
					if (CheckTelnet(isFirstChar, telnetBinaryMode, client))
					{
						C64.write(NVT_IAC);
					}
				}
				else   //  Finally regular data - just pass the data along.
				{
					C64.write(data);
					delayMicroseconds(200);  // 200 seems to be the limit to keep WYSE term from losing characters  TODO make this configurable
				}
			}
		}

		// 2. Check serial port for data and push it to the telnet client
		if (C64.available())
		{
			char data = C64.read();
			client.write((char)data);
			delay(1);

			// 3a. Check Escape (+++).  Just exit immediately if found (which disconnects)
			if (CheckEscape(data))
			{
				DisplayBoth("Escape!");
				return;
			}
		}

		// 3. Check for new incoming connections - reject
		if (server.hasClient())
		{
			WiFiClient newClient = server.available();
			RejectIncoming(newClient);
		}

	} // while (client.connected())

	DisplayBoth("Disconnected");
}