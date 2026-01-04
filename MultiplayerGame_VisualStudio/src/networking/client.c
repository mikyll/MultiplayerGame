#include "client.h"

static void handleConnection(void);
static void handleMessage(ENetPacket* packet);
static void (*connectOkCallback)(void);
static void (*connectFailCallback)(void);
static void (*disconnectCallback)(void);

static ENetAddress serverAddress;
static ENetHost* clientHost = NULL;
static ENetPeer* serverPeer = NULL;
static int isWaiting = 0;
static Uint32 startWaitingTime = 0;

void setClientOnConnect(void (*onConnectOk)(void), void (*onConnectFail)(void))
{
  connectOkCallback = onConnectOk;
  connectFailCallback = onConnectFail;
}

void setClientOnDisconnect(void (*onDisconnect)(void))
{
  disconnectCallback = onDisconnect;
}

int createClient(char* ipAddress, int port)
{
  // Create client host
  clientHost = enet_host_create(
    NULL, // create a client host
    1,  // only allow 1 outgoing connection
    2,  // allow up 2 channels to be used, 0 and 1
    0,  // assume any amount of incoming bandwidth
    0   // assume any amount of outgoing bandwidth
  );

  if (clientHost == NULL)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "An error occurred while trying to create an ENet client host.");
    return -1;
  }

  // If no IP address is specified, use default IP address (localhost)
  if (ipAddress == NULL || strlen(ipAddress) == 0)
  {
    enet_address_set_host(&serverAddress, DEFAULT_HOSTNAME);
  }
  else
  {
    if (isValidIPAddress(ipAddress) != 0)
    {
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Invalid IP address");

      return -1;
      
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Attempting to connect to IP address %s:%d...\n", ipAddress, port);

    enet_address_set_host(&serverAddress, ipAddress);
  }
  serverAddress.port = port;

  // Connect to server
  serverPeer = enet_host_connect(clientHost, &serverAddress, 2, 0);
  if (serverPeer == NULL)
  {
    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not connect to server.");

    return -1;
  }

  // Connection attempt
  isWaiting = 1;
  startWaitingTime = SDL_GetTicks();

  return 0;
}

void destroyClient(void)
{
  if (clientHost == NULL)
    return;

  if (serverPeer != NULL)
    enet_peer_disconnect_now(serverPeer, 0);

  enet_host_destroy(clientHost);
  clientHost = NULL;

  if (disconnectCallback != NULL)
    disconnectCallback();

  isWaiting = 0;
  startWaitingTime = 0;
}

static void handleConnection(void)
{
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Received connect event");
}

static void handleMessage(ENetPacket* packet)
{
  NetMessage receivedNetMessage;
  int id;

  memcpy(&receivedNetMessage, packet->data, sizeof(NetMessage));

  switch (receivedNetMessage.type)
  {
  case CONNECT_OK:
    {
      ConnectOK connectOK = receivedNetMessage.data.connectOK;

      // Spawn missing players
      for (int i = 0; i < connectOK.numPlayers; i++)
      {
        if (getPlayerByID(connectOK.players[i].id) == NULL)
        {
          id = spawnPlayer(connectOK.players[i]);

          // Set local player
          if (connectOK.localID == connectOK.players[i].id)
          {
            setLocalPlayer(getPlayerByID(id));
          }
        }
      }

      if (connectOkCallback != NULL)
        connectOkCallback();

      // Test
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Successfully connected to server. Player ID: P%d", connectOK.localID);

      isWaiting = 0;
      startWaitingTime = 0;

      // Send player color

      break;
    }
  case CONNECT_DENIED:
    {
      ConnectDenied connectDenied = receivedNetMessage.data.connectDenied;

      // TODO: never reached (it times out)
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Connection to server was denied: %s", connectDenied.message);

      destroyClient();

      if (connectFailCallback != NULL)
        connectFailCallback();

      isWaiting = 0;
      startWaitingTime = 0;

      break;
    }
  case PLAYER_JOINED:
    {
      PlayerJoined playerJoined = receivedNetMessage.data.playerJoined;

      if (getPlayerByID(playerJoined.newPlayer.id) == NULL)
      {
        spawnPlayer(playerJoined.newPlayer);
      }

      // Test
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "A new player joined: P%d", playerJoined.newPlayer.id);

      break;
    }
  case PLAYER_LEFT:
    {
      PlayerLeft playerLeft = receivedNetMessage.data.playerLeft;

      if (getPlayerByID(playerLeft.oldPlayer.id) != NULL)
      {
        destroyPlayerByID(playerLeft.oldPlayer.id);
      }

      // Test
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Player P%d left", playerLeft.oldPlayer.id);

      break;
    }
  case GAME_STATE:
    {
      GameState gameState = receivedNetMessage.data.gameState;

      // Update players
      for (int i = 0; i < gameState.numPlayers; i++)
      {
        Entity* player = getPlayerByID(gameState.players[i].id);
        if (player != NULL)
        {
          player->x = gameState.players[i].x;
          player->y = gameState.players[i].y;
        }
      }

      break;
    }
  default:
    {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unhandled event type: %x", receivedNetMessage.type);

      break;
    }
  }
}

void clientBefore(void)
{
  ENetEvent event;

  if (clientHost == NULL)
    return;

  if (isWaiting == 1 && SDL_GetTicks() - startWaitingTime >= 3000)
  {
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Connection attempt failed: timeout.");

    connectFailCallback();
    destroyClient();
    return;
  }

  while (enet_host_service(clientHost, &event, 0) > 0)
  {
    switch (event.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
      {
        handleConnection();

        return;
      }
    case ENET_EVENT_TYPE_DISCONNECT:
      {
        destroyClient();

        return;
      }
    case ENET_EVENT_TYPE_RECEIVE:
      {
        handleMessage(event.packet);

        enet_packet_destroy(event.packet);

        break;
      }
    default:
      {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unhandled event type: %x", event.type);

        break;
      }
    }
  }
}

// sends player update
void clientAfter(void)
{
  Entity* player;
  PlayerState playerState;
  ENetPacket* packet;

  if (clientHost == NULL)
    return;

  player = getLocalPlayer();
  if (player == NULL)
    return;

  memset(&playerState, 0, sizeof(PlayerState));
  playerState.player = *player;

  // Send local player position
  packet = enet_packet_create(&playerState, sizeof(PlayerState), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(serverPeer, 0, packet);
  enet_host_flush(clientHost);
}
