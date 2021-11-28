#pragma once
#include <ClassSetup.h>
#include <mutex>
#include <queue>
#include <condition_variable>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

extern bool isGameDone;

class Model;
class SendController;

class RecvController: public KeyPublisher{
  public:
    queue<char> keys;
    Model* Mobservers[2];
    SendController* Sobservers[2];
    int nMobservers = 0;
    int nSobservers = 0;
    int sock_client;
    bool isServerRecv;

    WINDOW *prwin;
    string pname;

    RecvController(string name, WINDOW *win);
    
    void addclient(int sock);

    virtual void addObserver(Model* observer);

    virtual void addObserver(SendController* observer);

    virtual void notifyObservers(char key);

    void setserver();

    void run();
};