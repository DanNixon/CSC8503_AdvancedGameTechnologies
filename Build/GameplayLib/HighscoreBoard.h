#pragma once

#include <string>
#include <vector>

#include <ncltech\IPubSubClient.h>

#include "LocalScore.h"

class HighscoreBoard : public IPubSubClient
{
public:
  struct HighscoreRecord
  {
    std::string name;
    float score;
  };

public:
  HighscoreBoard(PubSubBroker *broker);
  virtual ~HighscoreBoard();

  inline size_t NumScores() const
  {
    return m_highScores.size();
  }

  virtual size_t AddScore(const LocalScore *score, const std::string &name = "");
  virtual size_t AddScore(float score, const std::string &name = "");

  virtual bool HandleSubscription(const std::string &topic, const char *msg, uint16_t len);

  bool LoadFromFile(const std::string &filename);
  bool SaveToFile(const std::string &filename);

protected:
  std::vector<HighscoreRecord> m_highScores; //!< Current high scores
};