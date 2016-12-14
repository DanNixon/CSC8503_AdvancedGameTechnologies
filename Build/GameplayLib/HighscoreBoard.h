#pragma once

#include <string>
#include <vector>

#include <ncltech\IPubSubClient.h>

#include "LocalScore.h"

/**
 * @class HighscoreBoard
 * @author Dan Nixon
 * @brief Container for high scores.
 */
class HighscoreBoard : public IPubSubClient
{
public:
  /**
   * @brief Stores data for a single high score record.
   */
  struct HighscoreRecord
  {
    std::string name; //!< Player name
    float score;      //!< Score

    bool operator==(const HighscoreRecord & other) const
    {
      return name == other.name && score == other.score;
    }
  };

public:
  HighscoreBoard(PubSubBroker *broker);
  virtual ~HighscoreBoard();

  /**
   * @brief Gets the number of scores that have been recorded.
   * @return Score count
   */
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
