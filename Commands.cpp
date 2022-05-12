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
#include <algorithm>
#include <iostream>
#include <fstream>

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
  for (unsigned i = 0; i < str.size(); i++)
  {
    if (str[i] > '9' || str[i] < '0')
    {
      return false;
    }
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
  JobsList &jobs = JobsList::getInstance();
  Command *cmd = Command::getInstance(cmd_line, &jobs);
  jobs.removeFinishedJobs();
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
    fprintf(getOutFile(), "%s\n", path);
  }
}

CommandNamesToNumbers::CommandNamesToNumbers()
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
      "tail",
      "touch"};
  for (size_t i = 0; i < cmd_names.size(); i++)
  {
    commands_map_[cmd_names[i]] = i + 1;
  }
}

int CommandNamesToNumbers::getCommandNumber(const std::string &cmd_name)
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
  if (split_by == ' ')
  {
    str = _trim(str);
  }
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
    str = str.substr(index + 1);
    if (split_by == ' ')
    {
      str = _ltrim(str);
    }
  }
  return vec;
}

void JobsCommand::execute()
{
  jobs_->removeFinishedJobs();
  for (map<int, JobsList::JobEntry *>::iterator iter = jobs_->jobs_.begin();
       iter != jobs_->jobs_.end(); iter++)
  {
    fprintf(getOutFile(), "%s", iter->second->toString().c_str());
  }
}

std::string JobsList::JobEntry::getCommandText()
{
  return command_text_;
}

JobsCommand::JobsCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs) : BuiltInCommand(cmd_words, cmd_line), jobs_(jobs) {}

TailCommand::TailCommand(std::vector<std::string> cmd_words, std::string cmd_line) : BuiltInCommand(cmd_words, cmd_line) {}

void TailCommand::execute()
{
  int size = cmd_words_.size();
  if (size > 3 || size == 1 ||
      (size == 3 &&
       (cmd_words_[1][0] != '-' ||
        cmd_words_[1].size() < 2 ||
        !isNumbericString(cmd_words_[1].substr(1)))))
  {
    fprintf(getErrFile(), "smash error: tail: invalid arguments\n");
    return;
  }
  std::string file_name = size == 2 ? cmd_words_[1] : cmd_words_[2];
  int lines_amount = size == 2 ? 10 : stoi(cmd_words_[1].substr(1));
  std::ifstream file;
  file.open(file_name);
  std::string line;
  for (int i = 0; i < lines_amount; i++)
  {
    std::getline(file, line);
    if (file.eof())
    {
      break;
    }
    fprintf(getOutFile(), "%s\n", line.c_str());
  }
  file.close();
}

/*TouchCommand::TouchCommand(std::vector<std::string> cmd_words, std::string cmd_line) : BuiltInCommand(cmd_words, cmd_line) {}

void TouchCommand::execute()
{

}*/

Command *Command::getCommandFromTheRightType(std::vector<std::string> cmd_words,
                                             std::string cmd_line, JobsList *jobs)
{
  std::string command_type = cmd_words[0];
  CommandNamesToNumbers &commands_map = CommandNamesToNumbers::getInstance();
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
    TAIL = 10,
    TOUCH = 11
  };
  switch (commands_map.getCommandNumber(command_type))
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
  case BG:
    cmd_object_ptr = new BackgroundCommand(cmd_words, cmd_line, jobs);
    return cmd_object_ptr;
  case QUIT:
    cmd_object_ptr = new QuitCommand(cmd_words, cmd_line, jobs);
    return cmd_object_ptr;
  case TAIL:
    cmd_object_ptr = new TailCommand(cmd_words, cmd_line);
    return cmd_object_ptr;
  /*case TOUCH:
    cmd_object_ptr = new TouchCommand(cmd_words, cmd_line);
    return cmd_object_ptr;*/
  default:
    // the command is not a built-in command
    cmd_object_ptr = new ExternalCommand(cmd_line, jobs);
    return cmd_object_ptr;
  }
}

