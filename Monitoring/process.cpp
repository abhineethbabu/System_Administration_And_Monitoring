#include <iostream>
#include <string>
#include <unordered_map>
#include <windows.h>
#include <tlhelp32.h>

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

        return "Other";
    }
};

class Logger
{
public:
    pair<string, unordered_map<string, Process>> listProcesses()
    {
        string currentTime = __TIME__;

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

int main()
{
    Logger log;
    auto result = log.listProcesses();

    cout << "Time: " << result.first << endl;
    cout << "Processes:" << endl;

    for (const auto &entry : result.second)
    {
        const Process &process = entry.second;
        cout << "PID: " << process.pid
             << " | Process Name: " << process.name
             << " | Category: " << process.flag << endl;
    }

    return 0;
}
