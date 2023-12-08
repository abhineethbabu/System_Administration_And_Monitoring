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
        destaddr.sin_addr.s_addr = inet_addr("192.168.148.255");
        while (isAlive)
        {
            res = sendto(sockfd, message.c_str(), message.length(), 0, reinterpret_cast<sockaddr *>(&destaddr), sizeof(destaddr));
            if (res < 0)
            {
                cout << "Error broadcasting..." << endl;
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
        in_addr_t ip = inet_addr("192.168.148.1");
        in_addr_t subnet = inet_addr("255.255.255.0");
        in_addr_t network = ip & subnet;
        in_addr_t invertedSubnet = ~subnet;
        in_addr_t broadcast = network | invertedSubnet;
        struct in_addr broadcastAddress;
        broadcastAddress.s_addr = broadcast;
        destaddr.sin_addr.s_addr = inet_addr(inet_ntoa(broadcastAddress));
        while (i)
        {
            res = sendto(sockfd, message.c_str(), message.length(), 0, reinterpret_cast<sockaddr *>(&destaddr), sizeof(destaddr));
            if (res < 0)
            {
                cout << "Error broadcasting..." << endl;
            }
            else
            {
                //if the message is sent
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

// Utilities for system functions
class SystemUtils
{
private:
    string hostOS;

public:
    SystemUtils()
    {
        // get base system details
        hostOS = getHostOS();
    }
    string getHostOS()
    {
#ifdef _WIN32
        return "Windows-x86";
#elif _WIN64
        return "Windows-x64";
#elif __APPLE__ || __MACH__
        return "Mac OSX";
#elif __linux__
        return "Linux";
#elif __unix || __unix__
        return "Unix";
#else
        return "Unknown OS";
#endif
    }
    bool isOS(string name)
    {
        if (SystemUtils::hostOS == name)
            return true;
    }
};

class Server
{
private:
    string os;
    int serverType;
    int port;
    string intToString(int num){
        string result;
        int temp = num;
        while(temp){
            result+='0'+temp%10;
            temp/=10;
        }
        int i,j;
        char t;
        for(i =0,j = result.length()-1;i<j;i++,j--){
            t = result[i];
            result[i] = result[j];
            result[j] = t;
        }
        return result;
    }
    string generatePacket(string serviceName,string ip,int port){
        string res;
        res = serviceName;
        string portStr = intToString(port);
        res += ':';
        res += portStr;
        return res;
    }
    string getIP(){
        string ip;
        //code here to get ip
        return ip;
    }
    void acceptTCPConnections()
    {
    }
    void acceptUDPConnections()
    {
        int PORT = port;
        int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (serverSocket == -1)
        {
            std::cerr << "Failed to create socket." << std::endl;
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(PORT);

        if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) == -1)
        {
            std::cerr << "Failed to bind socket." << std::endl;
            close(serverSocket);
            return;
        }

        std::cout << "Server listening on port " << PORT << std::endl;
        char buffer[BUFFER_SIZE];
        while (true)
        {
            sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            ssize_t bytesRead = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
            if(bytesRead>0){
                std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
                cout<<buffer<<endl;
                
            }
        }
        close(serverSocket);
    }

public:
    Server(int type)
    {
        this->serverType = type;
        SystemUtils su;
        this->os = su.getHostOS();
        cout << "Server running on " << this->os << endl;
    }
    void listen(int port)
    {
        // broadcasting started and will go on til the program is executing
        string serviceName = "RemoteService";
        string broadcastString = generatePacket(serviceName,getIP(),port);
        
        Broadcaster b(UDP, 4545);
        b.setFrequency(10000);
        b.broadcast(serviceName, broadcastString);
        // start the server
        this->port = port;
        if (serverType == TCP)
            acceptTCPConnections();
        else
            acceptUDPConnections();
        //stop the broadcast as server has stopped
        b.stopBroadcast();
    } 
};

int main()
{
    int port = 4550;
    Server server(UDP);
    server.listen(port);
    return 0;
}