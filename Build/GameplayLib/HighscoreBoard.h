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

  virtual size_t AddScore(const LocalScore *score, const std::string &name = "");
  virtual size_t AddScore(float score, const std::string &name = "");
};