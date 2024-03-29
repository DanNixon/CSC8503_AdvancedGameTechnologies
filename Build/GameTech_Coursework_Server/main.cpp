#pragma once

#include <GameplayLib\HighscoreBoard.h>
#include <enet\enet.h>
#include <iphlpapi.h>
#include <nclgl\GameTimer.h>
#include <ncltech\PubSubBrokerNetNode.h>

#pragma comment(lib, "IPHLPAPI.lib")

#define SERVER_PORT 1200

PubSubBrokerNetNode g_node; //!< Pub/sub server

void PrintAllAdapterIPAddresses();

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

  // Init server
  if (!g_node.Initialize(SERVER_PORT, 8))
  {
    fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
    onExit(EXIT_FAILURE);
  }

  // Show interfaces
  printf("Server initiated\n");
  PrintAllAdapterIPAddresses();

  // Init high score board
  HighscoreBoard highScores(&g_node);

  // Run update loop
  GameTimer timer;
  timer.GetTimedMS();
  while (true)
  {
    float dt = timer.GetTimedMS() * 0.001f;
    g_node.PumpNetwork(dt);
    Sleep(0);
  }

  onExit(0);
}

/**
 * @brief Grabs a list of all network adapters on the computer and prints out all IPv4 addresses associated with them.
 */
void PrintAllAdapterIPAddresses()
{
  // Initially allocate 5KB of memory to store all adapter info
  ULONG outBufLen = 5000;

  IP_ADAPTER_INFO *pAdapters = NULL;
  DWORD status = ERROR_BUFFER_OVERFLOW;

  // Keep attempting to fit all adapter info inside our buffer, allocating more memory if needed
  // Note: Will exit after 5 failed attempts, or not enough memory. Lets pray it never comes to this!
  for (int i = 0; i < 5 && (status == ERROR_BUFFER_OVERFLOW); i++)
  {
    pAdapters = (IP_ADAPTER_INFO *)malloc(outBufLen);
    if (pAdapters != NULL)
    {
      // Get Network Adapter Info
      status = GetAdaptersInfo(pAdapters, &outBufLen);

      // Increase memory pool if needed
      if (status == ERROR_BUFFER_OVERFLOW)
      {
        free(pAdapters);
        pAdapters = NULL;
      }
      else
      {
        break;
      }
    }
  }

  if (pAdapters != NULL)
  {
    // Iterate through all Network Adapters, and print all IPv4 addresses associated with them to the console
    // - Adapters here are stored as a linked list termenated with a NULL next-pointer
    IP_ADAPTER_INFO *cAdapter = &pAdapters[0];
    while (cAdapter != NULL)
    {
      IP_ADDR_STRING *cIpAddress = &cAdapter->IpAddressList;
      while (cIpAddress != NULL)
      {
        printf("  - %s:%u\n", cIpAddress->IpAddress.String, SERVER_PORT);
        cIpAddress = cIpAddress->Next;
      }
      cAdapter = cAdapter->Next;
    }

    free(pAdapters);
  }
}
