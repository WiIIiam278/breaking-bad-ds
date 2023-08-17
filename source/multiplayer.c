#include "multiplayer.h"

// ** Wifi variables **:
int wifiChannel = 10;

// ** Wifi buffers **:
// Read part
char WIFI_Buffer[1024] = "";			// Wifi buffer is the incoming data from the wifi (DO NOT USE LIKE THIS, please use the "wirelessData" define)
#define wirelessData (WIFI_Buffer + 32) // Define to use the WIFI_Buffer array to avoid wrong values
char Values[1024] = "";					// Store all the values received from the wifi and wainting for treatment
char TempValues[1024] = "";				// Store values that can't be treated yet
int WIFI_ReceivedDataLength = 0;		// Size of data received from the wifi

// Write part
char tempSendBuffer[1024] = ""; // Temp buffer to use when the host need to resend the same data if the wifi is not working correctly

// ** Room variables **:
Client clients[MAX_CLIENT];
Client *localClient = &clients[0];
int hostIndex = EMPTY; // Index of the host in the clients array
bool isHost = false;   // Is the Nintendo DS hosting a room?

// For host
int speakTo = 1;		   // Index of the speak to speak with
int idCount = 0;		   // Id of the next client to add
int lastCommunication = 0; // Countdown to check the wifi timeout
char tempMacAddress[13];   // Mac address of the client that is trying to join the room
int timeoutCount = 0;	   // Store how many times the wifi has failed to communicate with the client

// For non-host
int joinRoomTimer = WIFI_TIMEOUT; // Timer to limit the time to join a room
bool tryJoinRoom = false;		  // Is the client trying to join a room
bool skipData;					  // If true, the client will skip data of the current wifi packet

// Other variables
const bool MP_MODE = false;
int mpStatus = MP_CLIENT_SEARCHING;

void joinMultiplayer(bool hostRoom)
{
	nifiInit();
	if (hostRoom)
	{
		createRoom();
	}
	else
	{
		mpStatus = MP_CLIENT_SEARCHING;
		joinRoomTimer = WIFI_TIMEOUT;
		tryJoinRoom = true;
		resetNifiValues();
	}
}

void disableMultiplayer()
{
	isHost = false;
	joinRoomTimer = WIFI_TIMEOUT;
	for (int i = 1; i < MAX_CLIENT; i++)
	{
		removeClient(&clients[i]);
	}
	resetNifiValues();
	Wifi_DisableWifi();
	mpStatus = MP_CONNECTION_LOST;
}

void tickMultiplayer()
{
	if (tryJoinRoom)
	{
		joinRoomTimer--;
		if (joinRoomTimer == 0) // Resend the request each time the timer is ended
		{
			scanForRoom();
			// Reset the timer
			joinRoomTimer = WIFI_TIMEOUT;
		}
		return;
	}

	// Check if the local client has lost the connection with the host
	if (localClient->id != EMPTY && !isHost)
	{
		lastCommunication++;
		if (lastCommunication == CLIENT_TIMEOUT)
		{
			for (int i = 0; i < MAX_CLIENT; i++)
			{
				removeClient(&clients[i]);
			}
			mpStatus = MP_CONNECTION_LOST;
		}
	}

	shareRequest(localClient, TICK_GAME);
}

/**
 * @brief Wifi handler call when data is received
 *
 * @param packetID
 * @param readlength
 */
void WirelessHandler(int packetID, int readlength)
{
	Wifi_RxRawReadPacket(packetID, readlength, (unsigned short *)WIFI_Buffer);

	// Get the correct data length
	WIFI_ReceivedDataLength = readlength - 32;

	// Treatment of the data
	treatData();
}

/**
 * @brief Send data to clients
 *
 * @param buffer
 * @param length
 */
void SendWirelessData(unsigned short *buffer, int length)
{
	if (Wifi_RawTxFrame(length, 0x0014, buffer) != 0)
	{
		printf("Error calling RawTxFrame\n");
	}
}

/**
 * @brief Reset variable used by the the Wifi to avoid wrong values when reconnecting
 *
 */
void resetNifiValues()
{
	// Reset clients data
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		resetClientValues(&clients[i]);
	}
	// Reset buffers
	WIFI_Buffer[0] = '\0';
	Values[0] = '\0';
	TempValues[0] = '\0';
	tempSendBuffer[0] = '\0';
	// Reset variables
	lastCommunication = 0;
	hostIndex = EMPTY;
	WIFI_ReceivedDataLength = 0;
}

/**
 * @brief Prepare the Nifi system
 */
