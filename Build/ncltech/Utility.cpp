#include "Utility.h"

#define NOMINMAX

#include <Windows.h>
#include <algorithm>
#include <shlobj.h>
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
 * @brief Joins a vector of strings on a delimiter.
 * @param strings Vector of strings to join
 * @param delimiter Delimiter to join on
 * @return Joined string
 */
std::string Utility::Join(const std::vector<std::string> &strings, char delim)
{
  std::stringstream str;

  for (auto it = strings.begin(); it != strings.end(); ++it)
  {
    if (it != strings.begin())
      str << delim;

    str << *it;
  }

  return str.str();
}

/**
 * @brief Converts a string into a string that can be a filename/path by removing whitespace and punctuation.
 * @param str String to sanitize
 * @return Sanitized string
 *
 * e.g. SanitizeFilename("Snooker Loopy!") = "SnookerLoopy"
 */
std::string Utility::SanitizeFilename(std::string str)
{
  str.erase(std::remove_if(str.begin(), str.end(), [](const char &c) { return ispunct(c) || isspace(c); }), str.end());
  return str;
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

/**
 * @brief Creates a new diretcory tree.
 * @param path Path to create
 * @return True if path was created
 */
bool Utility::MakeDirectories(const std::string &path)
{
  return CreateDirectory(path.c_str(), NULL) == TRUE;
}

/**
 * @brief Gets a diretory in th ehome directory in which to stopre configurations and persistent data (e.g. high scores).
 * @param appName Name of the game/application
 * @return Directory path (empty on failure)
 */
std::string Utility::GetHomeConfigDirectory(const std::string &appName)
{
  std::string directory;

  WCHAR path[MAX_PATH];
  char charPath[MAX_PATH];
  char c = ' ';

  if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path)) &&
      SUCCEEDED(WideCharToMultiByte(CP_ACP, 0, path, -1, charPath, 260, &c, NULL)))
  {
    std::string sanitizedName = SanitizeFilename(appName);
    directory = std::string(charPath) + "\\." + sanitizedName + "\\";
  }

  // Ensure the directory exists
  MakeDirectories(directory);

  return directory;
}
