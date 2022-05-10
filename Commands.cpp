#include <unistd.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <time.h>
#include <utime.h>
#include <cstring>
#include <unistd.h>

using namespace std;

#if 0
#define FUNC_ENTRY() \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT() \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

const std::string WHITESPACE = " \n\r\t\f\v";

string _ltrim(const std::string &s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string &s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string &s)
{
  return _rtrim(_ltrim(s));
}

bool isNumbericString(std::string str)
{
  while (str.size() != 0)
  {
    if (str[0] > '9' || str[0] < '0')
    {
      return false;
    }
    str.substr(1);
  }
  return true;
}

int _parseCommandLine(std::string cmd_line, char **args)
{
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for (std::string s; iss >> s;)
  {
    args[i] = (char *)malloc(s.length() + 1);
    memset(args[i], 0, s.length() + 1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(std::string cmd_line)
{
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char *cmd_line)
{
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos)
  {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&')
  {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h

SmallShell::SmallShell()
{

  // TODO: add your implementation
}

SmallShell::~SmallShell()
{
  // TODO: add your implementation
}

/**
 * Creates and returns a pointer to Command class which matches the given command line (cmd_line)
 */
Command *SmallShell::CreateCommand(std::string cmd_line)
{
  // For example:
  /*
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    if (firstWord.compare("pwd") == 0) {
      return new GetCurrDirCommand(cmd_line);
    }
    else if (firstWord.compare("showpid") == 0) {
      return new ShowPidCommand(cmd_line);
    }
    else if ...
    .....
    else {
      return new ExternalCommand(cmd_line);
    }
    */
  return nullptr;
}

void SmallShell::executeCommand(std::string cmd_line)
{
  // TODO: Add your implementation here
  Command *cmd = Command::getInstance(cmd_line, &jobs_);
  jobs_.removeFinishedJobs();
  cmd->execute();
  delete cmd;
  // for example:
  // Command* cmd = CreateCommand(cmd_line);
  // cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)
}

Prompt::Prompt()
{
  text_ = std::string("smash");
}

std::string &Prompt::getPrompt()
{
  return text_;
}

void Prompt::setPrompt(std::string new_prompt)
{
  text_ = new_prompt;
}

Command::Command(std::string &cmd_line) : cmd_line_(cmd_line) {}

BuiltInCommand::BuiltInCommand(std::vector<std::string> cmd_words, std::string &cmd_line) : Command(cmd_line), cmd_words_(cmd_words) {}

ChPromptCommand::ChPromptCommand(std::vector<std::string> cmd_words, std::string &cmd_line) : BuiltInCommand(cmd_words, cmd_line) {}

void ChPromptCommand::execute()
{
  Prompt &prompt = Prompt::getInstance();
  if (cmd_words_.size() == 1)
  {
    prompt.setPrompt();
    return;
  }
  prompt.setPrompt(cmd_words_[1]);
}

std::string Command::getCommandLine()
{
  return cmd_line_;
}

std::vector<std::string> BuiltInCommand::getBuiltInCommandWords()
{
  return cmd_words_;
}

ShowPwdCommand::ShowPwdCommand(std::vector<std::string> cmd_words, std::string &cmd_line) : BuiltInCommand(cmd_words, cmd_line) {}

void ShowPwdCommand::execute()
{
  /*if (cmd_words_.size() != 1)
  {
    return;
  }
  */
  char path[PATH_MAX_LENGTH];
  char *result = getcwd(path, PATH_MAX_LENGTH);
  if (result != nullptr)
  {
    std::cout << path << "\n";
  }
}

BuiltInCommandNamesToNumbers::BuiltInCommandNamesToNumbers()
{
  static const std::vector<std::string> cmd_names = {
      "chprompt",
      "showpid",
      "pwd",
      "cd",
      "jobs",
      "kill",
      "fg",
      "bg",
      "quit",
  };
  for (size_t i = 0; i < cmd_names.size(); i++)
  {
    commands_map_[cmd_names[i]] = i + 1;
  }
}

int BuiltInCommandNamesToNumbers::getCommandNumber(const std::string &cmd_name)
{
  std::map<std::string, int>::iterator cmd_iterator = commands_map_.find(cmd_name);
  if (cmd_iterator == commands_map_.end())
  {
    return 0;
  }
  return cmd_iterator->second;
}

std::vector<std::string> split(std::string str, char split_by = ' ')
{
  std::vector<std::string> vec;
  str = _trim(str);
  size_t index;
  while (str.size() > 0)
  {
    index = str.find(split_by);
    if (index == str.npos)
    {
      vec.push_back(str);
      return vec;
    }
    vec.push_back(str.substr(0, index));
    str = str.substr(index);
    str = _ltrim(str);
  }
  return vec;
}

void JobsCommand::execute()
{
  jobs_->removeFinishedJobs();
  for (map<int, JobsList::JobEntry *>::iterator iter = jobs_->jobs_.begin();
       iter != jobs_->jobs_.end(); iter++)
  {
    std::cout << *(iter->second);
  }
}

std::string JobsList::JobEntry::getCommandText()
{
  return command_text_;
}

JobsCommand::JobsCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs) : BuiltInCommand(cmd_words, cmd_line), jobs_(jobs) {}

Command *Command::getInstance(std::string cmd_line, JobsList *jobs)
{
  std::string cmd_line_copy = cmd_line;
  cmd_line_copy = _rtrim(cmd_line_copy);
  // bool is_foreground_command = true;
  if (cmd_line_copy[cmd_line_copy.size() - 1] == '&')
  {
    // is_foreground_command = false;
    cmd_line_copy.substr(0, cmd_line_copy.size() - 1); // removes the '&' from cmd_line
  }
  std::vector<std::string> cmd_words = split(cmd_line_copy);
  std::string command_type = cmd_words[0];
  BuiltInCommandNamesToNumbers &built_in_map = BuiltInCommandNamesToNumbers::getInstance();
  Command *cmd_object_ptr = nullptr;
  enum commannd_numbers
  {
    CHPROMPT = 1,
    SHOWPID = 2,
    PWD = 3,
    CD = 4,
    JOBS = 5,
    KILL = 6,
    FG = 7,
    BG = 8,
    QUIT = 9,
  };
  switch (built_in_map.getCommandNumber(command_type))
  {
  case CHPROMPT:
    cmd_object_ptr = new ChPromptCommand(cmd_words, cmd_line);
    return cmd_object_ptr;
  case SHOWPID:
    cmd_object_ptr = new ShowPidCommand(cmd_words, cmd_line);
    return cmd_object_ptr;
  case PWD:
    cmd_object_ptr = new ShowPwdCommand(cmd_words, cmd_line);
    return cmd_object_ptr;
  case CD:
    cmd_object_ptr = new ChangeDirCommand(cmd_words, cmd_line);
    return cmd_object_ptr;
  case JOBS:
    cmd_object_ptr = new JobsCommand(cmd_words, cmd_line, jobs);
    return cmd_object_ptr;
  case KILL:
    cmd_object_ptr = new KillCommand(cmd_words, cmd_line, jobs);
    return cmd_object_ptr;
  case FG:
    cmd_object_ptr = new ForegroundCommand(cmd_words, cmd_line, jobs);
    return cmd_object_ptr;
  /*case BG:
    cmd_object_ptr = new BackgroundCommand(cmd_words, cmd_line, jobs);
    return cmd_object_ptr;
  case QUIT:
    cmd_object_ptr = new QuitCommand(cmd_words, cmd_line, jobs);
    return cmd_object_ptr;
    */
  default:
    // the command is not a built in command
    cmd_object_ptr = new ExternalCommand(cmd_line, jobs);
    return cmd_object_ptr;
  }
  return cmd_object_ptr;
}

int JobsList::getJobsAmount()
{
  if (foreground_job_ == nullptr)
  {
    return jobs_.size();
  }
  return jobs_.size() + 1;
}

ForegroundCommand::ForegroundCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs) : BuiltInCommand(cmd_words, cmd_line), jobs_(jobs) {}

void ForegroundCommand::execute()
{
  if (cmd_words_.size() > 2 || (cmd_words_.size() == 2 && !isNumbericString(cmd_words_[1])))
  {
    std::cerr << "smash error: fg: invalid arguments\n";
    return;
  }
  if (cmd_words_.size() == 1 && jobs_->getJobsAmount() == 0)
  {
    std::cerr << "smash error: fg: jobs list is empty\n";
    return;
  }
  if (cmd_words_.size() == 2 && !jobs_->isJobIdExist(stoi(cmd_words_[1])))
  {
    std::cerr << "smash error: fg: job-id <job-id> does not exist\n";
    return;
  }

  int new_foreground_job_id = cmd_words_.size() == 1 ? jobs_->getMaxJobId() : stoi(cmd_words_[1]);
  jobs_->moveToForegound(new_foreground_job_id);
}

void JobsList::moveToForegound(int jobs_id)
{
  foreground_job_ = jobs_[jobs_id];
  jobs_.erase(jobs_id);
  pid_t p = jobs_[jobs_id]->getProcessId();
  waitpid(p, nullptr, 0);
  removeForegroundJob();
}

KillCommand::KillCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs) : BuiltInCommand(cmd_words, cmd_line), jobs_(jobs) {}

bool JobsList::isJobIdExist(int job_id)
{
  return foreground_job_->getJobId() == job_id || jobs_.find(job_id) != jobs_.end();
}

void KillCommand::execute()
{
  if (cmd_words_.size() != 3 || cmd_words_[1][0] != '-' ||
          isNumbericString(cmd_words_[1].substr(1)),
      isNumbericString(cmd_words_[0]))
  {
    std::cerr << "smash error: kill: invalid arguments\n";
    return;
  }
  if (!jobs_->isJobIdExist(stoi(cmd_words_[2])))
  {
    std::cerr << "smash error: kill: job-id " << cmd_words_[2] << " does not exist\n";
    return;
  }
  int result = kill(stoi(cmd_words_[2]), stoi(cmd_words_[1].substr(1)));
  if (result != 0)
  {
    perror("smash error: kill failed");
  }
}

ChangeDirCommand::ChangeDirCommand(std::vector<std::string> cmd_words, std::string cmd_line) : BuiltInCommand(cmd_words, cmd_line) {}

std::string getCurrentPath()
{
  std::string current_path;
  char path[PATH_MAX_LENGTH];
  char *result = getcwd(path, PATH_MAX_LENGTH);
  if (result == nullptr)
  {
    current_path = "/home"; // defualt path value
  }
  else
  {
    current_path = path;
  }
  return current_path;
}

void ChangeDirCommand::execute()
{
  if (cmd_words_.size() > 2)
  {
    std::cerr << "smash error: cd: too many arguments\n";
    return;
  }
  if (cmd_words_.size() < 2)
  {
    std::cerr << "smash error: cd: need an argument\n";
    return;
  }
  std::string current_path = getCurrentPath();
  LastPath &last_path = LastPath::getInstance();
  if (cmd_words_[1] == "-")
  {
    chdir(last_path.getLastPath().c_str());
  }
  else
  {
    chdir(cmd_words_[1].c_str());
  }
  last_path.setLastPath(current_path);
}

LastPath::LastPath()
{
  full_last_path_ = getCurrentPath();
}

std::string &LastPath::getLastPath()
{
  return full_last_path_;
}

void LastPath::setLastPath(std::string new_last_path)
{
  full_last_path_ = new_last_path;
}

// Creating PidCommand Object
ShowPidCommand::ShowPidCommand(std::vector<std::string> cmd_words, std::string cmd_line) : BuiltInCommand(cmd_words, cmd_line) {}

void ShowPidCommand::execute()
{
  /*getting the pid with a system call. Cannot use the DO_SYS
    macro because it doesnt allow us to return a value */
  pid_t smash_pid = getpid();
  // outputting the pid per the requested foramt
  std::cout << "smash pid is " << smash_pid << "\n";
}

int JobsList::JobEntry::getProcessId()
{
  return process_id_;
}

int JobsList::JobEntry::getJobId()
{
  return job_id_;
}

bool JobsList::JobEntry::getIsStopped()
{
  return is_stopped_;
}

void JobsList::JobEntry::setIsStopped(bool new_is_stopped)
{
  is_stopped_ = new_is_stopped;
}

std::ostream &operator<<(std::ostream &os, const JobsList::JobEntry &job)
{
  os << "[" << job.job_id_ << "] " << job.command_text_ << " : "
     << job.process_id_ << " " << difftime(time(nullptr), job.start_time_);
  if (job.is_stopped_)
  {
    os << " (stopped)";
  }
  return os << "\n";
}

JobsList::JobEntry::JobEntry(int job_id, int process_id,
                             std::string command_text,
                             bool is_stopped,
                             time_t start_time) : job_id_(job_id),
                                                  process_id_(process_id),
                                                  command_text_(command_text),
                                                  is_stopped_(is_stopped),
                                                  start_time_(start_time) {}

JobsList::JobsList() {}

JobsList::~JobsList() {}

ExternalCommand::ExternalCommand(std::string cmd_line, JobsList *jobs) : Command(cmd_line), jobs_(jobs) {}

void callBashExec(ExternalCommand *cmd)
{
  std::string str = cmd->getCommandLine();
  char *chars_str = new char[str.size()];
  std::cout << chars_str;
  int size = str.size();
  strncpy(chars_str, _rtrim(str).substr(0, size - 1).c_str(), size);
  execl("/bin/bash", "bash", "-c", chars_str, nullptr);
  std::cout << errno << "\n";
  // if we are here, the exec has failed.
  delete[] chars_str;
  std::cerr << "exec has failed!\n";
  exit(errno);
}

void JobsList::removeForegroundJob()
{
  delete foreground_job_;
  foreground_job_ = nullptr;
}

void ExternalCommand::execute()
{
  bool isForeground = !finishedWithAmparsand();
  jobs_->removeFinishedJobs();
  pid_t p = jobs_->addJob(this, isForeground);
  if (p == 0)
  {
    callBashExec(this);
  }
  else
  {
    if (isForeground)
    {
      waitpid(p, nullptr, 0);
      jobs_->removeForegroundJob();
    }
  }
}

int JobsList::getMaxJobId()
{
  int max_job_id = 0;
  if (jobs_.size() >= 1)
  {
    max_job_id = jobs_.rbegin()->second->getJobId();
  }
  if (foreground_job_ != nullptr)
  {
    max_job_id = max(max_job_id, foreground_job_->getJobId());
  }
  return max_job_id;
}

bool Command::finishedWithAmparsand()
{
  std::string str = _rtrim(cmd_line_);
  return str[str.size() - 1] == '&';
}

pid_t JobsList::addJob(Command *cmd, bool isForeground, bool isStopped)
{
  int max_job_id = getMaxJobId();
  pid_t p = fork();
  if (p < 0)
  {
    perror("smash error: fork failed");
  }
  if (p == 0)
  {
    return p;
  }
  else
  {
    max_job_id++;
    JobEntry *new_job = new JobEntry(max_job_id, p,
                                     cmd->getCommandLine(),
                                     isStopped, time(nullptr));
    if (isForeground)
    {
      foreground_job_ = new_job;
    }
    else
    {
      jobs_[max_job_id] = new_job;
    }
    return p;
  }
}

void JobsList::printJobsList()
{
  for (map<int, JobEntry *>::iterator iter = jobs_.begin();
       iter != jobs_.end(); iter++)
  {
    std::cout << iter->second;
  }
}

void JobsList::killAllJobs()
{
  for (std::map<int, JobEntry *>::iterator iter = jobs_.begin();
       iter != jobs_.end(); iter++)
  {
    JobEntry *job = iter->second;
    kill(job->getProcessId(), KILL_SIGNAL);
  }
}

void JobsList::removeFinishedJobs()
{
  std::vector<int> toErase;
  for (std::map<int, JobEntry *>::iterator iter = jobs_.begin();
       iter != jobs_.end(); iter++)
  {
    JobEntry *job = iter->second;

    if (waitpid(job->getProcessId(), nullptr, WNOHANG) > 0)
    {
      toErase.push_back(iter->first);
    }
  }
  for (unsigned i = 0; i < toErase.size(); i++)
  {
    int job_id = toErase[i];
    delete jobs_[job_id];
    jobs_.erase(job_id);
  }
}

JobsList::JobEntry *JobsList::getJobById(int jobId)
{
  return jobs_[jobId];
}

void JobsList::removeJobById(int jobId)
{
  jobs_.erase(jobId);
}
