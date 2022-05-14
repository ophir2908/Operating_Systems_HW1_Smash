#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char* argv[]) {
    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    //TODO: setup sig alarm handler
    SmallShell& smash = SmallShell::getInstance();
    Prompt& prompt = Prompt::getInstance();
    while(true) {
        std::cout << prompt.getPrompt() << "> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        if (cmd_line.empty()) { // OPHIR: remove?
            continue;
        }
        smash.executeCommand(cmd_line);
    }
    return 0;
}
