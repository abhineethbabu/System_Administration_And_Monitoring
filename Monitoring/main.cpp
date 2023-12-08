#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <utility>
#include <windows.h>
#include <tlhelp32.h>
#include <ctime>
#include <thread>
#include <chrono>
using namespace std;

#ifdef _WIN32
const string OS_NAME = "Windows";
#elif __linux__
const string OS_NAME = "Linux";
#else
const string OS_NAME = "Unknown";
#endif

string getOperatingSystem()
{
  return OS_NAME;
}

//---------------------------------------LOG---------------------------------------------------------
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
    if (str == "firefox.exe")
      return "Web_Browser";
    else if (str == "msedge.exe" || str == "chrome.exe")
      return "edge";
    else if (str == "cmd.exe" || str == "WindowsTerminal.exe")
      return "Terminal";
    else if (str == "Code.exe")
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
    // string currentTime = __TIME__;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
      cerr << "Error creating snapshot: " << GetLastError() << endl;
      return {currentTime, {}};
    }
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    unordered_map<string, Process> processMap;
    if (Process32First(hSnapshot, &pe32))
    {
      do
      {
        string pidString = to_string(pe32.th32ProcessID);
        string processName = pe32.szExeFile;
        Process process(pidString, processName);
        processMap[pidString] = process;

      } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return {currentTime, processMap};
  }
};

//---------------------------------------Executor---------------------------------------------------------
class Command
{
private:
  string command;
  bool sudo;

public:
  Command(const string &cmd, bool useSudo = false) : command(cmd), sudo(useSudo) {}

  string getCommand() const
  {
    string OS = getOperatingSystem();
    if (sudo)
    {
      if (OS == "Windows")
        return "runas  C:/Users/Abhi/Desktop/Major Project/C++ " + command;
      else
        return "sudo " + command;
    }
    else
    {
      return command;
    }
  }
};

class Executor
{

public:
  bool killProcess(const string &PID)
  {
    DWORD pid = stoul(PID, nullptr, 10);
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
    {
      cerr << "Error opening process with PID " << PID << ": " << GetLastError() << endl;
      return false;
    }

    if (!TerminateProcess(hProcess, 0))
    {
      cerr << "Error terminating process with PID " << PID << ": " << GetLastError() << endl;
      CloseHandle(hProcess);
      return false;
    }

    CloseHandle(hProcess);

    cout << "\tSuccessfully terminated process with PID " << PID << endl;
    return true;
  }

  pair<bool, string> runCommand(Command &command)
  {
    string result;
    bool success = false;
    string OS = getOperatingSystem();
    FILE *pipe;

    if (OS == "Windows")
      pipe = _popen(command.getCommand().c_str(), "r");
    else
      pipe = popen(command.getCommand().c_str(), "r");

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

      if (OS == "Windows")
        _pclose(pipe);
      else
        pclose(pipe);
    }

    return make_pair(success, result);
  }
};

//---------------------------------------Controller---------------------------------------------------------
class controller
{
private:
public:
};

int main()
{
  // string ans;
  // executor exe;
  // ans = log.getMAC();
  // cout << ans << endl;
  // bool it = log.CheckWebBrowser();
  // log.Status();
  // string out = exe.runCommand("ls");
  // cout << out << endl;
  Logger log;
  Executor exe;
  while (true)
  {
    auto result = log.listProcesses();

    // cout << "Time: " << result.first << endl;
    // cout << "Processes:" << endl;
    vector<string> kill;
    for (const auto &entry : result.second)
    {
      const Process &process = entry.second;
      // cout << "PID: " << process.pid << " | Process Name: " << process.name << " | Category: " << process.flag << endl;
      if (process.flag == "edge")
      {
        kill.push_back(process.pid);
      }
    }

    if (kill.size() > 0)
      cout << "\n\t Browser Detected !! {No of Broser Modules=> " << kill.size() << "} \n\t ** Initiating to KILL PROCESS **\n\n";

    int i = 1;
    for (auto x : kill)
    {
      if (exe.killProcess(x))
        cout << "Process Killed {module: " << i << "}" << endl;
      else
        cout << "Failed To KILL {module: " << i << "}" << endl;
      i++;
    }
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    int t = 100;
    for (long long i = 0; i < 1000000000; i++)
    {
      t += 100;
      t -= 100;
    }
    // sleep(10);
  }

  // auto result = log.listProcesses();

  // cout << "Time: " << result.first << endl;
  // cout << "Processes:" << endl;
  // vector<string> kill;
  // for (const auto &entry : result.second)
  // {
  //   const Process &process = entry.second;
  //   cout << "PID: " << process.pid << " | Process Name: " << process.name << " | Category: " << process.flag << endl;
  //   if (process.flag == "edge")
  //     kill.push_back(process.pid);
  // }

  // int i = 1;
  // for (auto x : kill)
  // {
  //   if (exe.killProcess(x))
  //     cout << "True " << i << endl;
  //   else
  //     cout << "False " << i << "  [PID]: " << x << endl;
  //   i++;
  // }

  // Command command("dir", false);
  // pair<bool, string> result = exe.runCommand(command);

  // if (result.first)
  // {
  //   cout << "Command executed successfully. Output:\n"
  //        << result.second << endl;
  // }
  // else
  // {
  //   cout << "Command execution failed." << endl;
  // }
  return 0;
}