void nifiPrepare()
{
	// Changes how packets are handled
	Wifi_SetRawPacketMode(PACKET_MODE_NIFI);

	// Init Wifi without automatic settings
	Wifi_InitDefault(false);
}

/**
 * @brief Init the Nifi system
 *
 */
void nifiInit()
{
	resetNifiValues();

	// Enable Wifi
	Wifi_EnableWifi();

	// Configure custom packet handler for when
	Wifi_RawSetPacketHandler(WirelessHandler);

	// Force specific channel for communication
	Wifi_SetChannel(wifiChannel);

	// Get MAC address of the Nintendo DS
	u8 macAddressUnsigned[6];
	Wifi_GetData(WIFIGETDATA_MACADDRESS, 6, macAddressUnsigned);
	// Convert unsigned values to hexa values
	sprintf(localClient->macAddress, "%02X%02X%02X%02X%02X%02X", macAddressUnsigned[0], macAddressUnsigned[1], macAddressUnsigned[2], macAddressUnsigned[3], macAddressUnsigned[4], macAddressUnsigned[5]);
}

/*
 * Treat data from the wifi module
 */
void treatData()
{
	// Get data lenght
	int recvd_len = strlen(wirelessData);

	// printf("%s\n", wirelessData);

	// Check if the packet is valid
	if (WIFI_ReceivedDataLength == recvd_len + 1)
	{
		// Add the wifi buffer to the data buffer
		strcat(Values, wirelessData);

		int StartPosition, EndPosition;
		// Get data of the packet
		while ((StartPosition = strstr(Values, "{") - Values + 1) > 0 && (EndPosition = strstr(Values + StartPosition, "}") - Values) > 0)
		{
			char currentPacket[MAX_REQUEST_LENGTH] = "";
			strncpy(currentPacket, Values + StartPosition, EndPosition - StartPosition);

			// Start spliting incoming data
			char *ptr = strtok(currentPacket, ";");
			int SplitCount = 0;
			char params[MAX_REQUEST_PARAM_COUNT][MAX_REQUEST_PARAM_LENGTH] = {0};

			while (ptr != NULL)
			{
				strcpy(params[SplitCount], ptr);
				SplitCount++;
				ptr = strtok(NULL, ";");

				if (MP_MODE)
				{
					NF_WriteText(1, 0, 1, 4 + SplitCount, ptr);
				}
			}

			if (strcmp(params[REQUEST_TYPE_INDEX], "GAME") == 0 && !skipData) // Check if the request is about game management (refuse the request if the request was already treated)
			{
				if (strcmp(params[REQUEST_NAME_INDEX], "TICK_GAME") == 0) // A client is sending a tick
				{
					// printf("%s\n", wirelessData);
					int clientId, destinatorId;
					// Get the client destinator id
					sscanf(params[3], "%d", &destinatorId);

					// If the local client is in a party and the destinator id is the same as the local client id
					if (destinatorId == localClient->id && localClient->id != EMPTY)
					{
						// Get the if of the client that sent the request
						sscanf(params[2], "%d", &clientId);

						Client *client = getClientById(clientId);
						if (client != NULL)
						{
							// Get the value
							sscanf(params[4], "%d", &client->playerTargetX);
							sscanf(params[5], "%d", &client->playerTargetZ);
							sscanf(params[6], "%d", &client->playerTileX);
							sscanf(params[7], "%d", &client->playerTileZ);
							sscanf(params[8], "%d", &client->playerFacing);
							sscanf(params[9], "%d", &client->timeLeft);
							sscanf(params[10], "%d", &client->currentBatchStep);
							sscanf(params[11], "%d", &client->batchesComplete);

							shareRequest(client, TICK_GAME);
						}
					}
				}
			}
			else if (strcmp(params[REQUEST_TYPE_INDEX], "ROOM") == 0) // Check if the request is about room management
			{
				if (isHost) // These request are for the host client
				{
					if (strcmp(params[REQUEST_NAME_INDEX], "SCAN") == 0) // A client is searching for a room
					{
						printf("%s SEARCH FOR ROOM\n", params[2]);

						// Get the mac address of the client
						sprintf(tempMacAddress, params[2]);
						addClient(EMPTY, false);
						mpStatus = MP_HOST_READY;
					}
					else if (strcmp(params[REQUEST_NAME_INDEX], "CONFIRM_LISTEN") == 0) // If the client has received host's data
					{
						// Get the id of the client that has received the data
						int clientId;
						sscanf(params[2], "%d", &clientId);

						// If the id of the client is the same as the id of current treated client
						if (clientId == clients[speakTo].id)
						{
							communicateWithNextClient();
						}
					}
				}
				else // These request are for non-host clients
				{
					if (strcmp(params[REQUEST_NAME_INDEX], "CONFIRM_JOIN") == 0) // The host said that the local client can join the room
					{
						// Check if the mac address of the local client is the same as the one of the client that wants to join the room
						if (strcmp(params[2], localClient->macAddress) == 0 && localClient->id == EMPTY)
						{
							// Get the host id
							int hostId;
							sscanf(params[3], "%d", &hostId);
							// Set local player id
							sscanf(params[4], "%d", &localClient->id);
							addClient(hostId, true);
							mpStatus = MP_CLIENT_READY;
							printf("JOINED %d'S ROOM, YOUR ID: %d\n", hostId, localClient->id);
							tryJoinRoom = false;
						}
					}
					else if (strcmp(params[REQUEST_NAME_INDEX], "WANTSPEAK") == 0) // The host asked for communication
					{
						int clientId;
						sscanf(params[2], "%d", &clientId);
						// If the host wants to communicate with the local client

						if (localClient->id == clientId)
						{
							lastCommunication = 0;
							int messageId;
							sscanf(params[3], "%d", &messageId);

							// If the request wasn't read yet
							if (localClient->lastMessageId < messageId)
							{
								// Clear temp buffer
								strcpy(tempSendBuffer, "");
								skipData = false;
								localClient->lastMessageId = messageId;
							}
							else // If the request was already read
							{
								// Skip the request data
								skipData = true;
							}

							SendDataTo(&clients[hostIndex]);
						}
					}
					else if (strcmp(params[REQUEST_NAME_INDEX], "ADDCLIENTS") == 0) // Add multiples non local players
					{
						int destinatorId;
						sscanf(params[2], "%d", &destinatorId);
						if (destinatorId == localClient->id)
						{
							for (int i = 3; i < SplitCount; i++)
							{
								int FoundId = EMPTY;
								sscanf(params[i], "%d", &FoundId);
								addClient(FoundId, false);
								mpStatus = MP_CLIENT_READY;
							}
						}
					}
				}
				//////// Next conditions are for the host and non-host clients

				if (strcmp(params[REQUEST_NAME_INDEX], "QUIT") == 0 && !skipData) // A client quit the party
				{
					int clientId;
					sscanf(params[2], "%d", &clientId);
					int destinatorId;
					sscanf(params[3], "%d", &destinatorId);

					if (localClient->id == destinatorId)
					{
						Client *client = getClientById(clientId);
						if (client != NULL)
						{
							removeClient(client);
							printf("%d HAS LEFT THE ROOM\n", clientId);
							mpStatus = MP_CONNECTION_LOST;
						}
					}
				}
			}

			// Clear "TempValues"
			for (int i = 0; i < sizeof(TempValues); i++)
				TempValues[i] = '\0';

			// Add all characters after current data packet to "TempValues"
			strcat(TempValues, Values + EndPosition + 1);
			// Copy "TempValues" to "Values"
			strcpy(Values, TempValues);
		}
	}
}

