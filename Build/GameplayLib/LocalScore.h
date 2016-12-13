#pragma once

class LocalScore
{
public:
  LocalScore();
  virtual ~LocalScore();

  virtual void SetStartingScore(float startingScore);
  virtual void SetModifier(float modifier);
  virtual void SetBonus(float bonus);

  inline float GetDefault() const
  {
    return m_startingScore;
  }

  inline float GetModifier() const
  {
    return m_modifier;
  }

  inline float GetBonus() const
  {
    return m_bonus;
  }

  virtual void Reset();

  virtual void DeltaPoints(float points);

  inline float GetScoreCounter() const
  {
    return m_score;
  }

protected:
  float m_startingScore; //!< Default starting score
  float m_modifier;      //!< Score modifier coefficient
  float m_bonus;         //!< Additional score added on score submission

  float m_score; //!< Current score
};