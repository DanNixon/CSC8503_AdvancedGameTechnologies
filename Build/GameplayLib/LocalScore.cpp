#include "LocalScore.h"

#include <ncltech\NCLDebug.h>
#include <ncltech\Utility.h>

#include "HighscoreBoard.h"

/**
 * @brief Creates a new local score counter.
 * @param broker Pub/sub broker used to communicate eith the high score board
 */
LocalScore::LocalScore(PubSubBroker *broker)
    : IPubSubClient(broker)
    , m_startingScore(0.0f)
    , m_modifier(1.0f)
    , m_bonus(0.0f)
    , m_score(0.0f)
{
  // Subscribe to topics
  if (broker != nullptr)
  {
    broker->Subscribe(this, "highscore/add");
    broker->Subscribe(this, "highscore/list");
  }
}

LocalScore::~LocalScore()
{
}

/**
 * @brief Sets the default starting score.
 * @param startingScore Default score
 */
void LocalScore::SetStartingScore(float startingScore)
{
  m_startingScore = startingScore;
}

/**
 * @brief Sets the score modifier coefficient.
 * @param modifier Score modifier
 */
void LocalScore::SetModifier(float modifier)
{
  m_modifier = modifier;
}

/**
 * @brief Sets the additive score bonus.
 * @param bonus Score bonus
 */
void LocalScore::SetBonus(float bonus)
{
  m_bonus = bonus;
}

/**
 * @brief Submits this score to the high score board.
 * @param name Player name
 */
void LocalScore::Submit(const std::string &name)
{
  if (m_broker != nullptr)
  {
    std::string msg = name + ',' + std::to_string(m_score);
    m_broker->BroadcastMessage(this, "highscore/add", msg.c_str(), (uint16_t)msg.size());
  }
}

/**
 * @brief Requests high scores from the sevrer score board.
 * @param Number of scores to request
 *
 * Scores are output onto the in game log.
 */
void LocalScore::RequestHighscores(size_t num)
{
  if (m_broker != nullptr)
  {
    std::string msg = std::to_string(num);
    m_broker->BroadcastMessage(this, "highscore/list", msg.c_str(), (uint16_t)msg.size());
  }
}

/**
 * @brief Resets the score counter to the default score.
 */
void LocalScore::Reset()
{
  m_score = m_startingScore;
}

/**
 * @brief Award (or remove) points.
 * @param points Point difference
 */
void LocalScore::DeltaPoints(float points)
{
  m_score += m_bonus + (m_modifier * points);
}

/**
 * @copydoc IPubSubClient::HandleSubscription
 */
bool LocalScore::HandleSubscription(const std::string &topic, const char *msg, uint16_t len)
{
  if (topic == "highscore/add")
  {
    long position = std::atol(msg);
    NCLDebug::Log("Score ranking: %d", position);
  }
  else if (topic == "highscore/list")
  {
    std::vector<std::string> tokens = Utility::Split(std::string(msg), ',');

    // This is most likely a request from another client, not a reponse
    if (tokens.size() % 2 != 0)
      return false;

    NCLDebug::Log("High scores:");
    for (size_t i = 0; i < tokens.size(); i += 2)
    {
      std::string name = tokens[i];
      if (name.empty())
        name = "[no name]";

      NCLDebug::Log("  %d) %s - %s", (i / 2) + 1, name.c_str(), tokens[i + 1].c_str());
    }
  }
  else
  {
    return false;
  }

  return true;
}
