#include <bits/stdc++.h>
#include <thread>
#include <atomic>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

#define UDP 1
#define TCP 2

class Broadcaster
{
private:
    string serviceName;
    string message;
    int type;
    int freq;
    unsigned short int port;
    int sockfd;
    int *isAlive;
    void startUDPBroadcast(int port, int *isAlive, string &message, string &serviceName)
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            cout << "Error creating socket";
            return;
        }

        int broadcastEnabled = 1;
        int res = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnabled, sizeof(broadcastEnabled));
        if (res < 0)
        {
            cout << "Unable to set broadcast options" << endl;
            close(sockfd);
            return;
        }
        sockaddr_in destaddr;
        memset(&destaddr, 0, sizeof(destaddr));
        destaddr.sin_family = AF_INET;
        destaddr.sin_port = htons(port);
        destaddr.sin_addr.s_addr = inet_addr("255.255.255.255");
        while (isAlive)
        {
            res = sendto(sockfd, message.c_str(), message.length(), 0, reinterpret_cast<sockaddr *>(&destaddr), sizeof(destaddr));
            if (res < 0)
            {
                cout << "Error broadcasting..." << endl;
            }
            else
            {
                cout << "message sent" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(freq));
        }
        close(sockfd);
    }

    void startTCPBroadcast()
    {
    }

public:
    Broadcaster()
    {
        this->type = 1;
        this->port = 4545;
    }
    Broadcaster(int type, int port)
    {
        this->type = type;
        this->port = port;
    }
    void setFrequency(int freq)
    {
        this->freq = freq;
    }
    thread *t;
    string broadcast(string &serviceName, string &message)
    {
        // start broadcast
        this->serviceName = serviceName;
        this->message = message;
        this->isAlive = new int;
        int *i = isAlive;
        *isAlive = 1;
        int freq = this->freq;
        int port = this->port;
        if (type == UDP)
        {
            thread *t = new thread([i, message, freq, port]()
                                   {
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            cout << "Error creating socket";
            return;
        }

        int broadcastEnabled = 1;
        int res = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnabled, sizeof(broadcastEnabled));
        if (res < 0)
        {
            cout << "Unable to set broadcast options" << endl;
            close(sockfd);
            return;
        }
        sockaddr_in destaddr;
        memset(&destaddr, 0, sizeof(destaddr));
        destaddr.sin_family = AF_INET;
        destaddr.sin_port = htons(port);
        destaddr.sin_addr.s_addr = inet_addr("172.22.11.25");
        while (i)
        {
            res = sendto(sockfd, message.c_str(), message.length(), 0, reinterpret_cast<sockaddr *>(&destaddr), sizeof(destaddr));
            if (res < 0)
            {
                cout << "Error broadcasting..." << endl;
            }
            else
            {
                cout << "message sent" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(freq));
        }
        close(sockfd); });
        }
        else
        {
            startTCPBroadcast();
        }
        return "";
    }

    void stopBroadcast()
    {
        // stopping the broadcast
        *(this->isAlive) = 0;
    }
};

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
    string discover(string &serviceName, int port, int timeout)
    {
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            cout << "Error creating discovery socket" << endl;
            return "";
        }
        int broadcastEnable = 1;
        int res = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        if (res < 0)
        {
            cout << "Error setting to discovery mode" << endl;
            close(sockfd);
            return "";
        }
        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port); // Replace with the desired port
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        res = bind(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr));
        if (res < 0)
        {
            cout << "Error binding the socket" << endl;
            close(sockfd);
            return "";
        }
        char buffer[1024];
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        while (true)
        {
            int bytesRead = recvfrom(sockfd, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);
            if (bytesRead < 0)
            {
                std::cerr << "Error receiving data" << std::endl;
                break;
            }
            buffer[bytesRead] = '\0';
            std::cout << "Received broadcast message from " << inet_ntoa(clientAddr.sin_addr) << ": " << buffer << std::endl;
        }
        close(sockfd);
        return string(buffer);
    }
};

int main()
{
    string serviceName = "TestService";
    // string message = "This is a test service running on 4545 port";
    // Broadcaster broadcaster;
    // broadcaster.setFrequency(1000);
    // broadcaster.broadcast(serviceName, message);
    // this_thread::sleep_for(chrono::milliseconds(100000));
    // broadcaster.stopBroadcast();
    cout << "Discoverer Starts" << endl;
    Discoverer discoverer;
    cout << discoverer.discover(serviceName, 4545, 10000) << endl;
    return 0;
}