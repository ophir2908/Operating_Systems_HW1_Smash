#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char *argv[])
{
    if (signal(SIGALRM, alarmHandler) == SIG_ERR)
    {
        perror("smash error: failed to set alarm handler");
    }
    if (signal(SIGTSTP, ctrlZHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if (signal(SIGINT, ctrlCHandler) == SIG_ERR)
    {
        perror("smash error: failed to set ctrl-C handler");
    }

    // TODO: setup sig alarm handler
    SmallShell &smash = SmallShell::getInstance();
    Prompt &prompt = Prompt::getInstance();
    bool did_quit = false;
    while (!did_quit)
    {
        std::cout << prompt.getPrompt() << "> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        if (_ltrim(cmd_line).empty())
        {
            continue;
        }
        smash.executeCommand(cmd_line, &did_quit);
    }
    return 0;
}
