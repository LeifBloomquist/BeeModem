// ----------------------------------------------------------
// Simple Incoming connection handling

void Incoming()
{
	C64.println();

	if (!wifi_connected)
	{
		C64.println(F("*** Not connected to Wi-Fi ***\n"));
		return;
	}

    unsigned int WiFiLocalPort = readEEPROMInteger(ADDR_PORT_LO);

    C64.print(F("Incoming port ("));
    C64.print(WiFiLocalPort);
    C64.print(F("): "));

    String strport = GetInput();

    if (strport.length() > 0)
    {
        WiFiLocalPort = strport.toInt();
		if (WiFiLocalPort <= 0) return;
        updateEEPROMInteger(ADDR_PORT_LO, WiFiLocalPort);
    }
	
	if (WiFiLocalPort <= 0) return;

    // Start the server 
    WiFiServer wifi_server(WiFiLocalPort);    
    wifi_server.begin();
    wifi_server.setNoDelay(true);

	ShowListener(WiFiLocalPort);

    while (true)
    {
        // 1. Check for new connections
        if (wifi_server.hasClient())
        {
            // This code has to be here for disconnections via +++ to work.  If moved a separate function, .stop() doesn't work.  Scope issue?
            WiFiClient FirstClient = wifi_server.available();
            C64.print(F("Incoming connection from "));
            C64.println(FirstClient.remoteIP());
            FirstClient.println(F("CONNECTING..."));

            //CheckTelnet(client);
            TerminalMode(FirstClient, wifi_server);

            FirstClient.stop();

            DisplayBoth("Incoming connection closed.");
			ShowListener(WiFiLocalPort);
        }

        // 2. Check for cancel
        if (C64.available() > 0)  // Key hit
        {
            C64.read();  // Eat Character
            C64.println(F("Cancelled"));
            wifi_server.close();
            wifi_server.stop();
            return;
        }
    }
}

// Reject additional incoming connections
bool RejectIncoming(WiFiClient client)
{
    //no free/disconnected spot so reject
    client.write("\n\rSorry, server is busy\n\r\n\r");
    client.stop();
}
void ShowListener(int localPort)
{
	C64.print(F("\r\nWaiting for connection on "));
	C64.print(WiFi.localIP());
	C64.print(" port ");
	C64.println(localPort);
}