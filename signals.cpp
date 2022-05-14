#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlCHandler(int sig_num)
{
  std::cout << "smash: got ctrl-C\n";
  JobsList &jobs = JobsList::getInstance();
  std::cout << "smash: process " << jobs.getForegroundJobPid() << " was killed\n";
  if (jobs.getForegroundJob() == nullptr) {
    return;
  }
  kill(jobs.getForegroundJobPid(), SIGKILL);
  jobs.removeForegroundJob();
}

void ctrlZHandler(int sig_num)
{
  std::cout << "smash: got ctrl-Z\n";
  JobsList &jobs = JobsList::getInstance();
  std::cout << "smash: process " << jobs.getForegroundJobPid() << " was stopped\n";
  if (jobs.getForegroundJob() == nullptr) {
    return;
  }
  kill(jobs.getForegroundJobPid(), SIGSTOP);
  jobs.stopForegroundJob();
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}