FILE *Command::getInFile()
{
  return in_file_;
}
FILE *Command::getOutFile()
{
  return out_file_;
}
FILE *Command::getErrFile()
{
  return err_file_;
}
void Command::setInFile(FILE *in_file)
{
  in_file_ = in_file;
}
void Command::setOutFile(FILE *out_file)
{
  out_file_ = out_file;
}
void Command::setErrFile(FILE *err_file)
{
  err_file_ = err_file;
}

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
  if (std::find(cmd_line.begin(), cmd_line.end(), '|') != cmd_line.end())
  {
    // this is a pipeline command
    return new PipeCommand(cmd_line);
  }
  if (std::find(cmd_line.begin(), cmd_line.end(), '>') != cmd_line.end())
  {
    // this is an RedirectionCommand command
    return new RedirectionCommand(cmd_line);
  }
  return getCommandFromTheRightType(cmd_words, cmd_line, jobs);
}

void RedirectionCommand::execute()
{
  cmd_->execute();
  fclose(input_file_);
}

RedirectionCommand::RedirectionCommand(std::string cmd_line) : Command(cmd_line)
{
  std::vector<std::string> cmd_parts = split(cmd_line, '>');
  char writing_type[2] = "w";
  if (cmd_parts[1] == "")
  {
    // we contains ">>"
    writing_type[0] = 'a';
    cmd_parts.erase(++cmd_parts.begin());
  }
  JobsList &jobs = JobsList::getInstance();
  cmd_ = Command::getInstance(cmd_parts[0], &jobs);
  std::string file_name = _trim(cmd_parts[1]);
  input_file_ = fopen(file_name.c_str(), writing_type);
  if (input_file_ == nullptr)
  {
    perror("fopen failed\n");
  }
  else
  {
    cmd_->setOutFile(input_file_);
  }
}

void PipeCommand::execute()
{
  first_cmd_->execute();
  second_cmd_->execute();
  fclose(fds_[0]);
  fclose(fds_[1]);
}

PipeCommand::PipeCommand(std::string cmd_line) : Command(cmd_line)
{
  std::vector<std::string> cmd_parts = split(cmd_line, '|');
  bool is_to_cerr = false;
  if (cmd_parts[1][0] == '&')
  {
    is_to_cerr = true;
    cmd_parts[1] = cmd_parts[1].substr(1);
  }
  JobsList &jobs = JobsList::getInstance();
  first_cmd_ = Command::getInstance(cmd_parts[0], &jobs);
  second_cmd_ = Command::getInstance(cmd_parts[1], &jobs);
  int tmp_fds[2];
  if (pipe(tmp_fds) < 0)
  {
    perror("pipe creation failed.\n");
  }
  fds_[0] = fdopen(tmp_fds[0], "r");
  fds_[1] = fdopen(tmp_fds[1], "w");
  if (is_to_cerr)
  {
    first_cmd_->setErrFile(fds_[1]);
  }
  else
  {
    first_cmd_->setOutFile(fds_[1]);
  }
  second_cmd_->setInFile(fds_[0]);
}

QuitCommand::QuitCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs) : BuiltInCommand(cmd_words, cmd_line), jobs_(jobs) {}

void QuitCommand::execute()
{
  if (cmd_words_.size() == 2 && cmd_words_[1] == "kill")
  {
    fprintf(getOutFile(), "smash: sending SIGKILL signal to %d jobs:\n", jobs_->getJobsAmount());
    jobs_->killAllJobsWithPrinting();
  }
  exit(0);
}

BackgroundCommand::BackgroundCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs) : BuiltInCommand(cmd_words, cmd_line), jobs_(jobs) {}

