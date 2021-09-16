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

class SendController: public KeyObserver{
  public:
    queue<char> keys;
    mutex m;
    condition_variable cv;
    int sock_client;
    bool isServerSender = false;

    void addclient(int sock);

    virtual void update(char key);

    char read();

    void setserver();

    void setmain();

    void run();
};