/**
 * @brief Share a request to clients
 *
 * @param clientSender
 * @param requestType
 */
void shareRequest(Client *clientSender, enum RequestType requestType)
{
	// If the local client is the host
	if (isHost)
	{
		// Send the data to all clients
		for (int clientIndex = 1; clientIndex < MAX_CLIENT; clientIndex++)
		{
			Client *clientToUpdate = &clients[clientIndex];
			// Avoid sending the request to the current treated client
			if (clientSender->id != clientToUpdate->id && clientToUpdate->id != EMPTY)
			{
				createRequest(clientSender, clientToUpdate, requestType);
			}
		}
	}
	else if (clientSender == localClient) // Share the request to the host only
	{
		Client *clientToUpdate = &clients[hostIndex];
		createRequest(clientSender, clientToUpdate, requestType);
	}
}

/**
 * @brief Create a request and add it to the client to update's send buffer
 *
 * @param clientSender Client pointer to get data from
 * @param clientToUpdate Client pointer to send data to
 * @param requestType Request type (See RequestType enum in main.h)
 */
void createRequest(Client *clientSender, Client *clientToUpdate, enum RequestType requestType)
{
	// (Increase the buffer size if needed)
	char buffer[255] = "";
	switch (requestType)
	{
	case TICK_GAME:
		sprintf(buffer, "{GAME;TICK_GAME;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d}", clientSender->id, clientToUpdate->id,
				clientSender->playerTargetX, clientSender->playerTargetZ,
				clientSender->playerTileX, clientSender->playerTileZ,
				clientSender->playerFacing, clientSender->timeLeft,
				clientSender->currentBatchStep, clientSender->batchesComplete);
		break;
	}

	if (MP_MODE)
	{
		NF_WriteText(1, 0, 1, 2, buffer);
	}

	AddDataTo(clientToUpdate, buffer);
}

