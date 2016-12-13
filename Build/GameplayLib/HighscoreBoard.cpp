#include "HighscoreBoard.h"

#include <algorithm>
#include <sstream>

#include <ncltech\Utility.h>

HighscoreBoard::HighscoreBoard(PubSubBroker *broker)
    : IPubSubClient(broker)
{
  // Subscribe to topics
  if (broker != nullptr)
  {
    broker->Subscribe(this, "highscore/add");
    broker->Subscribe(this, "highscore/list");
    broker->Subscribe(this, "highscore/load");
    broker->Subscribe(this, "highscore/save");
  }
}

HighscoreBoard::~HighscoreBoard()
{
}

size_t HighscoreBoard::AddScore(const LocalScore *score, const std::string &name)
{
  return AddScore(score->GetScoreCounter(), name);
}

size_t HighscoreBoard::AddScore(float score, const std::string &name)
{
  m_highScores.push_back({name, score});
  std::sort(m_highScores.begin(), m_highScores.end(), [](HighscoreRecord &a, HighscoreRecord &b) { return a.score > b.score; });
  // TODO
  return 0;
}

bool HighscoreBoard::HandleSubscription(const std::string &topic, const char *msg, uint16_t len)
{
  if (topic == "highscore/add")
  {
    std::vector<std::string> tokens = Utility::Split(std::string(msg), ',');
    float score = std::stof(tokens[1]);

    size_t position = AddScore(score, tokens[0]);

    std::string ack = std::to_string(position);
    m_broker->BroadcastMessage(this, topic, ack.c_str(), (uint16_t)ack.size());
  }
  else if (topic == "highscore/list")
  {
    size_t numToList = std::atoi(msg);

    std::stringstream str;
    for (size_t i = 0; i < numToList; i++)
    {
      if (1 > 0)
        str << ',';

      str << m_highScores[i].name << ',' << m_highScores[i].score;
    }

    std::string ack = str.str();
    m_broker->BroadcastMessage(this, topic, ack.c_str(), (uint16_t)ack.size());
  }
  else if (topic == "highscore/load")
  {
    std::string filename(msg);
    char result = (char)LoadFromFile(filename);
    m_broker->BroadcastMessage(this, topic, &result, 1);
  }
  else if (topic == "highscore/save")
  {
    std::string filename(msg);
    char result = (char)SaveToFile(filename);
    m_broker->BroadcastMessage(this, topic, &result, 1);
  }
  else
  {
    return false;
  }

  return true;
}

bool HighscoreBoard::LoadFromFile(const std::string &filename)
{
  // TODO
  return false;
}

bool HighscoreBoard::SaveToFile(const std::string &filename)
{
  // TODO
  return false;
}