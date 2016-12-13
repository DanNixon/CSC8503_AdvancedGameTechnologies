/** @file */

#pragma once

#include "Command.h"
#include "CommandContainer.h"

#include <memory>

/**
 * @class SubCommand
 * @author Dan Nixon
 * @brief Command that holds a collection of additional commands.
 */
class SubCommand : public Command, public CommandContainer
{
public:
  /**
   * @brief Creates a new subcommand.
   * @param commandName Name of the command
   * @param desc Short description of the command
   */
  SubCommand(const std::string &commandName, const std::string &desc = "")
      : Command(commandName,
                [this](std::istream &in, std::ostream &out, std::vector<std::string> &argv) {
                  argv.erase(argv.begin());
                  return this->handle(in, out, argv);
                },
                0, desc)
      , CommandContainer()
  {
  }

  virtual ~SubCommand()
  {
  }
};

/**
 * @brief Shared pointer to a SubCommand.
 */
typedef std::shared_ptr<SubCommand> SubCommand_ptr;

/**
 * @brief Shared pointer to a const SubCommand.
 */
typedef std::shared_ptr<const SubCommand> SubCommand_const_ptr;