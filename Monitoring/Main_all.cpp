#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <utility>
#include <windows.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <ctime>
// #pragma comment(lib, "iphlpapi.lib")

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
        if (str == "chrome.exe" || str == "firefox.exe")
            return "Web_Browser";
        else if (str == "msedge.exe")
            return "edge";
        else if (str == "cmd.exe" || str == "WindowsTerminal.exe")
            return "Terminal";
        else if (str == "Code.exe")
            return "Code_Editor";

        return "Other";
    }
};

//---------------------------------------LOG---------------------------------------------------------
class Logger
{
private:
    // vector<int> log;
    // int interval = 1000;

public:
    string getMAC() // To get MAC address in Windows OS
    {
        IP_ADAPTER_INFO *AdapterInfo;
        DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
        AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
        if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
        {
            AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
            if (AdapterInfo == NULL)
            {
                cout << "ERROR!";
                return "";
            }
        }
        if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
        {
            PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
            do
            {
                char macAddress[18];
                sprintf(macAddress, "%02x:%02x:%02x:%02x:%02x:%02x",
                        pAdapterInfo->Address[0],
                        pAdapterInfo->Address[1],
                        pAdapterInfo->Address[2],
                        pAdapterInfo->Address[3],
                        pAdapterInfo->Address[4],
                        pAdapterInfo->Address[5]);
                string MACaddressString(macAddress);
                free(AdapterInfo);
                return MACaddressString;
            } while (pAdapterInfo != NULL);
        }
        free(AdapterInfo);
        return "";
    }

    bool CheckWebBrowser()
    {
        vector<string> window_classes = {
            "Chrome_WidgetWin_1",
            "MozillaWindowClass",
            "Windows.UI.Core.CoreWindow"};
        for (const string &window_class : window_classes)
        {
            if (FindWindow(window_class.c_str(), NULL) != NULL)
            {
                cout << "\nyes\n";
                return true;
            }
        }
        cout << "\nno\n";
        return false;
    }

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

    // Another method to get active PID
    void Status() // To get the active pid
    {
        DWORD aProcesses[1024], cbNeeded, cProcesses;
        if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        {
            return;
        }
        cProcesses = cbNeeded / sizeof(DWORD);
        cout << "Process IDs:" << endl;
        for (unsigned int i = 0; i < cProcesses; i++)
        {
            if (aProcesses[i] != 0)
            {
                cout << aProcesses[i] << endl;
            }
        }
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
private:
    // string command;

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

        cout << "Successfully terminated process with PID " << PID << endl;
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

    string runCommand(string command)
    {
        HANDLE hPipeRead, hPipeWrite;
        CreatePipe(&hPipeRead, &hPipeWrite, NULL, 0);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hPipeWrite;
        ZeroMemory(&pi, sizeof(pi));
        if (!CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        {
            CloseHandle(hPipeRead);
            CloseHandle(hPipeWrite);
            cout << "came";
            return "";
        }
        CloseHandle(hPipeWrite);
        char buffer[1024];
        DWORD dwRead;
        string output;
        while (ReadFile(hPipeRead, buffer, sizeof(buffer), &dwRead, NULL))
        {
            output.append(buffer, dwRead);
        }
        CloseHandle(hPipeRead);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        return output;
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
    auto result = log.listProcesses();

    cout << "Time: " << result.first << endl;
    cout << "Processes:" << endl;
    vector<string> kill;
    for (const auto &entry : result.second)
    {
        const Process &process = entry.second;
        cout << "PID: " << process.pid << " | Process Name: " << process.name << " | Category: " << process.flag << endl;
        if (process.flag == "edge")
            kill.push_back(process.pid);
    }

    Executor exe;
    int i = 1;
    for (auto x : kill)
    {
        if (exe.killProcess(x))
            cout << "True " << i << endl;
        else
            cout << "False " << i << "  [PID]: " << x << endl;
        i++;
    }

    Command command("dir", false);
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
