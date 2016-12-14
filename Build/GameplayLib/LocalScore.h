#pragma once

#include <ncltech\IPubSubClient.h>

/**
 * @class LocalScore
 * @author Dan Nixon
 * @brief Utility class for recording a player score.
 */
class LocalScore : public IPubSubClient
{
public:
  LocalScore(PubSubBroker *broker);
  virtual ~LocalScore();

  virtual void SetStartingScore(float startingScore);
  virtual void SetModifier(float modifier);
  virtual void SetBonus(float bonus);

  /**
   * @brief Gets the default score.
   * @return Default score
   */
  inline float GetDefault() const
  {
    return m_startingScore;
  }

  /**
   * @brief Gets the score modifier coefficient.
   * @return Score modifier
   */
  inline float GetModifier() const
  {
    return m_modifier;
  }

  /**
   * @brief Gets the bonus amount.
   * @return Bonus
   */
  inline float GetBonus() const
  {
    return m_bonus;
  }

  virtual void Submit(const std::string &name = "Player");
  virtual void Reset();

  virtual void DeltaPoints(float points);

  /**
   * @brief Gets the current score.
   * @return Score
   */
  inline float GetScore() const
  {
    return m_score;
  }

  virtual bool HandleSubscription(const std::string &topic, const char *msg, uint16_t len);

protected:
  float m_startingScore; //!< Default starting score
  float m_modifier;      //!< Score modifier coefficient
  float m_bonus;         //!< Additional score added on score submission

  float m_score; //!< Current score
};
