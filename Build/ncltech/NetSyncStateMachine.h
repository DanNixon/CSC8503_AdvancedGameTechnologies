#pragma once

#include "StateMachine.h"

#include "IPubSubClient.h"

/**
 * @class NetSyncStateMachine
 * @author Dan Nixon
 * @brief State machine that can be synchronised with identical state machines over a publisher/subscriber broker.
 */
class NetSyncStateMachine : public StateMachine, public IPubSubClient
{
public:
  NetSyncStateMachine(PubSubBroker *broker, const std::string &rxTopic, const std::string &txTopic);
  virtual ~NetSyncStateMachine();

  virtual bool Transfer();
  virtual bool HandleSubscription(const std::string &topic, const char *msg, uint16_t len) override;

protected:
  const std::string m_txTopic; //!< Topic used to broadcast state changes to
  bool m_txBlanking;           //!< Flag indicating that no broadcast should take place
};