void BackgroundCommand::execute()
{
  if (cmd_words_.size() > 2 || (cmd_words_.size() == 2 && !isNumbericString(cmd_words_[1])))
  {
    fprintf(getErrFile(), "smash error: fg: invalid arguments\n");
    return;
  }
  JobsList::JobEntry *job_to_resume = nullptr;
  if (cmd_words_.size() == 1)
  {
    job_to_resume = jobs_->getLastStoppedJob();
    if (job_to_resume == nullptr)
    {
      fprintf(getErrFile(), "smash error: bg: there is no stopped jobs to resume\n");
      return;
    }
  }
  else
  { // cmd_words_.size() == 2
    int job_id = stoi(cmd_words_[1]);
    if (!jobs_->isJobIdExist(job_id))
    {
      fprintf(getErrFile(), "smash error: bg: job-id <job-id> does not exist\n");
      return;
    }
    job_to_resume = jobs_->getJobById(job_id);
    if (!job_to_resume->isStopped())
    {
      fprintf(getErrFile(), "smash error: bg: job-id <job-id fg > is already running in the background\n");
      return;
    }
  }
  fprintf(getOutFile(), "%s\n", job_to_resume->getCommandText().c_str());
  jobs_->resumeJobById(job_to_resume->getJobId());
}

void JobsList::resumeJobById(int job_id)
{
  JobEntry *job = getJobById(job_id);
  job->setIsStopped(false);
  stopped_jobs_ids_.remove(job_id);
  kill(job->getProcessId(), SIGCONT);
}

JobsList::JobEntry *JobsList::getLastStoppedJob()
{
  if (stopped_jobs_ids_.size() == 0)
  {
    return nullptr;
  }
  JobsList::JobEntry *last_stopped_job = getJobById(stopped_jobs_ids_.front());
  stopped_jobs_ids_.pop_front();
  return last_stopped_job;
}

int JobsList::getJobsAmount()
{
  return jobs_.size();
}

ForegroundCommand::ForegroundCommand(std::vector<std::string> cmd_words,
                                     std::string cmd_line, JobsList *jobs) : BuiltInCommand(cmd_words, cmd_line), jobs_(jobs) {}

void ForegroundCommand::execute()
{

  if (cmd_words_.size() > 2 || (cmd_words_.size() == 2 && !isNumbericString(cmd_words_[1])))
  {
    fprintf(getErrFile(), "smash error: fg: invalid arguments\n");
    return;
  }

  if (cmd_words_.size() == 1 && jobs_->getJobsAmount() == 0)
  {
    fprintf(getErrFile(), "smash error: fg: jobs list is empty\n");
    return;
  }

  if (cmd_words_.size() == 2 && !jobs_->isJobIdExist(stoi(cmd_words_[1])))
  {
    fprintf(getErrFile(), "smash error: fg: job-id <job-id> does not exist\n");
    return;
  }
  int new_foreground_job_id = cmd_words_.size() == 2 ? stoi(cmd_words_[1]) : jobs_->getMaxJobId();
  jobs_->moveToForegound(new_foreground_job_id);
  fprintf(getOutFile(), "%s : %d\n", jobs_->getForegroundJob()->getCommandText().c_str(), jobs_->getForegroundJobPid());
}

void JobsList::moveToForegound(int job_id)
{
  foreground_job_ = jobs_[job_id];
  jobs_.erase(job_id);
  if (foreground_job_->isStopped() == true)
  {
    resumeJobById(job_id);
  }
  pid_t p = foreground_job_->getProcessId();
  int stat;
  waitpid(p, &stat, WUNTRACED);
  if (stat == 0)
  {
    removeForegroundJob();
  }
}

KillCommand::KillCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs) : BuiltInCommand(cmd_words, cmd_line), jobs_(jobs) {}

bool JobsList::isJobIdExist(int job_id)
{
  return (foreground_job_ != nullptr && foreground_job_->getJobId() == job_id) || jobs_.find(job_id) != jobs_.end();
}

