#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <string.h>
#include <vector>
#include <map>
#include <list>
#define PATH_MAX_LENGTH (200)
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

#define DO_SYS(syscall)               \
  do                                  \
  {                                   \
    /* safely invoke a system call */ \
    if ((syscall) == -1)              \
    {                                 \
      perror(#syscall);               \
      exit(1);                        \
    }                                 \
    while (0)

class JobsList;
class Command
{
protected:
  Command(std::string &cmd_line);

  std::string cmd_line_;
  // bool is_foreground_command_;
  // TODO: Add your data members
public:
  bool finishedWithAmparsand();
  std::string getCommandLine();
  virtual ~Command() = default;
  virtual void execute() = 0;
  static Command *getInstance(std::string cmd_line, JobsList *jobs);
  // virtual void prepare();
  // virtual void cleanup();
  //  TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command
{
protected:
  std::vector<std::string> cmd_words_;

public:
  BuiltInCommand(std::vector<std::string> cmd_params, std::string &cmd_line);
  std::vector<std::string> getBuiltInCommandWords();
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command
{
  JobsList *jobs_;

public:
  ExternalCommand(std::string cmd_line, JobsList *jobs);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command
{
  // TODO: Add your data members
public:
  PipeCommand(std::string cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command
{
  // TODO: Add your data members
public:
  explicit RedirectionCommand(std::string cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  // void prepare() override;
  // void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand
{
  std::string lastPwd_;
  // TODO: Add your data members public:
public:
  ChangeDirCommand(std::vector<std::string> cmd_params, std::string cmd_line);
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand
{
public:
  GetCurrDirCommand(std::string cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand
{
public:
  ShowPidCommand(std::vector<std::string> cmd_params, std::string cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand
{
  JobsList *jobs_;
public: 
  // TODO: Add your data members public:
  QuitCommand(std::vector<std::string>, JobsList *jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};

class JobsCommand;
class JobsList
{
public:
  class JobEntry
  {
    const int job_id_;
    const int process_id_;
    const std::string command_text_;
    bool is_stopped_;
    const time_t start_time_;
    friend std::ostream &operator<<(std::ostream &os, const JobsList::JobEntry &job);

  public:
    JobEntry(int job_id, int process_id, std::string command_text_, bool is_stopped, time_t start_time_);
    int getProcessId();
    std::string getCommandText();
    int getJobId();
    bool isStopped();
    void setIsStopped(bool new_is_stopped);
    // TODO: Add your data members
  };

private:
  std::map<int, JobEntry *> jobs_;
  JobEntry *foreground_job_;
  std::list<int> stopped_jobs_ids_;
  // TODO: Add your data members
  friend JobsCommand;
  JobsList();

public:
  int getMaxJobId();
  ~JobsList();
  int getForegroundJobPid();
  JobEntry *getForegroundJob();
  void moveToForegound(int job_id);
  void removeForegroundJob();
  bool isJobIdExist(int job_id);
  pid_t addJob(Command *cmd, bool isForeground, bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry *getJobById(int jobId);
  void removeJobById(int jobId);
  int getJobsAmount();
  JobsList(JobsList const &) = delete;       // disable copy ctor
  void operator=(JobsList const &) = delete; // disable = operator
  static JobsList &getInstance()             // make JobsList singleton
  {
    static JobsList instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  void stopForegroundJob();
  void resumeJobById(int job_id);
  JobEntry *getLastStoppedJob();
  // JobEntry *getLastJob(int *lastJobId);
  //  TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand
{
  // TODO: Add your data members
  JobsList *jobs_;

public:
  JobsCommand(std::vector<std::string> cmd_params, std::string cmd_line, JobsList *jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand
{
  JobsList *jobs_;
  // TODO: Add your data members
public:
  KillCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand
{
  JobsList *jobs_;
  // TODO: Add your data members
public:
  ForegroundCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand
{
  JobsList *jobs_;
  // TODO: Add your data members
public:
  BackgroundCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class TailCommand : public BuiltInCommand
{
public:
  TailCommand(std::string cmd_line);
  virtual ~TailCommand() {}
  void execute() override;
};

class TouchCommand : public BuiltInCommand
{
public:
  TouchCommand(std::string cmd_line);
  virtual ~TouchCommand() {}
  void execute() override;
};

class SmallShell
{
private:
  // TODO: Add your data members
  SmallShell();

public:
  Command *CreateCommand(std::string cmd_line);
  SmallShell(SmallShell const &) = delete;     // disable copy ctor
  void operator=(SmallShell const &) = delete; // disable = operator
  static SmallShell &getInstance()             // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(std::string cmd_line);
  // TODO: add extra methods as needed
};

// ------------- new classes -------------

class Prompt
{
private:
  std::string text_;
  Prompt();

public:
  Prompt(Prompt const &) = delete;         // disable copy ctor
  void operator=(Prompt const &) = delete; // disable = operator
  static Prompt &getInstance()             // make Prompt singleton
  {
    static Prompt instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~Prompt() = default;
  std::string &getPrompt();
  void setPrompt(std::string new_prompt = std::string("smash"));
};

class LastPath
{
  // a factory with path test to the last directory we were in.
private:
  std::string full_last_path_;
  LastPath();

public:
  LastPath(LastPath const &) = delete;       // disable copy ctor
  void operator=(LastPath const &) = delete; // disable = operator
  static LastPath &getInstance()             // make Prompt singleton
  {
    static LastPath instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~LastPath() = default;
  std::string &getLastPath();
  void setLastPath(std::string new_last_path);
};

class BuiltInCommandNamesToNumbers
{
private:
  std::map<std::string, int> commands_map_;
  BuiltInCommandNamesToNumbers();

public:
  BuiltInCommandNamesToNumbers(BuiltInCommandNamesToNumbers const &) = delete; // disable copy ctor
  void operator=(BuiltInCommandNamesToNumbers const &) = delete;               // disable = operator
  static BuiltInCommandNamesToNumbers &getInstance()                           // make BuiltInCommandNamesToNumbers singleton
  {
    static BuiltInCommandNamesToNumbers instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~BuiltInCommandNamesToNumbers() = default;
  int getCommandNumber(const std::string &cmd_name);
};

class ChPromptCommand : public BuiltInCommand
{
public:
  ChPromptCommand(std::vector<std::string> cmd_params, std::string &cmd_line);
  virtual ~ChPromptCommand() = default;
  void execute() override;
};

class ShowPwdCommand : public BuiltInCommand
{
  // TODO: Add your data members public:
public:
  ShowPwdCommand(std::vector<std::string> cmd_params, std::string &cmd_line);
  virtual ~ShowPwdCommand() {}
  void execute() override;
};


#endif // SMASH_COMMAND_H_
