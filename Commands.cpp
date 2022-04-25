#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
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
  Command *cmd = Command::createCommand(cmd_line);
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

Command::Command(std::vector<std::string> cmd_params) : params_(cmd_params) {}

BuiltInCommand::BuiltInCommand(std::vector<std::string> cmd_params) : Command(cmd_params) {}

ChPromptCommand::ChPromptCommand(std::vector<std::string> cmd_params) : BuiltInCommand(cmd_params) {}

void ChPromptCommand::execute()
{
  Prompt &prompt = Prompt::getInstance();
  if (params_.size() == 0)
  {
    prompt.setPrompt();
    return;
  }
  prompt.setPrompt(params_[0]);
}

ShowPwdCommand::ShowPwdCommand(std::vector<std::string> cmd_params) : BuiltInCommand(cmd_params) {}

void ShowPwdCommand::execute()
{
  if (params_.size() != 0)
  {
    return;
  }
  char path[PATH_MAX_LENGTH];
  char *result = getcwd(path, PATH_MAX_LENGTH);
  if (result != nullptr)
  {
    std::cout << path << "\n";
  }
}

BuiltInCommandNamesMap::BuiltInCommandNamesMap()
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
    map_[cmd_names[i]] = i + 1;
  }
}

int BuiltInCommandNamesMap::getCommandNumber(const std::string &cmd_name)
{
  std::map<std::string, int>::iterator cmd_iterator = map_.find(cmd_name);
  if (cmd_iterator == map_.end())
  {
    return 0;
  }
  return cmd_iterator->second;
}

std::vector<std::string> split(std::string str)
{
  std::vector<std::string> vec;
  str = _trim(str);
  size_t index;
  while (str.size() > 0)
  {
    index = str.find(' ');
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

Command *Command::createCommand(std::string cmd_line)
{
  cmd_line = _rtrim(cmd_line);
  //bool is_foreground_command = true;
  if (cmd_line[cmd_line.size() - 1] == '&')
  {
    //is_foreground_command = false;
    cmd_line.substr(0, cmd_line.size() - 1); // removes the '&' from cmd_line
  }
  std::vector<std::string> cmd_params = split(cmd_line);
  std::string command_type = cmd_params[0];
  cmd_params.erase(cmd_params.begin());
  BuiltInCommandNamesMap &built_in_map = BuiltInCommandNamesMap::getInstance();
  Command *cmd_object_ptr = nullptr;
  switch (built_in_map.getCommandNumber(command_type))
  {
  case 1:
    cmd_object_ptr = new ChPromptCommand(cmd_params);
    return cmd_object_ptr;
  /*case 2:
    cmd_object_ptr = new ShowPidCommand(cmd_params);
    return cmd_object_ptr;*/
  case 3:
    cmd_object_ptr = new ShowPwdCommand(cmd_params);
    return cmd_object_ptr;
  case 4:
    cmd_object_ptr = new ChangeDirCommand(cmd_params);
    return cmd_object_ptr;
  /*case 5:
    cmd_object_ptr = new JobsCommand(cmd_params);
    return cmd_object_ptr;
  case 6:
    cmd_object_ptr = new KillCommand(cmd_params);
    return cmd_object_ptr;
  case 7:
    cmd_object_ptr = new ForegroundCommand(cmd_params);
    return cmd_object_ptr;
  case 8:
    cmd_object_ptr = new BackgroundCommand(cmd_params);
    return cmd_object_ptr;
  case 9:
    cmd_object_ptr = new QuitCommand(cmd_params);
    return cmd_object_ptr;
    */
  default:
    // the command is not a built in command
    break;
  }
  return cmd_object_ptr;
}

ChangeDirCommand::ChangeDirCommand(std::vector<std::string> cmd_params) : BuiltInCommand(cmd_params) {}

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
  if (params_.size() > 1)
  {
    std::cerr << "smash error: cd: too many arguments\n";
    return;
  }
  if (params_.size() == 0)
  {
    std::cerr << "smash error: cd: need an argument\n";
    return;
  }
  std::string current_path = getCurrentPath();
  std::cout << "OPHIR: " << current_path << "\n";
  LastPath &last_path = LastPath::getInstance();
  if (params_[0] == "-")
  {
    chdir(last_path.getLastPath().c_str());
  }
  else
  {
    chdir(params_[0].c_str());
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
