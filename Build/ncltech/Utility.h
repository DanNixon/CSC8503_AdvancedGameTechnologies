#pragma once

#include <string>
#include <vector>

/**
 * @class Utility
 * @author Dan Nixon
 * @brief Contains utlity functions for general use.
 */
class Utility
{
public:
  static std::vector<std::string> Split(const std::string &str, char delim = '/');
};
