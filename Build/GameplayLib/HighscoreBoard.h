#pragma once

#include <string>

#include "LocalScore.h"

class HighscoreBoard
{
public:
  struct HighscoreRecord
  {
    std::string name;
    float score;
  };

public:
  HighscoreBoard();
  virtual ~HighscoreBoard();

  virtual void AddScore(const LocalScore *score);
};