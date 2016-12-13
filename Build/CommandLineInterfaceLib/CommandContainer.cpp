/** @file */

#include "CommandContainer.h"

#include <algorithm>
#include <iomanip>

/**
 * @brief Indentation length of descriptions on "help" command.
 */
const size_t CommandContainer::HELP_CMD_WIDTH = 20;

/**
 * @brief Create a new container.
 *
 * Implicitly adds the "help" command listing all commands in this container.
 */
CommandContainer::CommandContainer()
{
  /* Add help command */
  m_commands.push_back(std::make_shared<Command>("help",
                                                 [this](std::istream &in, std::ostream &out, std::vector<std::string> argv) {
                                                   this->help(out);
                                                   return 0;
                                                 },
                                                 0, "Shows command usage."));
}

CommandContainer::~CommandContainer()
{
  m_commands.clear();
}

/**
 * @brief Adds a new command to the container.
 * @param command Pointer to command
 */
void CommandContainer::registerCommand(Command_ptr command)
{
  m_commands.push_back(command);
}

/**
 * @brief Handles input from the command line.
 * @param in Input stream
 * @param out Output stream
 * @param tokens Vector of tokens to process
 * @return Exit code of command
 *
 * The first elements of the tokens vector is the name of the command that is
 * searched for.
 */
int CommandContainer::handle(std::istream &in, std::ostream &out, std::vector<std::string> &tokens)
{
  if (tokens.size() == 0)
  {
    help(out);
    return -1;
  }

  auto it = std::find_if(m_commands.begin(), m_commands.end(), [tokens](Command_ptr c) { return c->commandName() == tokens[0]; });

  if (it == m_commands.end())
  {
    out << "Command \"" << tokens[0] << "\" not found.\n";
    return COMMAND_EXIT_SUBCOMMANDNOTFOUND;
  }

  return (*it)->handleCmdFunc(in, out, tokens);
}

/**
 * @brief Prints usage information for all commands in this container to a
 *        stream.
 * @param out Output stream to print usage to
 */
void CommandContainer::help(std::ostream &out)
{
  out << "Command usage:\n";

  for (auto it = m_commands.begin(); it != m_commands.end(); ++it)
    out << ' ' << std::left << std::setw(HELP_CMD_WIDTH) << (*it)->commandName() << ": " << (*it)->description() << '\n';
}