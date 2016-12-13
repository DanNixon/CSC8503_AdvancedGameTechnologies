#pragma once

// clang-format off
#include <winsock2.h>
#include <enet\enet.h>
#include <iostream>
// clang-format on

#include "ClientCLI.h"

int main(int argc, char **argv)
{
  // Init ENet
  if (enet_initialize() != 0)
  {
    fprintf(stderr, "An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }

  // Init and run CLI
  ClientCLI cli(std::cin, std::cout);
  cli.InitCLI();
  int exitCode = cli.run();

  system("pause");
  exit(exitCode);
}