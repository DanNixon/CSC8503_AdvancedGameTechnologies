#include "PubSubBrokerNetNode.h"

PubSubBrokerNetNode::PubSubBrokerNetNode()
{
}

PubSubBrokerNetNode::~PubSubBrokerNetNode()
{
  m_connections.clear();

  // Close network
  Release();
}

void PubSubBrokerNetNode::ConnectToBroker(const uint8_t ip[4], uint16_t port)
{
  ENetPeer *peer = ConnectPeer(ip[0], ip[1], ip[2], ip[3], port);
  // m_connections.push_back(peer);
}

bool PubSubBrokerNetNode::IsConnectedToServer() const
{
  if (m_connections.empty())
    return false;

  return m_connections.back()->state == ENET_PEER_STATE_CONNECTED;
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
  ENetPacket *packet = rxEvent.packet;
  char *data = (char *)packet->data;

  // Handle packet
  auto it = std::find(data, data + packet->dataLength, '=');
  if (it != data + packet->dataLength)
  {
    // Parse packet data
    std::string topic(data, it);

    std::vector<char> pl(it + 1, data + packet->dataLength);
    const char *payload = pl.data();

    printf("- Message from client %d on topic %s: %s\n", rxEvent.peer->incomingPeerID, topic.c_str(), payload);

    // Broadcast message
    BroadcastMessage(nullptr, topic, payload, (uint16_t)pl.size(), rxEvent.peer->incomingPeerID);
  }
  else
  {
    printf("- Message from client %d: malformed packet\n", rxEvent.peer->incomingPeerID);
  }

  // Free packet
  enet_packet_destroy(packet);
}

void PubSubBrokerNetNode::BroadcastMessage(IPubSubClient *source, const std::string &topic, const char *msg, uint16_t len,
                                           int32_t ignorePeer)
{
  // Length (topic length + equals + payload length + null terminator)
  uint16_t packetLen = (uint16_t)topic.length() + len + 2;

  // Build packet
  char *payload = new char[packetLen];
  const char *topicStr = topic.c_str();
  char *p = payload;

  // Topic
  memcpy_s(p, strlen(topicStr), topicStr, strlen(topicStr));
  p += strlen(topicStr);

  // Seperator
  *(p++) = '=';

  // Payload
  memcpy_s(p, (rsize_t)len, msg, (rsize_t)len);
  p += len;

  // NULL terminate
  *(p++) = '\0';

  // Send packet
  ENetPacket *packet = enet_packet_create(payload, packetLen, 0);

  for (auto it = m_connections.begin(); it != m_connections.end(); ++it)
  {
    if ((*it)->incomingPeerID != ignorePeer)
      enet_peer_send(*it, 0, packet);
  }

  PubSubBroker::BroadcastMessage(source, topic, msg, len);
}