#include <iostream>
#include <string>
#include <cstdio>
#include <utility>

using namespace std;

class Command
{
public:
  Command(const string &cmd, bool useSudo = false) : command(cmd), sudo(useSudo) {}

  string getCommand() const
  {
    if (sudo)
    {
#ifdef _WIN32
      return "runas /user:Administrator " + command;
#else
      return "sudo " + command;
#endif
    }
    else
    {
      return command;
    }
  }

private:
  string command;
  bool sudo;
};

class Executer
{
public:
  pair<bool, string> runCommand(Command &command)
  {
    string result;
    bool success = false;

#ifdef _WIN32
    FILE *pipe = _popen(command.getCommand().c_str(), "r");
#else
    FILE *pipe = popen(command.getCommand().c_str(), "r");
#endif

    if (pipe)
    {
      success = true;
      try
      {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
          result += buffer;
        }
      }
      catch (const string &error)
      {
        success = false;
        cout << error;
      }

#ifdef _WIN32
      _pclose(pipe);
#else
      pclose(pipe);
#endif
    }

    return make_pair(success, result);
  }
};

int main()
{

  Executer exe;

  Command command("echo Hello, World!", true);
  pair<bool, string> result = exe.runCommand(command);

  if (result.first)
  {
    cout << "Command executed successfully. Output:\n"
         << result.second << endl;
  }
  else
  {
    cout << "Command execution failed." << endl;
  }

  return 0;
}
