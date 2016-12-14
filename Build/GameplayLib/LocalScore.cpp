#include "LocalScore.h"

#include <ncltech\NCLDebug.h>
#include <ncltech\Utility.h>

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
  // TODO
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
    NCLDebug::Log("Score ranking: %l", position);
  }
  else if (topic == "highscore/list")
  {
    std::vector<std::string> tokens = Utility::Split(std::string(msg), ',');

    NCLDebug::Log("High scores:");
    for (size_t i = 0; i < tokens.size(); i += 2)
    {
      std::string name = tokens[i];
      if (name.empty())
        name = "[no name]";

      NCLDebug::Log("  %d) %5.0f - %s", (i / 2) + 1, name.c_str(), tokens[i + 1].c_str());
    }
  }
  else
  {
    return false;
  }

  return true;
}
