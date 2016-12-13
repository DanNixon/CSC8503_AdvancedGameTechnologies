#include "PubSubBrokerNetNode.h"

PubSubBrokerNetNode::PubSubBrokerNetNode()
{
}

PubSubBrokerNetNode::~PubSubBrokerNetNode()
{
  // Disconnect all active connections
  for (auto it = m_connections.begin(); it != m_connections.end(); ++it)
    enet_peer_disconnect_now(*it, 0);
  m_connections.clear();

  // Close network
  Release();
}

bool PubSubBrokerNetNode::ConnectToBroker(const uint8_t ip[4], uint16_t port)
{
  ENetPeer * peer = ConnectPeer(ip[0], ip[1], ip[2], ip[3], port);
  bool result = (peer != nullptr);

  if (result)
    m_connections.push_back(peer);

  return result;
}

void PubSubBrokerNetNode::PumpNetwork(float dt)
{
  ServiceNetwork(dt, [this](const ENetEvent &evnt) {
    switch (evnt.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
      printf("- New client (%d) connected.\n", evnt.peer->incomingPeerID);
      this->m_connections.push_back(evnt.peer);
      break;

    case ENET_EVENT_TYPE_RECEIVE:
      this->HandleRxEvent(evnt);
      break;

    case ENET_EVENT_TYPE_DISCONNECT:
      auto it = std::find(this->m_connections.begin(), this->m_connections.end(), evnt.peer);
      if (it != this->m_connections.end())
      {
        printf("- Client %d (known) has disconnected.\n", evnt.peer->incomingPeerID);
        enet_peer_disconnect_now(*it, 0);
        this->m_connections.erase(it);
      }
      else
      {
        printf("- Client %d (unknown) has disconnected.\n", evnt.peer->incomingPeerID);
      }

      break;
    }
  });
}

void PubSubBrokerNetNode::HandleRxEvent(const ENetEvent &rxEvent)
{
  // Handle packet
  // TODO
  printf("\t Client %d says: %s\n", rxEvent.peer->incomingPeerID, rxEvent.packet->data);

  // Free packet
  enet_packet_destroy(rxEvent.packet);
}

void PubSubBrokerNetNode::NetworkTxPayload(const std::string &topic, const char * msg, uint16_t len)
{
  // Build packet
  char * payload = new char[len];
  // TODO

  // Send packet
  ENetPacket *packet = enet_packet_create(payload, len, 0);
  enet_host_broadcast(m_pNetwork, 0, packet);
}