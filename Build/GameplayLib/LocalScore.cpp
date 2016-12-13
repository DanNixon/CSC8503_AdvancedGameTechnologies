#include "LocalScore.h"

#include <ncltech\NCLDebug.h>
#include <ncltech\Utility.h>

LocalScore::LocalScore(PubSubBroker * broker)
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

void LocalScore::SetStartingScore(float startingScore)
{
  m_startingScore = startingScore;
}

void LocalScore::SetModifier(float modifier)
{
  m_modifier = modifier;
}

void LocalScore::SetBonus(float bonus)
{
  m_bonus = bonus;
}

void LocalScore::Reset()
{
  m_score = m_startingScore;
}

void LocalScore::DeltaPoints(float points)
{
  m_score += m_bonus + (m_modifier * points);
}

bool LocalScore::HandleSubscription(const std::string & topic, const char * msg, uint16_t len)
{
  if (topic == "highscore/add")
  {
    bool result = (msg == "1");

    if (result)
      NCLDebug::Log("New high score!");
    else
      NCLDebug::Log("No new high score...");
  }
  else if (topic == "highscore/list")
  {
    std::vector<std::string> tokens = Utility::Split(std::string(msg), ',');

    NCLDebug::Log("High scores:");
    for (size_t i = 0; i < tokens.size(); i += 2)
      NCLDebug::Log("  %d) %5.0f - %s", (i / 2) + 1, tokens[i].c_str(), tokens[i + 1].c_str());
  }
  else
  {
    return false;
  }

  return true;
}
