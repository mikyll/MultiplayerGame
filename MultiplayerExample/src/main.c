#include "main.h"

static void initApp();
static void cleanupApp();

static void initApp()
{
	memset(&app, 0, sizeof(App));
    app.textureTail = &app.textureHead;

	srand(time(NULL));

	initSDL();
    initFonts();
    initEnet();

	atexit(cleanupApp);
}

static void cleanupApp()
{
    cleanupSDL();

    cleanupEnet();
}

int main()
{
	initApp();

    showMenu();

	while (1)
	{
		prepareScene();

        // Input
		doInput();

		// Logic
		app.delegate.logic();

		// Draw
		app.delegate.draw();

		presentScene();

		SDL_Delay(16);
	}

	return 0;
}

/*
typedef struct {
    char message[64];
    Entity players[64];
    int numPlayers;
} MyStruct;

// TEST
void doServer();
void doClient();
void addEntity(MyStruct* myStruct, float x, float y);
void showEntities();

int main()
{
    initNetworking();
    atexit(quitEnet);

    int choice = 0;
    printf("Multiplayer mode:\n1. Host\n2. Join\n\nEnter your choice: ");
    if (scanf_s("%d", &choice) != 1 || (choice != 1 && choice != 2))
        return 1;



    // Server
    if (choice == 1)
    {
        printf("\n====== Server mode ======\n\n");
        doServer();
    }

    // Client
    if (choice == 2)
    {
        printf("\n====== Client mode ======\n\n");
        doClient();
    }

    return 0;
}

void doServer2()
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 12345;

    ENetHost* server = enet_host_create(&address, 32, 2, 0, 0);
    if (server == NULL) {
        fprintf(stderr, "Failed to create ENet server host\n");
        return EXIT_FAILURE;
    }

    printf("Server started, waiting for connections...\n");

    ENetEvent event;
    while (1)
    {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("New client connected from %x:%u.\n",
                    event.peer->address.host,
                    event.peer->address.port);
                break;

            case ENET_EVENT_TYPE_RECEIVE: {
                MyStruct receivedStruct;
                memcpy(&receivedStruct, event.packet->data, sizeof(MyStruct));
                printf("Received message: %s\n\n",
                    receivedStruct.message);
                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Client disconnected.\n");
                break;

            default:
                break;
            }
        }
    }


    enet_host_destroy(server);
}

void doClient2()
{
    ENetHost* client = enet_host_create(NULL, 1, 2, 0, 0);
    if (client == NULL) {
        fprintf(stderr, "Failed to create ENet client host\n");
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetPeer* peer;
    ENetEvent event;

    enet_address_set_host(&address, "localhost");
    address.port = 12345;

    // Connect to server
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL) {
        fprintf(stderr, "No available peers for initiating an ENet connection\n");
        return EXIT_FAILURE;
    }

    // Wait for connection response
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to %s:%d succeeded.\n", "localhost", 12345);

        while (1)
        {
            MyStruct myStruct;
            memset(&myStruct, 0, sizeof(MyStruct));

            printf("Enter message: ");
            fgets(myStruct.message, 64, stdin);
            myStruct.message[strcspn(myStruct.message, "\n")] = '\0'; // Remove trailing newline character

            if (strlen(myStruct.message) == 0)
                continue;

            ENetPacket* packet = enet_packet_create(&myStruct, sizeof(MyStruct), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            enet_host_flush(client);
        }
    }
    else {
        enet_peer_reset(peer);
        fprintf(stderr, "Connection to %s:%d failed.\n", "localhost", 12345);
        return EXIT_FAILURE;
    }

    enet_peer_disconnect(peer, 0);
    while (enet_host_service(client, &event, 3000) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_RECEIVE:
            printf("%s\n", (char*)event.packet->data);
            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("Disconnection succeeded.\n");
            return EXIT_SUCCESS;

        default:
            break;
        }
    }

    enet_host_destroy(client);
}

// TESTONEEEE
void addEntity(MyStruct* myStruct, float x, float y)
{
    // Allocate memory for the player entity
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    if (entity == NULL)
    {
        printf("Entity creation failed.\n");
        return;
    }

    // Init player parameters
    memset(entity, 0, sizeof(Entity));
    entity->x = x;
    entity->y = y;
    entity->next = NULL;

    // Add player
    myStruct->players[myStruct->numPlayers] = *entity;

}
void showEntities(MyStruct* myStruct)
{
    Entity* player;
    int i = 1;
    for (player = myStruct->playersHead.next; player != NULL; player = player->next)
    {
        if (player != NULL)
        {
            printf("Player%d: %2.f, %2.f\n", i, player->x, player->y);
        }
    }
    printf("Players: %d\n", i-1);
}
int countEntities(MyStruct myStruct)
{
    Entity* player;
    int i = 0;
    for (player = myStruct.playersHead.next; player != NULL; player = player->next)
    {
        i++;
    }
    return i;
}
void doServer()
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 12345;

    ENetHost* server = enet_host_create(&address, 32, 2, 0, 0);
    if (server == NULL) {
        fprintf(stderr, "Failed to create ENet server host\n");
        return EXIT_FAILURE;
    }

    printf("Server started, waiting for connections...\n");

    ENetEvent event;
    while (1)
    {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("New client connected from %x:%u.\n",
                    event.peer->address.host,
                    event.peer->address.port);
                break;

            case ENET_EVENT_TYPE_RECEIVE: {
                MyStruct receivedStruct;
                memcpy(&receivedStruct, event.packet->data, sizeof(MyStruct));
                printf("Received message: %s\n\n",
                    receivedStruct.message);

                showEntities(&receivedStruct);

                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Client disconnected.\n");
                break;

            default:
                break;
            }
        }
    }


    enet_host_destroy(server);
}

void doClient()
{
    ENetHost* client = enet_host_create(NULL, 1, 2, 0, 0);
    if (client == NULL) {
        fprintf(stderr, "Failed to create ENet client host\n");
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetPeer* peer;
    ENetEvent event;

    enet_address_set_host(&address, "localhost");
    address.port = 12345;

    // Connect to server
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL) {
        fprintf(stderr, "No available peers for initiating an ENet connection\n");
        return EXIT_FAILURE;
    }

    // Wait for connection response
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to %s:%d succeeded.\n", "localhost", 12345);

        while (1)
        {
            MyStruct myStruct;
            memset(&myStruct, 0, sizeof(MyStruct));

            Entity* e;
            while (myStruct.playersHead.next)
            {
                e = myStruct.playersHead.next;
                myStruct.playersHead.next = e->next;
                free(e);
            }
            myStruct.playersTail = &myStruct.playersHead;


            addEntity(&myStruct, 10.0f, 10.0f);
            addEntity(&myStruct, 15.0f, 15.0f);
            addEntity(&myStruct, 20.0f, 30.0f);
            printf("Num entities: %d\n\n", countEntities(myStruct));

            printf("Enter message: ");
            fgets(myStruct.message, 64, stdin);
            myStruct.message[strcspn(myStruct.message, "\n")] = '\0'; // Remove trailing newline character

            if (strlen(myStruct.message) == 0)
                continue;

            ENetPacket* packet = enet_packet_create(&myStruct, sizeof(MyStruct), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(peer, 0, packet);
            enet_host_flush(client);
        }
    }
    else {
        enet_peer_reset(peer);
        fprintf(stderr, "Connection to %s:%d failed.\n", "localhost", 12345);
        return EXIT_FAILURE;
    }

    enet_peer_disconnect(peer, 0);
    while (enet_host_service(client, &event, 3000) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_RECEIVE:
            printf("%s\n", (char*)event.packet->data);
            enet_packet_destroy(event.packet);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("Disconnection succeeded.\n");
            return EXIT_SUCCESS;

        default:
            break;
        }
    }

    enet_host_destroy(client);
}
*/