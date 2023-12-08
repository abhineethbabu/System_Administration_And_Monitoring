#include <iostream>
#include <string>

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

int main()
{
  string os = getOperatingSystem();
  cout << "Operating System: " << os << endl;

  return 0;
}
