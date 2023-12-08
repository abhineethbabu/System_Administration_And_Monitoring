#include <bits/stdc++.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
const int PORT = 4545;
const int BUFFER_SIZE = 1024;

void handleClient(int clientSocket, const sockaddr_in &clientAddress)
{
  char buffer[BUFFER_SIZE];

  while (true)
  {
    memset(buffer, 0, BUFFER_SIZE);

    ssize_t bytesRead = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
    if (bytesRead <= 0)
    {
      cerr << "Error receiving data from client." << endl;
      break;
    }

    cout << "Received from " << inet_ntoa(clientAddress.sin_addr) << ": " << buffer << endl;

    if (strcmp(buffer, "close") == 0)
    {
      cout << "Client " << inet_ntoa(clientAddress.sin_addr) << " has closed the connection." << endl;
      break;
    }
  }

  close(clientSocket);
}

int main()
{

  int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if (serverSocket == -1)
  {
    cerr << "Error creating socket." << endl;
    return -1;
  }

  // Bind socket to a port
  sockaddr_in serverAddress{};
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);

  if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
  {
    cerr << "Error binding socket to port." << endl;
    close(serverSocket);
    return -1;
  }

  cout << "Server is listening on port " << PORT << endl;

  while (true)
  {

    sockaddr_in clientAddress{};
    socklen_t clientAddrLen = sizeof(clientAddress);
    char buffer[BUFFER_SIZE];

    ssize_t bytesRead = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddress, &clientAddrLen);
    if (bytesRead == -1)
    {
      cerr << "Error receiving data from client." << endl;
      continue;
    }

    thread(handleClient, serverSocket, clientAddress).detach();
  }
  close(serverSocket);

  return 0;
}
