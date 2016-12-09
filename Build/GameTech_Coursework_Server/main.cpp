#pragma once

#include <enet\enet.h>
#include <iphlpapi.h>
#include <nclgl\GameTimer.h>
#include <nclgl\Vector3.h>
#include <nclgl\common.h>
#include <ncltech\NetworkBase.h>

#pragma comment(lib, "IPHLPAPI.lib")

#define SERVER_PORT 1234

NetworkBase server;
GameTimer timer;
GameTimer txTimer;

void Win32_PrintAllAdapterIPAddresses();

int onExit(int exitcode)
{
  server.Release();
  system("pause");
  exit(exitcode);
}

int main(int arcg, char **argv)
{
  if (enet_initialize() != 0)
  {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }

  if (!server.Initialize(SERVER_PORT, 4))
  {
    fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
    onExit(EXIT_FAILURE);
  }

  printf("Server Initiated\n");

  Win32_PrintAllAdapterIPAddresses();

  timer.GetTimedMS();
  while (true)
  {
    float dt = timer.GetTimedMS() * 0.001f;

    server.ServiceNetwork(dt, [&](const ENetEvent &evnt) {
      switch (evnt.type)
      {
      case ENET_EVENT_TYPE_CONNECT:
        printf("- New Client Connected\n");
        break;

      case ENET_EVENT_TYPE_RECEIVE:
        printf("\t Client %d says: %s\n", evnt.peer->incomingPeerID, evnt.packet->data);
        enet_packet_destroy(evnt.packet);
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        printf("- Client %d has disconnected.\n", evnt.peer->incomingPeerID);
        break;
      }
    });

    if (txTimer.GetTimedMS() > 1000.0f)
    {
      // ENetPacket *position_update = enet_packet_create(&pos, sizeof(Vector3), 0);
      // enet_host_broadcast(server.m_pNetwork, 0, position_update);
    }

    Sleep(0);
  }

  onExit(0);
}

// Yay Win32 code >.>
//  - Grabs a list of all network adapters on the computer and prints out all
//  IPv4 addresses associated with them.
void Win32_PrintAllAdapterIPAddresses()
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
        printf("\t - Listening for connections on %s:%u\n", cIpAddress->IpAddress.String, SERVER_PORT);
        cIpAddress = cIpAddress->Next;
      }
      cAdapter = cAdapter->Next;
    }

    free(pAdapters);
  }
}