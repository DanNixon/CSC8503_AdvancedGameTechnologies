#pragma once

#include <ncltech/StateMachine.h>

class Ball
{
public:
  Ball();
  virtual ~Ball();

protected:
  StateMachine m_stateMachine;
};