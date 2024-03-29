#pragma once

#include <ncltech\NetworkBase.h>
#include <ncltech\Scene.h>

class Net1_Client : public Scene
{
public:
  Net1_Client(const std::string &friendly_name);

  virtual void OnInitializeScene() override;
  virtual void OnCleanupScene() override;
  virtual void OnUpdateScene(float dt) override;

  void ProcessNetworkEvent(const ENetEvent &evnt);

protected:
  Object *m_pObj;

  NetworkBase m_Network;
  ENetPeer *m_pServerConnection;
};