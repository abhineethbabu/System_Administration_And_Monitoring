#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;
const char *SERVER_IP = "127.0.0.1";
const int PORT = 4545;
const int BUFFER_SIZE = 1024;

int main()
{

  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1)
  {
    cerr << "Error creating socket.\n";
    return -1;
  }

  sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
  serverAddress.sin_port = htons(PORT);

  if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
  {
    cerr << "Error connecting to server.\n";
    close(clientSocket);
    return -1;
  }

  cout << "Connected to server.\n";

  char buffer[BUFFER_SIZE];
  while (true)
  {
    cout << "Enter a message (type 'exit' to close the connection): ";
    cin.getline(buffer, sizeof(buffer));

    if (strcmp(buffer, "exit") == 0)
    {
      break;
    }

    send(clientSocket, buffer, strlen(buffer), 0);
  }

  close(clientSocket);

  return 0;
}