/**
 * @brief Get the Client By Id (doens't return the local player)
 *
 * @param clientId
 * @return Client*
 */
Client *getClientById(int clientId)
{
	for (int i = 1; i < MAX_CLIENT; i++)
	{
		if (clients[i].id == clientId)
		{
			return &clients[i];
		}
	}
	return NULL;
}

/**
 * @brief Create a room
 *
 */
void createRoom()
{
	// Is the client is not already a host
	if (!isHost)
	{
		resetNifiValues();

		// Create a room
		isHost = true;
		idCount = 0;
		localClient->id = idCount;
		mpStatus = MP_HOST_SEARCHING;
		timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(WIFI_RATE), managerServer);
	}
}

/**
 * @brief Remove a client
 *
 * @param client Client pointer to remove
 */
void removeClient(Client *client)
{
	if (client->id != EMPTY)
	{
		if (isHost)
		{
			for (int i = 1; i < MAX_CLIENT; i++)
			{
				// Send a request to all clients to remove the client from all other clients (the removed client is not called)
				if (&clients[i] != client && clients[i].id != EMPTY)
				{
					char buffer[18];
					sprintf(buffer, "{ROOM;QUIT;%d;%d}", client->id, clients[i].id);
					AddDataTo(&clients[i], buffer);
				}
			}
		}
	}
	resetClientValues(client);
}

/**
 * @brief Reset client values
 *
 * @param client Client pointer to reset
 */
void resetClientValues(Client *client)
{
	client->id = EMPTY;
	if (client != localClient)
	{
		strcpy(client->macAddress, "");
		client->playerTargetX = client->playerTileX = isHost ? 3 : 4;
	}
	else
	{
		skipData = false;
		client->playerTargetX = client->playerTileX = !isHost ? 3 : 4;
	}
	strcpy(client->sendBuffer, "");
	client->lastMessageId = 0;
	client->playerTargetZ = client->playerTileZ = 1;
	client->batchesComplete = client->currentBatchStep = 0;
	client->playerFacing = 1;
	client->timeLeft = EMPTY;
}

/**
 * @brief Add a client
 *
 * @param id Client's id (not used by the room owner, set to -1/EMPTY)
 * @param addHost Are we adding the host?
 */
void addClient(int id, bool addHost)
{
	bool macAlreadyExists = false;
	bool idAlreadyExists = false;
	if (isHost) // If the local client is the host
	{
		// Check if the client to add is already in the room (same mac address), because the client is maybe trying to join the room multiple times if the wifi is not working properly
		for (int i = 1; i < MAX_CLIENT; i++)
		{
			if (strcmp(tempMacAddress, clients[i].macAddress) == 0)
			{
				macAlreadyExists = true;
				break;
			}
		}
	}
	else
	{
		// Check if the client to add is already in the room (same id)
		for (int i = 1; i < MAX_CLIENT; i++)
		{
			if (clients[i].id == id)
			{
				idAlreadyExists = true;
				break;
			}
		}
	}

	// If the client to add is not already in the room
	if (!macAlreadyExists && !idAlreadyExists)
	{
		int addedClientIndex = EMPTY;
		// Try to find a free client slot
		for (int i = 1; i < MAX_CLIENT; i++)
		{
			if (clients[i].id == EMPTY)
			{
				if (isHost)
				{
					// Set client id
					idCount++;
					clients[i].id = idCount;
					// Set client mac address
					sprintf(clients[i].macAddress, tempMacAddress);
					printf("ADDED AT %d, ID : %d\n", i, idCount);
				}
				else
				{
					printf("CLIENT ADDED : %d\n", id);
					// Apply id
					clients[i].id = id;
				}

				// Store the host index
				if (addHost)
				{
					hostIndex = i;
				}
				addedClientIndex = i;
				break;
			}
		}

		// If the client is added by the host
		if (addedClientIndex != EMPTY && isHost)
		{
			// Send the client his id
			char newClientBuffer[100];
			sprintf(newClientBuffer, "{ROOM;CONFIRM_JOIN;%s;%d;%d}", tempMacAddress, localClient->id, clients[addedClientIndex].id);
			// Send the client all the other clients ids
			sprintf(newClientBuffer + strlen(newClientBuffer), "{ROOM;ADDCLIENTS;%d", clients[addedClientIndex].id);
			// Send the client id to all the other clients
			for (int i = 1; i < MAX_CLIENT; i++)
			{
				if (clients[i].id != EMPTY && i != addedClientIndex)
				{
					// Send the client all the other clients ids
					sprintf(newClientBuffer + strlen(newClientBuffer), ";%d", clients[i].id);
					// Send the client id to all the other clients
					char bufferForOtherClients[24];
					sprintf(bufferForOtherClients, "{ROOM;ADDCLIENTS;%d;%d}", clients[i].id, clients[addedClientIndex].id);
					AddDataTo(&clients[i], bufferForOtherClients);
				}
			}
			sprintf(newClientBuffer + strlen(newClientBuffer), "}");
			AddDataTo(&clients[addedClientIndex], newClientBuffer);
		}
	}
}

