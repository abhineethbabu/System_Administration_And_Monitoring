#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fstream>

using namespace std;
#define PORT 4545
#define SA sockaddr

int sockfd;

struct msg
{
  char filename[100];
  char file[2048];
};

void sig_handler(int signo)
{

  if (signo == SIGINT)
  {
    cout << "\t Exiting..." << '\n';
    close(sockfd);
    exit(1);
  }
}

void connection(int sockfd)
{

  msg buffer;

  signal(SIGINT, sig_handler);
  signal(SIGTSTP, sig_handler);

  while (1)
  {
    memset(buffer.filename, 0, sizeof(buffer.filename));
    memset(buffer.file, '\0', sizeof(buffer.file));
    cout << "\n\t Enter the filename: ";
    cin >> buffer.filename;

    ifstream file;
    file.open(buffer.filename);
    file.seekg(0);
    char c;
    int i = 0;

    while (!file.eof())
    {
      file.get(c);
      buffer.file[i] = c;
      i++;
    }

    write(sockfd, &buffer, sizeof(buffer));

    cout << "\n\t File read and sent to the server..." << '\n';

    cout << "\n\t file: " << '\n';
    cout << buffer.file << '\n';
  }
}

int main(int argc, char const *argv[])
{

  sockaddr_in servaddr, cli;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("socket creation failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully created..\n");

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  // servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
  servaddr.sin_port = htons(PORT);

  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
  {
    printf("connection with the server failed...\n");
    exit(0);
  }
  else
    printf("connected to the server..\n");

  connection(sockfd);
}