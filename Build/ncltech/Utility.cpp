#include "Utility.h"

#include <sstream>

/**
 * @brief Splits a string by a delimiter.
 * @param str String to split
 * @param delim Delimiter
 * @return Vector of substrings
 */
std::vector<std::string> Utility::Split(const std::string &str, char delim)
{
  std::vector<std::string> retVal;
  std::stringstream ss(str);
  std::string item;

  while (std::getline(ss, item, delim))
  {
    if (!item.empty())
      retVal.push_back(item);
  }

  return retVal;
}

/**
 * @brief Parses an IP address from a string.
 * @param data Storage for IP address
 * @param str String containing IP address
 *
 * Format is the standard IPv4 standard, i.e. 127.0.0.1
 */
void Utility::ParseIPAddress(uint8_t *data, const std::string &str)
{
  std::vector<std::string> tokens = Split(str, '.');

  for (size_t i = 0; i < 4; i++)
    data[i] = std::stoi(tokens[i]);
}
