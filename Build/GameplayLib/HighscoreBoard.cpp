#include "HighscoreBoard.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include <ncltech\Utility.h>

/**
 * @brief Create a new high score board.
 * @param broker Pub/sub broker used to communicate
 */
HighscoreBoard::HighscoreBoard(PubSubBroker *broker)
    : IPubSubClient(broker)
    , m_highscoreFilename(Utility::GetHomeConfigDirectory("GameTech") + "highscores.txt")
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

/**
 * @brief Adds a new score to the high score board.
 * @param score Score record
 * @param name Player name
 * @return Position of the score in the rankings
 */
size_t HighscoreBoard::AddScore(const LocalScore *score, const std::string &name)
{
  return AddScore(score->GetScore(), name);
}

/**
 * @brief Adds a new score to the high score board.
 * @param score Score value
 * @param name Player name
 * @return Position of the score in the rankings
 */
size_t HighscoreBoard::AddScore(float score, const std::string &name)
{
  HighscoreRecord newRecord = {name, score};
  m_highScores.push_back(newRecord);
  std::sort(m_highScores.begin(), m_highScores.end(), [](HighscoreRecord &a, HighscoreRecord &b) { return a.score > b.score; });
  auto it = std::find_if(m_highScores.begin(), m_highScores.end(), [newRecord](HighscoreRecord &r) { return r == newRecord; });
  return (size_t)(it - m_highScores.begin());
}

/**
 * @copydoc IPubSubClient::HandleSubscription
 */
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
    size_t numToList = std::min((size_t)std::atoi(msg), m_highScores.size());

    std::stringstream str;
    for (size_t i = 0; i < numToList; i++)
    {
      if (i > 0)
        str << ',';

      str << m_highScores[i].name << ',' << m_highScores[i].score;
    }

    std::string ack = str.str();
    m_broker->BroadcastMessage(this, topic, ack.c_str(), (uint16_t)ack.size());
  }
  else if (topic == "highscore/load")
  {
    char result = (msg[0] == 'L' && LoadFromFile()) ? '1' : '0';
    m_broker->BroadcastMessage(this, topic, &result, 1);
  }
  else if (topic == "highscore/save")
  {
    char result = (msg[0] == 'S' && SaveToFile()) ? '1' : '0';
    m_broker->BroadcastMessage(this, topic, &result, 1);
  }
  else
  {
    return false;
  }

  return true;
}

/**
 * @brief Loads scores from an ASCII file.
 * @return True if loading was successful
 */
bool HighscoreBoard::LoadFromFile()
{
  std::ifstream file;

  // Open file
  file.open(m_highscoreFilename, std::ios_base::in);
  if (!file.is_open())
    return false;

  // Clear existing high scores
  m_highScores.clear();

  // Parse new high scores
  std::string line;
  while (std::getline(file, line))
  {
    std::vector<std::string> tokens = Utility::Split(line, ',');
    if (tokens.size() != 2)
      continue;
    m_highScores.push_back({tokens[0], std::stof(tokens[1])});
  }

  file.close();
  return !m_highScores.empty();
}

/**
 * @brief Saves scores to an ASCII file.
 * @return True if saving was successful
 */
bool HighscoreBoard::SaveToFile()
{
  std::ofstream file;

  // Open file
  file.open(m_highscoreFilename, std::ios_base::out | std::ios_base::trunc);
  if (!file.is_open())
    return false;

  // Output high scores to file
  for (auto it = m_highScores.begin(); it != m_highScores.end(); ++it)
    file << it->name << ',' << it->score << '\n';

  file.close();
  return true;
}
