#include <bits/stdc++.h>
#include <thread>
#include <atomic>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
#define UDP 1
#define TCP 2
#define BUFFER_SIZE 1024

class Discoverer
{
private:
    int type;

public:
    Discoverer()
    {
        this->type = UDP;
    }
    Discoverer(int type)
    {
        this->type = type;
    }
    pair<string, string> discover(string &serviceName, int port, int timeout)
    {
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            cout << "Error creating discovery socket" << endl;
            return {"", ""};
        }
        int broadcastEnable = 1;
        int res = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        if (res < 0)
        {
            cout << "Error setting to discovery mode" << endl;
            close(sockfd);
            return {"", ""};
        }
        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        res = bind(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr));
        if (res < 0)
        {
            cout << "Error binding the socket" << endl;
            close(sockfd);
            return {"", ""};
        }
        char buffer[1024];
        for(int i =0;i<BUFFER_SIZE;i++)buffer[i] = 0;
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        string ip;
        while (true)
        {
            int bytesRead = recvfrom(sockfd, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);
            if (bytesRead < 0)
            {
                std::cerr << "Error receiving data" << std::endl;
                break;
            }
            buffer[bytesRead] = '\0';
            // std::cout << "Received broadcast message from " << inet_ntoa(clientAddr.sin_addr) << ": " << buffer << std::endl;
            ip = inet_ntoa(clientAddr.sin_addr);
            break;
        }
        close(sockfd);
        return {ip, string(buffer)};
    }
};

class Client
{
private:
    int connectionType;
    int port;
    void connectToServerViaUDP(string ip, int serverPort)
    {
        int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSocket == -1)
        {
            std::cerr << "Failed to create socket." << std::endl;
            return connect(this->port);
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
        {
            std::cerr << "Invalid server address." << std::endl;
            close(clientSocket);
            return connect(this->port);
        }

        char message[] = "Service Subscriber 1 connected";
        if (sendto(clientSocket, message, strlen(message), 0, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1)
        {
            std::cerr << "Failed to send message." << std::endl;
            close(clientSocket);
            return connect(this->port);
        }
        std::cout << "Service Detected" << std::endl;
        close(clientSocket);
    }
    void connectToServerViaTCP(string ip, int serverPort)
    {
    }
    int parsePort(string &message)
    {
        int len = message.length();
        int i;
        for (i = 0; i < len; i++)
        {
            if (message[i] == ':')
                break;
        }
        i++;
        if (i >= len)
            return -1;
        int serverPort = 0;
        for (; i < len; i++)
        {
            serverPort = serverPort * 10 + (message[i] - '0');
        }
        return serverPort;
    }
    string parseServiceName(string &message)
    {
        string serviceName;
        for (int i = 0; i < message.length(); i++)
        {
            if (message[i] == ':')
                break;
            serviceName += message[i];
        }
        return serviceName;
    }

public:
    Client(int type)
    {
        // default Constructor
        this->connectionType = type;
        cout << "Client Started" << endl;
    }
    void connect(int port)
    {
        this->port = port;
        // search for the server
        Discoverer discovrer;
        string serviceName = "RemoteService";
        pair<string, string> result = discovrer.discover(serviceName, port, 10000);
        // establish connection
        string ip = result.first;
        int serverPort = parsePort(result.second);
        string serviceNameRecv = parseServiceName(result.second);
        if (serviceNameRecv != serviceName)
        {
            // different service is running on the same port
            cout << serviceNameRecv << endl;
            return connect(port);
        }
        cout << serviceName << " service running on IP is : " << ip << ", port : " << serverPort << endl;
        if (connectionType == UDP)
        {
            connectToServerViaUDP(ip, serverPort);
        }
        else
        {
            connectToServerViaTCP(ip, serverPort);
        }
    }
};
int main()
{
    int port = 4545;
    Client client(UDP);
    client.connect(port);
    return 0;
}