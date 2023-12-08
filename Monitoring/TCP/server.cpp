#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;
const int PORT = 4545;
const int BUFFER_SIZE = 1024;

int main()
{

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1)
  {
    cerr << "Error creating socket.\n";
    return -1;
  }

  sockaddr_in serverAddress;
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);

  if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
  {
    cerr << "Error binding socket.\n";
    close(serverSocket);
    return -1;
  }

  if (listen(serverSocket, 5) == -1)
  {
    cerr << "Error listening for connections.\n";
    close(serverSocket);
    return -1;
  }

  cout << "Server listening on port " << PORT << "...\n";

  sockaddr_in clientAddress;
  socklen_t clientAddressLength = sizeof(clientAddress);
  int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
  if (clientSocket == -1)
  {
    cerr << "Error accepting connection.\n";
    close(serverSocket);
    return -1;
  }

  cout << "Connection accepted from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "\n";

  char buffer[BUFFER_SIZE];
  while (true)
  {
    memset(buffer, 0, sizeof(buffer));
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0)
    {
      cerr << "Connection closed by client.\n";
      break;
    }

    cout << "Received message from client: " << buffer << "\n";
  }

  close(clientSocket);
  close(serverSocket);

  return 0;
}
