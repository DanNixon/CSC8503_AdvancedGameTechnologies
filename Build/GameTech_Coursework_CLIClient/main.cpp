#pragma once

#include <enet\enet.h>
#include <iphlpapi.h>
#include <nclgl\GameTimer.h>
#include <ncltech\PubSubBrokerNetNode.h>
#include <ncltech\FunctionalPubSubClient.h>

#pragma comment(lib, "IPHLPAPI.lib")

#define SERVER_PORT 1234

PubSubBrokerNetNode g_node;
GameTimer g_timer;

GameTimer g_txTimer; //TODO

int onExit(int exitCode)
{
  g_node.Release();
  system("pause");
  exit(exitCode);
}

int main(int argc, char **argv)
{
  // Init ENet
  if (enet_initialize() != 0)
  {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }

  // Init client
  if (!g_node.Initialize(12345, 8))
  {
    fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
    onExit(EXIT_FAILURE);
  }

  // Connect to server
  const uint8_t ip[] = { 127, 0, 0, 1 };
  g_node.ConnectToBroker(ip, SERVER_PORT);

  printf("CLI client initiated.\n");

  // Add broadcast message handler
  FunctionalPubSubClient broadcastMessageHandler(g_node);
  broadcastMessageHandler.SetSubscriptionHandler([&](const std::string &, const char * msg, uint16_t) {
    printf("Broadcast message: %s\n", msg);
    return true;
  });
  g_node.Subscribe(&broadcastMessageHandler, "broadcast_messages");

  // Run update loop
  g_timer.GetTimedMS();
  while (true)
  {
    float dt = g_timer.GetTimedMS() * 0.001f;
    g_node.PumpNetwork(dt);

    if (g_txTimer.GetTimedMS(false) > 5000.0f)
    {
      char * m = "world";
      g_node.BroadcastMessage(&broadcastMessageHandler, "broadcast_messages", m, (uint16_t) strlen(m));
      g_txTimer.GetTimedMS();
    }

    Sleep(0);
  }

  onExit(0);
}