#pragma once

#include <enet\enet.h>
#include <iphlpapi.h>
#include <nclgl\GameTimer.h>
#include <ncltech\PubSubBrokerNetNode.h>

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
  if (enet_initialize() != 0)
  {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }

  if (!g_node.Initialize(12345, 8))
  {
    fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
    onExit(EXIT_FAILURE);
  }

  const uint8_t ip[] = { 127, 0, 0, 1 };
  g_node.ConnectToBroker(ip, SERVER_PORT);

  printf("CLI client initiated.\n");

  g_timer.GetTimedMS();
  while (true)
  {
    float dt = g_timer.GetTimedMS() * 0.001f;
    g_node.PumpNetwork(dt);

    g_txTimer.GetTimedMS();
    if (g_txTimer.GetMS() > 5000.0f)
    {
      char * m = "world";
      g_node.NetworkTxPayload("aaaa", m, (uint16_t) strlen(m));
    }

    Sleep(0);
  }

  onExit(0);
}