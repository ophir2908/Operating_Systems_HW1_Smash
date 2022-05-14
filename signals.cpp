#include <iostream>
#include <signal.h>
#include <unistd.h>

#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlCHandler(int sig_num)
{
  std::cout << "smash: got ctrl-C\n";
  JobsList &jobs = JobsList::getInstance();
  if (jobs.getForegroundJob() == nullptr)
  {
    return;
  }
  std::cout << "smash: process " << jobs.getForegroundJobPid() << " was killed\n";
  kill(jobs.getForegroundJobPid(), SIGKILL);
  jobs.removeForegroundJob();
}

void ctrlZHandler(int sig_num)
{
  std::cout << "smash: got ctrl-Z\n";
  JobsList &jobs = JobsList::getInstance();
  if (jobs.getForegroundJob() == nullptr)
  {
    return;
  }
  std::cout << "smash: process " << jobs.getForegroundJobPid() << " was stopped\n";
  kill(jobs.getForegroundJobPid(), SIGSTOP);
  jobs.stopForegroundJob();
}

void alarmHandler(int sig_num)
{
  // TODO: Add your implementation
  TimeStamp &time_stamp = TimeStamp::getInstance();
  TimeoutData *timeout_data = time_stamp.getFirst();
  if (--timeout_data->delta_ <= 0)
  {
    std::cout << "smash: got an alarm\n";
    std::cout << "smash: " << timeout_data->cmd_line_ << " timed out!\n";
    JobsList &jobs = JobsList::getInstance();
    pid_t pid_to_kill = jobs.getJobById(timeout_data->job_id_)->getProcessId();
    kill(pid_to_kill, SIGKILL);
    time_stamp.pop();
  }
  if (!time_stamp.empty())
  {
    alarm(1);
  }
}
