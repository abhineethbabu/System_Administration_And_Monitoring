#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <fstream>
#include <string>
#include <cstring>
#include <signal.h>

#define PORT 4545

pthread_t thread_tcp[100], thread_file[10];
int threadno_tcp = 0;
int mistfd;

struct msg
{
  char filename[100];
  char file[2048];
};

struct req
{
  int des;
  socklen_t addlen;
  sockaddr_in clientaddr;
};

void *tcp_connection(void *);

void sig_handler(int signo)
{

  if (signo == SIGINT)
  {
    std::cout << "\t Exiting..." << '\n';
    close(mistfd);
    exit(1);
  }
}

int main(int argc, char const *argv[])
{

  sockaddr_in mistaddr;
  sockaddr_in clientaddr;

  socklen_t addrlen = sizeof(clientaddr);

  if ((mistfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    std::cout << "\n\t Socket creation failed...\n\t Exiting..." << '\n';
    return 0;
  }

  std::cout << "\n\t Socket created..." << '\n';

  memset((sockaddr *)&mistaddr, 0, sizeof(mistaddr));
  mistaddr.sin_family = AF_INET;
  mistaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  mistaddr.sin_port = htons(PORT);

  if (bind(mistfd, (sockaddr *)&mistaddr, sizeof(mistaddr)) != 0)
  {
    std::cout << "\n\t Binding failed...\n\t Exiting..." << '\n';
    return 0;
  }

  std::cout << "\n\t Binding succesful..." << '\n';

  if ((listen(mistfd, 5)) != 0)
  {
    std::cout << "\n\t Server not listning..." << '\n';
    return 0;
  }

  signal(SIGINT, sig_handler);
  signal(SIGTSTP, sig_handler);

  std::cout << "\n\t Server listning..." << '\n';

  while (1)
  {

    int connfd;
    if ((connfd = accept(mistfd, (sockaddr *)&clientaddr, &addrlen)) < 0)
      std::cout << "\n\t Client connection declined..." << '\n';
    else
      std::cout << "\n\t Client connection accepted..." << '\n';

    req *r = new req;
    bzero(r, sizeof(req));
    r->addlen = addrlen;
    r->clientaddr = clientaddr;
    r->des = connfd;

    pthread_create(&thread_tcp[threadno_tcp++], NULL, tcp_connection, (void *)r);
    if (threadno_tcp == 100)
      threadno_tcp = 0;
  }
}

void *tcp_connection(void *arg)
{
  req sock = *((req *)arg);
  msg buffer;

  while (1)
  {
    read(sock.des, &buffer, sizeof(buffer));

    std::cout << "\n\t File name: " << buffer.filename;
    std::cout << "\n\n"
              << buffer.file << '\n';
  }
}