#pragma once

#include <functional>

/**
* @typedef IBehaviourList
* @brief List of pointers to IBehaviour instances.
*/
typedef std::vector<IBehaviour *> IBehaviourList;

class IBehaviour
{
public:
  IBehaviour()
  {
  }

  virtual ~IBehaviour()
  {
  }
};