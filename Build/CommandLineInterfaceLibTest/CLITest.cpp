#include <sstream>
#include <string>
#include <CppUnitTest.h>

#include <CLI.h>
#include <Command.h>
#include <SubCommand.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CLITest)
{
public:	
  TEST_METHOD(CLI_Prompt_Help_Exit)
  {
    // Simulated input
    std::stringstream in("help\nexit\n");

    // Capture output
    std::stringstream out;

    CLI c(in, out);

    const std::string expected =
      "> "
      "Command usage:\n"
      " help                : Shows command usage.\n"
      " exit                : Exit the application.\n"
      "> ";

    Assert::AreEqual(0, c.run());
    Assert::AreEqual(expected, out.str());
  }

  TEST_METHOD(CLI_MissingCommand)
  {
    // Simulated input
    std::stringstream in("nope\nexit\n");

    // Capture output
    std::stringstream out;

    CLI c(in, out);

    const std::string expected =
      "> "
      "Command \"nope\" not found.\n"
      "> ";

    Assert::AreEqual(0, c.run());
    Assert::AreEqual(expected, out.str());
  }

  TEST_METHOD(CLI_TooFewArguments)
  {
    // Simulated input
    std::stringstream in("test 111\nexit\n");

    // Capture output
    std::stringstream out;

    CLI c(in, out);

    c.registerCommand(Command_ptr(new Command(
      "test",
      [](std::istream &in, std::ostream &out, std::vector<std::string> &argv) {
        out << "Test command.\n";
        return 0;
      }, 3,
      "Is a test.")));

    const std::string expected =
      "> "
      "Too few arguments (got 2, expected 3).\n"
      "> ";

    Assert::AreEqual(0, c.run());
    Assert::AreEqual(expected, out.str());
  }

  TEST_METHOD(CLI_SubCommand)
  {
    // Simulated input
    std::stringstream in("test\nsub1 help\nsub1 list\nexit\n");

    // Capture output
    std::stringstream out;

    CLI c(in, out);

    c.registerCommand(Command_ptr(new Command(
      "test",
      [](std::istream &in, std::ostream &out, std::vector<std::string> &argv) {
        out << "Test command.\n";
        return 0;
      }, 0,
      "Is a test.")));

    SubCommand_ptr sub1(new SubCommand("sub1", "Test subcommand."));

    sub1->registerCommand(Command_ptr(new Command(
      "list",
      [](std::istream &in, std::ostream &out, std::vector<std::string> &argv) {
        out << "Test command => list.\n";
        return 0;
      }, 0,
      "Is a test 2.")));

    c.registerCommand(sub1);

    const std::string expected =
      "> "
      "Test command.\n"
      "> "
      "Command usage:\n"
      " help                : Shows command usage.\n"
      " list                : Is a test 2.\n"
      "> "
      "Test command => list.\n"
      "> ";

    Assert::AreEqual(0, c.run());
    Assert::AreEqual(expected, out.str());
  }
};