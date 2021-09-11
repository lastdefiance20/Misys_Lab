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

class RecvController: public KeyPublisher{
  public:
    queue<char> keys;
    Model* Mobservers[2];
    int nMobservers = 0;
    int sock_client;

    void addclient(int sock);

    virtual void addObserver(Model* observer);

    virtual void notifyObservers(char key);

    void run();
};