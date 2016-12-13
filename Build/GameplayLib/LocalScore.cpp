#include "LocalScore.h"

LocalScore::LocalScore()
    : m_startingScore(0.0f)
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

void LocalScore::AwardPoints(float points)
{
  m_score += m_bonus + (m_modifier * points);
}