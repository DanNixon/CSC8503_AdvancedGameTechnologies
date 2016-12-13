/** @file */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief Represents reasons for termination and their associated exit codes.
 */
enum CommandExit
{
  COMMAND_EXIT_CLEAN = 0,
  COMMAND_EXIT_EXCEPTION = -1,
  COMMAND_EXIT_TOO_FEW_ARGUMENTS = -2,
  COMMAND_EXIT_SUBCOMMANDNOTFOUND = -3
};

/**
 * @class Command
 * @author Dan Nixon
 * @brief Stores data for a single executable CLI command.
 */
class Command
{
public:
  /**
   * @brief Function type for command handler functions.
   *
   * Takes a reference to the input and output streams and a vector of string
   * parameters as parameters and returns an exit code.
   */
  typedef std::function<int(std::istream &in, std::ostream &out, std::vector<std::string> &argv)> CMDHandlerFunc;

public:
  /**
   * @brief Creates a new command.
   * @param commandName Name of the command
   * @param func Function to be executed when command is invoked
   * @param numArguments Number of arguments required for this command
   * @param desc Short description of the command
   *
   * commandName is what is matched to input on the command line when selecting
   * the command to be executed.
   *
   * desc is shown when the CommandContainer::help() function is invoked.
   */
  Command(const std::string &commandName, CMDHandlerFunc func, size_t numArguments = 0, const std::string &desc = "")
      : m_commandName(commandName)
      , m_handlerFunc(func)
      , m_numArguments(numArguments)
      , m_description(desc)
  {
  }

  virtual ~Command()
  {
  }

  /**
   * @brief Gets the name of the command.
   * @return Command name
   */
  inline std::string commandName() const
  {
    return m_commandName;
  }

  /**
   * @brief Gets a description of the command.
   * @return Command description
   */
  inline std::string description() const
  {
    return m_description;
  }

  /**
   * @brief Handles execution of the command function.
   * @param in Input stream
   * @param out Output stream
   * @param tokens Parameters
   * @return Exit code of command
   *
   * The first element of the tokens vector contains the name of the command
   * being executed (as per argv convention).
   */
  inline int handleCmdFunc(std::istream &in, std::ostream &out, std::vector<std::string> &tokens) const
  {
    if (tokens.size() < m_numArguments)
    {
      out << "Too few arguments (got " << tokens.size() << ", expected " << m_numArguments << ").\n";
      return COMMAND_EXIT_TOO_FEW_ARGUMENTS;
    }

    try
    {
      return m_handlerFunc(in, out, tokens);
    }
    catch (std::runtime_error &rte)
    {
      out << "Runtime error: " << rte.what() << '\n';
      return COMMAND_EXIT_EXCEPTION;
    }
    catch (...)
    {
      out << "Unknown exception\n";
      return COMMAND_EXIT_EXCEPTION;
    }
  }

private:
  const std::string m_commandName;
  const CMDHandlerFunc m_handlerFunc;
  const size_t m_numArguments;
  const std::string m_description;
};

/**
 * @brief Shared pointer to a Command.
 */
typedef std::shared_ptr<Command> Command_ptr;

/**
 * @brief Shared pointer to a const Command.
 */
typedef std::shared_ptr<const Command> Command_const_ptr;