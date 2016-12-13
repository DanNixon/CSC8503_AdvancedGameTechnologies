#include "HighscoreBoard.h"

HighscoreBoard::HighscoreBoard()
{
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
  // TODO
  return 0;
}