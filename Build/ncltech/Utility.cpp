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