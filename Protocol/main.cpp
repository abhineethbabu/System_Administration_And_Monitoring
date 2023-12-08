#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <queue>

using namespace std;

class Log
{
private:
    string message;
    string type;
    int priority;
    long long timestamp;

public:
    Log(){
        this->message = "";
        this->type = "";
        this->priority = -1;
        this->timestamp = 0;
    }
    Log(string &message, string &type, long long timestamp, int priority = 0)
    {
        this->type = type;
        this->timestamp = timestamp;
        this->priority = priority;
        this->message = message;
    }
    string getMessage(){
        return this->message;
    }
    string getType(){
        return this->type;
    }
    int getpriority(){
        return this->priority;
    }
    long long getTimeStamp(){
        return this->timestamp;
    }
};

class Command{
private:
    string command;
    bool isSudoNeeded;
    string type;
    long long timestamp;
public:
    Command(string &command,string type="",long long timestamp = 0,bool isSudoNeeded = false){
        this->command = command;
        this->type = type;
        this->timestamp = timestamp;
        this->isSudoNeeded = false;
    }
    string getCommand(){
        return this->command;
    }
    string getType(){
        return this->type;
    }
    bool isSudo(){
        return this->isSudoNeeded;
    }
    long long getTimeStamp(){
        return this->timestamp;
    }
    void setCommand(string &command){
        this->command = command;
    }
    void setType(string &type){
        this->type = type;
    }
    void setSudo(bool isSudoNeeded){
        this->isSudoNeeded = isSudoNeeded;
    }
};

class Executor
{
private:
public:
    string execute(Command &command)
    {
        string result;
        FILE *pipe = popen(command.getCommand().c_str(),"r");
        if(!pipe){
            //there was a problem creating another process
        }
        try{
            char buffer[128];
            while(fgets(buffer,sizeof(buffer),pipe)!=nullptr){
                result+=buffer;
            }
        }catch(string error){
            pclose(pipe);
            cout<<error;
        }
        pclose(pipe);
        return result;
    }
};

class Logger{
private:
    queue<Log> q;
public:
    Logger(){

    }
    Log getNext(){
        if(q.empty()){
            return Log();
        }
        Log log = q.front();
        q.pop();
        return log;
    }
    bool hasNext(){
        return q.empty()==false;
    }
    void pushLog(Log l){
        q.push(l);
    }
};



class Controller
{
private:
    string MAC_ADDRESS;
    string IP_ADDRESS;
    string PORT;
    int ping;
    long long lastCommunicationTime;
    Executor executor;
    void connect(){

    }

public:
    Controller(){
        //initialize
    }
    
    void start(){
        //this will run indefinitely
        try{
            while(true){
                //Check if logger has any info
                cout<<"Checking info from logger...."<<endl;
                //if logger has logs
                //sleep for 5 seconds before sending logs again
                sleep(5);
            }
        }catch(int err){
            cout<<err<<endl;
        }
    }
};


int main()
{
    string commandName = "pwd";
    Command command(commandName,"Basic",0,false);
    Executor executor;
    string result = executor.execute(command);
    cout<<result;
    //create a Controller
    //Controller controller;
    //controller.start();

    // Server Side
    /*
    Server Starts broadcasting its ip
    accepts connection from multiple clients
    accept requests from multiple connected clients
    polls each client for logs
    */
    
    //Client Side
    /*
    Client Starts
    Searches for the server through multicast protocol
    gets the connection information about the server
    tries to establish a connection
    enters an event loop and collects logs
    whenever server sends command to execute then send 
    */
    return 0;
}