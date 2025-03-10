#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <string.h>
#include <vector>
#include <map>
#include <list>
#define PATH_MAX_LENGTH (200)
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define MAX_SIGNUM (64)

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
  static Command *getCommandFromTheRightType(std::vector<std::string> cmd_words,
                                             std::string cmd_line, JobsList *jobs,
                                             bool *did_quit, FILE **fds, bool is_foreground_command);

  FILE *in_file_ = stdin;
  FILE *out_file_ = stdout;
  FILE *err_file_ = stderr;
  bool does_need_redirect_[3] = {false, false, false};

protected:
  Command(std::string &cmd_line);
  Command(std::string &cmd_line, FILE *in_file, FILE *out_file, FILE *err_file);

  std::string cmd_line_;
  // bool is_foreground_command_;
  // TODO: Add your data members
public:
  virtual FILE **getFds();

  bool pipeRedirectInput();
  bool pipeRedirectOutput();
  bool pipeRedirectError();
  void setPipeRedirectInput(bool does_need_redirect_in);
  void setPipeRedirectOutput(bool does_need_redirect_out);
  void setPipeRedirectError(bool does_need_redirect_err);
  FILE *getInFile();
  FILE *getOutFile();
  FILE *getErrFile();
  void setInFile(FILE *in_file);
  void setOutFile(FILE *out_file);
  void setErrFile(FILE *err_file);
  virtual bool isExternal();
  bool finishedWithAmparsand();
  std::string getCommandLine();
  virtual ~Command() = default;
  virtual void execute() = 0;
  static Command *getInstance(std::string cmd_line, JobsList *jobs, bool *did_quit, FILE **fds);
  // virtual void prepare();
  // virtual void cleanup();
  //  TODO: Add your extra methods if needed
};

class TimeoutCommand : public Command
{
  JobsList *jobs_;
  std::vector<std::string> cmd_words_;
  std::string inner_cmd_line_;
  int waiting_time_;
  Command *inner_cmd_ = nullptr;

public:

  TimeoutCommand(std::vector<std::string> cmd_words, std::string &cmd_line, JobsList *jobs, bool is_foreground_command);
  ~TimeoutCommand();
  void execute() override;
};

typedef struct {
  int job_id_;
  int delta_;
  std::string cmd_line_;
} TimeoutData;

class TimeStamp
{
  std::list<TimeoutData> list_;
  TimeStamp() {}
public:
  TimeStamp(TimeStamp const &) = delete;       // disable copy ctor
  void operator=(TimeStamp const &) = delete; // disable = operator
  static TimeStamp &getInstance()             // make JobsList singleton
  {
    static TimeStamp instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  bool empty();
  TimeoutData* getFirst();
  void insertTimeoutCommand(int job_id, int time, std::string cmd_line_);
  void pop();
};

class BuiltInCommand : public Command
{
protected:
  std::vector<std::string> cmd_words_;

public:
  BuiltInCommand(std::vector<std::string> cmd_words, std::string &cmd_line);
  std::vector<std::string> getBuiltInCommandWords();
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command
{
  JobsList *jobs_;
  FILE *fds_[2]; // using only if the external command is a part of a pipe command
public:
  FILE **getFds() override;
  ExternalCommand(std::string cmd_line, JobsList *jobs, FILE **fds);
  virtual ~ExternalCommand() {}
  void execute() override;
  bool isExternal() override;
};

class PipeCommand : public Command
{
  Command *first_cmd_;
  Command *second_cmd_;
  FILE *fds_[2];

  // TODO: Add your data members
public:
  FILE **getFds() override;
  PipeCommand(std::string cmd_line);
  virtual ~PipeCommand();
  void execute() override;
};

class RedirectionCommand : public Command
{
  Command *inner_cmd_;
  FILE *file_;
  // TODO: Add your data members
public:
  explicit RedirectionCommand(std::string cmd_line);
  virtual ~RedirectionCommand() { delete inner_cmd_; }
  void execute() override;
  // void prepare() override;
  // void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand
{
  std::string lastPwd_;
  // TODO: Add your data members public:
public:
  ChangeDirCommand(std::vector<std::string> cmd_words, std::string cmd_line);
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
  ShowPidCommand(std::vector<std::string> cmd_words, std::string cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand
{
  JobsList *jobs_;
  bool *did_quit_;

public:
  // TODO: Add your data members public:
  QuitCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs, bool *did_quit);
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

  public:
    JobEntry(int job_id, int process_id, std::string command_text_, bool is_stopped, time_t start_time_);
    int getProcessId();
    std::string getCommandText();
    std::string toString();
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
  int getLastNewPid();
  int getMaxJobId();
  ~JobsList();
  int getForegroundJobPid();
  JobEntry *getForegroundJob();
  void moveToForegound(int job_id);
  void removeForegroundJob();
  bool doesJobIdExist(int job_id);
  pid_t addJob(Command *cmd, bool isForeground, bool isStopped = false);
  void killAllJobsWithPrinting();
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
  JobsCommand(std::vector<std::string> cmd_words, std::string cmd_line, JobsList *jobs);
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
  TailCommand(std::vector<std::string> cmd_words, std::string cmd_line);
  virtual ~TailCommand() {}
  void execute() override;
};

class TouchCommand : public BuiltInCommand
{
public:
  TouchCommand(std::vector<std::string> cmd_words, std::string cmd_line);
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
  void executeCommand(std::string cmd_line, bool *did_quit);
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

class CommandNamesToNumbers
{
private:
  std::map<std::string, int> commands_map_;
  CommandNamesToNumbers();

public:
  CommandNamesToNumbers(CommandNamesToNumbers const &) = delete; // disable copy ctor
  void operator=(CommandNamesToNumbers const &) = delete;        // disable = operator
  static CommandNamesToNumbers &getInstance()                    // make CommandNamesToNumbers singleton
  {
    static CommandNamesToNumbers instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~CommandNamesToNumbers() = default;
  int getCommandNumber(const std::string &cmd_name);
};

class ChPromptCommand : public BuiltInCommand
{
public:
  ChPromptCommand(std::vector<std::string> cmd_words, std::string &cmd_line);
  virtual ~ChPromptCommand() = default;
  void execute() override;
};

class ShowPwdCommand : public BuiltInCommand
{
  // TODO: Add your data members public:
public:
  ShowPwdCommand(std::vector<std::string> cmd_words, std::string &cmd_line);
  virtual ~ShowPwdCommand() {}
  void execute() override;
};

std::string _ltrim(const std::string &s);

#endif // SMASH_COMMAND_H_
