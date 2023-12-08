#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
const int PORT = 4545;
const int BUFFER_SIZE = 1024;

int main()
{
  
  int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if (clientSocket == -1)
  {
    cerr << "Error creating socket." << endl;
    return -1;
  }

  sockaddr_in serverAddress{};
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

  while (true)
  {
    cout << "Enter message : ";
    string message;
    getline(cin, message);

    sendto(clientSocket, message.c_str(), message.size(), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (message == "close")
    {
      break;
    }
  }

  close(clientSocket);

  return 0;
}
