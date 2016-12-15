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
  static std::string Join(const std::vector<std::string> &strings, char delim = ',');
  static std::string SanitizeFilename(std::string str);
  static bool StringToBool(std::string str, bool defaultVal = false);

  static void ParseIPAddress(uint8_t *data, const std::string &str);

  static bool MakeDirectories(const std::string &path);
  static std::string GetHomeConfigDirectory(const std::string &appName);
};