void KillCommand::execute()
{
  if (cmd_words_.size() != 3 || cmd_words_[1][0] != '-' ||
          isNumbericString(cmd_words_[1].substr(1)),
      isNumbericString(cmd_words_[0]))
  {
    fprintf(getErrFile(), "smash error: kill: invalid arguments\n");
    return;
  }
  if (!jobs_->isJobIdExist(stoi(cmd_words_[2])))
  {
    fprintf(getErrFile(), "smash error: kill: job-id %s does not exist\n", cmd_words_[2].c_str());
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
    fprintf(getErrFile(), "smash error: cd: too many arguments\n");
    return;
  }
  if (cmd_words_.size() < 2)
  {
    fprintf(getErrFile(), "smash error: cd: need an argument\n");
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
  fprintf(getOutFile(), "smash pid is %d\n", smash_pid);
}

int JobsList::JobEntry::getProcessId()
{
  return process_id_;
}

int JobsList::JobEntry::getJobId()
{
  return job_id_;
}

bool JobsList::JobEntry::isStopped()
{
  return is_stopped_;
}

void JobsList::JobEntry::setIsStopped(bool new_is_stopped)
{
  is_stopped_ = new_is_stopped;
}

std::string JobsList::JobEntry::toString()
{
  std::string str = "[" + to_string(job_id_) + "] " + command_text_ + " : " + to_string(process_id_) + " " + to_string(difftime(time(nullptr), start_time_));
  if (is_stopped_)
  {
    str += " (stopped)";
  }
  return str;
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

JobsList::~JobsList()
{
  // delete JobEntrys
  for (std::map<int, JobEntry *>::iterator iter = jobs_.begin();
       iter != jobs_.end(); iter++)
  {
    delete iter->second;
  }
  if (foreground_job_ != nullptr)
  {
    delete foreground_job_;
  }
}

ExternalCommand::ExternalCommand(std::string cmd_line, JobsList *jobs) : Command(cmd_line), jobs_(jobs) {}

void callBashExec(ExternalCommand *cmd)
{
  std::string str = cmd->getCommandLine();
  char *chars_str = new char[str.size()];
  fprintf(cmd->getOutFile(), "%s", chars_str);
  str = _rtrim(str);
  int size = str.size();
  strncpy(chars_str, str.c_str(), size + 1);
  if (chars_str[size - 1] == '&')
  {
    chars_str[size - 1] = '\0';
  }
  execl("/bin/bash", "bash", "-c", chars_str, nullptr);
  // if we are here, the exec has failed.
  delete[] chars_str;
  fprintf(cmd->getErrFile(), "exec has failed!\n");
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
      int stat;
      waitpid(p, &stat, WUNTRACED);
      if (stat == 0)
      {
        jobs_->removeForegroundJob();
      }
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
  int new_job_id = getMaxJobId() + 1;
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
    JobEntry *new_job = new JobEntry(new_job_id, p,
                                     cmd->getCommandLine(),
                                     isStopped, time(nullptr));
    if (isForeground)
    {
      foreground_job_ = new_job;
    }
    else
    {
      jobs_[new_job_id] = new_job;
    }
    return p;
  }
}

void JobsList::killAllJobsWithPrinting()
{
  for (std::map<int, JobEntry *>::iterator iter = jobs_.begin();
       iter != jobs_.end(); iter++)
  {
    JobEntry *job = iter->second;
    std::cout << job->getProcessId() << ": " << job->getCommandText() << "\n";
    kill(job->getProcessId(), SIGKILL);
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
    stopped_jobs_ids_.remove(job_id);
    delete jobs_[job_id];
    jobs_.erase(job_id);
  }
}

JobsList::JobEntry *JobsList::getJobById(int jobId)
{
  if (foreground_job_ != nullptr && foreground_job_->getJobId() == jobId)
  {
    return foreground_job_;
  }
  return jobs_[jobId];
}

void JobsList::removeJobById(int jobId)
{
  jobs_.erase(jobId);
  stopped_jobs_ids_.remove(jobId);
}

int JobsList::getForegroundJobPid()
{
  return foreground_job_->getProcessId();
}

JobsList::JobEntry *JobsList::getForegroundJob()
{
  return foreground_job_;
}

void JobsList::stopForegroundJob()
{
  foreground_job_->setIsStopped(true);
  int forented_job_id = foreground_job_->getJobId();
  stopped_jobs_ids_.push_front(forented_job_id);
  jobs_[forented_job_id] = foreground_job_;
  foreground_job_ = nullptr;
}