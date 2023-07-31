#pragma once

#include <nds.h>
#include <dswifi9.h>
#include <stdio.h>
#include <gl2d.h>

#include "nf_lib.h"

#define EMPTY -1

// Nifi settings
#define MAX_CLIENT 6         // Max number of clients
#define WIFI_RATE 240        // Read/Write rate per second
#define WIFI_TIMEOUT 10      // Number of wifi rate before the message is considered lost
#define MAX_TIMEOUT_RETRY 10 // Number of try before client is considered as disconnected
#define CLIENT_TIMEOUT 60    // Number of frame before local client is considered as disconnected

// Network packet settings
#define REQUEST_TYPE_INDEX 0
#define REQUEST_NAME_INDEX 1
#define MAX_REQUEST_LENGTH 256
#define MAX_REQUEST_PARAM_LENGTH 64
#define MAX_REQUEST_PARAM_COUNT 10

enum RequestType
{
    TICK_GAME = 0
};

enum MultiplayerStatus
{
    MP_HOST_SEARCHING = 0,
    MP_CLIENT_SEARCHING = 1,
    MP_HOST_READY = 2,
    MP_CLIENT_READY = 3,
    MP_CONNECTION_LOST = 4
};

typedef struct
{
    int id;                // Id of the client
    char macAddress[13];   // Mac address of the client
    char sendBuffer[1024]; // Wifi write buffer of the client
    int lastMessageId;     // Last message id received by the client

    int playerTargetX;
    int playerTargetZ;
    int playerTileX;
    int playerTileZ;
    int playerFacing;
    int timeLeft;
    int currentBatchStep;
    int batchesComplete;
} Client;

// ** All functions **
void treatData();
void sendPacketByte(u8 command, u8 data);
void createRoom();
void scanForRoom();
void addClient(int id, bool addHost);
void SendDataTo(Client *client);
void AddDataTo(Client *client, const char *data);
void removeClient(Client *client);
void resetClientValues(Client *client);
void managerServer();
void communicateWithNextClient();
Client *getClientById(int consoleId);
void shareRequest(Client *clientSender, enum RequestType requestType);
void createRequest(Client *clientSender, Client *clientToUpdate, enum RequestType requestType);
void resetNifiValues();