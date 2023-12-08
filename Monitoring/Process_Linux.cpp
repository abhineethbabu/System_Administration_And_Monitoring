#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>

using namespace std;

class Process
{
public:
  string pid;
  string name;
  string flag;

  Process() {}

  Process(const string &pid, const string &name)
      : pid(pid), name(name), flag(categorize(name)) {}

private:
  static string categorize(const string &str)
  {
    if (str == "chrome" || str == "firefox")
      return "Web_Browser";
    else if (str == "msedge")
      return "edge";
    else if (str == "bash" || str == "gnome-terminal-server")
      return "Terminal";
    else if (str == "code")
      return "Code_Editor";

    return "Other";
  }
};

class Logger
{
public:
  string getTimeStamp()
  {
    time_t now = time(0);
    return to_string(now);
  }

  pair<string, unordered_map<string, Process>> listProcesses()
  {
    string currentTime = getTimeStamp();
    unordered_map<string, Process> processMap;

    ifstream procDir("/proc");
    if (!procDir)
    {
      cerr << "Error opening /proc directory" << endl;
      return {currentTime, processMap};
    }

    string pid;
    while (procDir >> pid)
    {
      if (pid.find_first_not_of("0123456789") != string::npos)
      {

        continue;
      }

      ifstream statusFile("/proc/" + pid + "/status");
      if (statusFile)
      {
        string line;
        string processName;
        while (getline(statusFile, line))
        {
          istringstream iss(line);
          string key, value;
          iss >> key >> value;
          if (key == "Name:")
          {
            processName = value;
            break;
          }
        }

        Process process(pid, processName);
        processMap[pid] = process;
      }
      else
      {
        cerr << "Error opening /proc/" + pid + "/status file" << endl;
      }
    }

    return {currentTime, processMap};
  }
};

int main()
{

  Logger logger;
  pair<string, unordered_map<string, Process>> result = logger.listProcesses();

  cout << "Time: " << result.first << endl;
  for (const auto &entry : result.second)
  {
    const Process &process = entry.second;
    cout << "PID: " << process.pid << ", Name: " << process.name << ", Flag: " << process.flag << endl;
  }

  return 0;
}