/**
 * @brief Scan for a room
 *
 */
void scanForRoom()
{
	isHost = false;

	char buffer[25];
	sprintf(buffer, "{ROOM;SCAN;%s}", localClient->macAddress);
	SendWirelessData((unsigned short *)buffer, strlen(buffer) + 1);
}

/**
 * @brief Select the next client to communicate with
 *
 */
void communicateWithNextClient()
{
	// Reset values
	timeoutCount = 0;
	lastCommunication = 0;

	// Change client to communicate with
	speakTo++;
	if (speakTo == MAX_CLIENT) // Go back to the beginning of the list
		speakTo = 1;

	if (clients[speakTo].id != EMPTY)
	{
		// AddDataTo(&clients[speakTo], "{}"); // TO REMOVE FIX : Data can't be sent if the buffer is empty
		SendDataTo(&clients[speakTo]);
	}
	else
	{
		lastCommunication = WIFI_TIMEOUT - 1;
	}
}

/**
 * @brief Manage client's communication order and timeout
 *
 */
void managerServer()
{
	// Only the host can manage the server
	if (isHost)
	{
		// Increase the time to check timeout
		lastCommunication++;
		if (lastCommunication == WIFI_TIMEOUT)
		{
			// If the clients is in the party (id not empty)
			if (clients[speakTo].id != EMPTY)
			{
				// If the timeout max count isn't reached, retry communication
				if (timeoutCount <= MAX_TIMEOUT_RETRY)
				{
					timeoutCount++;
					lastCommunication = 0;

					SendDataTo(&clients[speakTo]);
				}
				else
				{
					// Remove the client
					removeClient(&clients[speakTo]);
					communicateWithNextClient();
				}
			}
			else
			{
				communicateWithNextClient();
			}
		}
	}
}

/**
 * @brief Add data in the client's send buffer to send it to the client
 *
 * @param client client to add data to
 * @param data data to add
 */
void AddDataTo(Client *client, const char *data)
{
	if (client != NULL && client->id != EMPTY && strlen(data) != 0)
	{
		sprintf(client->sendBuffer + strlen(client->sendBuffer), "%s", data);
	}
}

/**
 * @brief Send data to a client with Nifi
 *
 * @param client client to send data to
 */
void SendDataTo(Client *client)
{
	// If the buffer is not empty, copy the buffer to a new one and clear the buffer
	if (timeoutCount == 0)
	{
		if (isHost)
		{
			client->lastMessageId++;

			// Ask to client to communicate
			sprintf(tempSendBuffer, "{ROOM;WANTSPEAK;%d;%d}", client->id, client->lastMessageId);
		}
		else
		{
			// Tell the host that the local client has received the message
			sprintf(tempSendBuffer, "{ROOM;CONFIRM_LISTEN;%d}", localClient->id);
		}

		sprintf(tempSendBuffer + strlen(tempSendBuffer), "%s", client->sendBuffer);

		// Clear the client's send buffer
		strcpy(client->sendBuffer, "");
	}

	// Send data
	SendWirelessData((unsigned short *)tempSendBuffer, strlen(tempSendBuffer) + 1);
}

int getMultiplayerStatus()
{
	return mpStatus;
}

Client *getOpponent()
{
	for (int i = 1; i < MAX_CLIENT; i++)
	{
		if (clients[i].id != EMPTY)
		{
			return &clients[i];
		}
	}
	mpStatus = MP_CONNECTION_LOST;
	return NULL;
}

Client *getLocalClient()
{
	return localClient;
}

bool isHostClient()
{
	return isHost;
}